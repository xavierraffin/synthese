
/** OSMFileFormat class implementation.
	@file OSMFileFormat.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "OSMExpatParser.h"
#include "OSMFileFormat.hpp"

#include "AdminFunctionRequest.hpp"
#include "AllowedUseRule.h"
#include "CityTableSync.h"
#include "Crossing.h"
#include "CrossingTableSync.hpp"
#include "DataSource.h"
#include "DataSourceAdmin.h"
#include "EdgeProjector.hpp"
#include "ForbiddenUseRule.h"
#include "FrenchPhoneticString.h"
#include "PropertiesHTMLTable.h"
#include "ReverseRoadChunk.hpp"
#include "RoadPlace.h"
#include "RoadPlaceTableSync.h"
#include "RoadTableSync.h"
#include "RoadChunk.h"
#include "RoadChunkTableSync.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/prep/PreparedGeometry.h>
#include <geos/operation/distance/DistanceOp.h>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace geos::operation;
using namespace geos::geom::prep;

namespace synthese 
{
	using namespace admin;
	using namespace algorithm;
	using namespace db;
	using namespace geography;
	using namespace graph;
	using namespace html;
	using namespace impex;
	using namespace osm;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, road::OSMFileFormat>::FACTORY_KEY("OpenStreetMap");
	}

	namespace road
	{
		const string OSMFileFormat::Importer_::PARAMETER_ADD_CENTRAL_CHUNK_REFERENCE("add_central_chunk_reference");

		bool OSMFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& stream,
			boost::optional<const server::Request&> request
		) const {
			NetworkPtr network;
			boost::filesystem::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);
			boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
			if(!file.good())
			{
				throw std::runtime_error("unable to open file");
			}
			std::string ext = boost::filesystem::extension(filePath);
			ExpatParser parser;
			if(ext == ".bz2")
			{
				in.push(boost::iostreams::bzip2_decompressor());
				in.push(file);
				std::istream data(&in);
				network = parser.parse(data);
			}
			else
			{
				network = parser.parse(file);
			}

			network->consolidate(true);

			util::Log::GetInstance().info("finished parsing osm xml");

			// TODO: use a typedef
			// FIXME: valgrind shows a leak from here.
			std::map<int, std::pair<RelationPtr, std::map<int, WayPtr> > > waysByBoundaries =
					network->getWaysByAdminBoundary(8);

			util::Log::GetInstance().info("extracted ways by boundary");

			typedef std::map<int, NodePtr> NodesMap;
			typedef std::vector<std::pair<osm::NodePtr, Point*> > HousesNodesWithGeom;
			HousesNodesWithGeom housesNodesWithGeom;

			BOOST_FOREACH(const NodesMap::value_type& nodePair, *network->getNodes())
			{
				NodePtr node = nodePair.second;

				if(node->hasTag("addr:housenumber") && node->hasTag("addr:street"))
				{
					// Compute the house geometry
					shared_ptr<Point> houseCoord(_dataSource.getActualCoordinateSystem().createPoint(node->getLon(), node->getLat()));
					housesNodesWithGeom.push_back(make_pair(node, static_cast<Point*>(houseCoord->clone())));
				}
			}

			// cities, places and roads
			// TODO: move to osm module.
			typedef pair<int, std::pair<RelationPtr, std::map<int, WayPtr> > > BoundaryType;
			typedef pair<int, double> ClosestWayFromCentroid;

			BOOST_FOREACH(const BoundaryType& boundary_ways, waysByBoundaries)
			{
				// insert city
				// TODO: extract into a _getOrCreateCity method.
				ClosestWayFromCentroid closestWayFromCentroid;
				RelationPtr boundary = boundary_ways.second.first;
				string cityId("0");
				if(boundary->hasTag("ref:INSEE"))
					cityId = boundary->getTag("ref:INSEE");
				Geometry* centroid = boundary->toGeometry()->getCentroid();
				std::string cityCode = cityId;
				std::string cityName = to_upper_copy(lexical_matcher::FrenchPhoneticString::to_plain_lower_copy(boundary->getTag(Element::TAG_NAME)));
				util::Log::GetInstance().info("treating ways of boundary " + cityName);
				CityTableSync::SearchResult cities = CityTableSync::Search(
					_env,
					boost::optional<std::string>(), // exactname
					((cityId != "0") ? boost::optional<std::string>() : boost::optional<std::string>(cityName)), // likeName
					((cityId != "0") ? boost::optional<std::string>(cityId) : boost::optional<std::string>()),
					0, 0, true, true,
					util::UP_LINKS_LOAD_LEVEL // code
				);
				shared_ptr<City> city;

				if(cities.empty())
				{
					city = boost::shared_ptr<City>(new City);
					city->setName(cityName);
					city->setCode(cityCode);
					city->setKey(CityTableSync::getId());
					_env.getEditableRegistry<City>().add(city);
					closestWayFromCentroid = make_pair(0, 9999.9);
				}
				else
				{
					city = cities.front();
					closestWayFromCentroid = make_pair(0, 0);
				}

				// The Synthese <-> OSM objects mapping is done in the following way:
				// 1:n OSM ways with the same name and on the same city (case and accents insensitive) -> 1 RoadPlace
				// OSM way -> 1 Road
				// 1:n OSM nodes between start/end/intersection node -> 1 RoadChunk

				// insert ways of city
				// TODO: move to osm module
				typedef std::pair<int, WayPtr> WayType;
				BOOST_FOREACH(const WayType& w, boundary_ways.second.second)
				{
					WayPtr way = w.second;

					shared_ptr<RoadPlace> roadPlace = _getOrCreateRoadPlace(way, city);

					// Create Road
					shared_ptr<MainRoadPart> road(new MainRoadPart(0, way->getAssociatedRoadType()));

					road->setRoadPlace(*roadPlace);
					road->setKey(RoadTableSync::getId());
					_env.getEditableRegistry<MainRoadPart>().add(road);
					_recentlyCreatedRoadParts[way->getId()] = road;

					if(_addCentralChunkReference)
					{
						double distance = distance::DistanceOp::distance(*centroid, *way->toGeometry()->getCentroid());
						if(closestWayFromCentroid.second > distance)
							closestWayFromCentroid = make_pair(way->getId(), distance);
					}

					double maxSpeed = way->getAssociatedSpeed();

					TraficDirection traficDirection = TWO_WAYS;
					if(way->hasTag("highway"))
					{
						if(way->getTag("highway") == "motorway")
							traficDirection = ONE_WAY;
						else if(way->getTag("highway") == "motorway_link")
							traficDirection = ONE_WAY;
					}

					if(way->hasTag("oneway"))
					{
						if(way->getTag("oneway") == "yes")
							traficDirection = ONE_WAY;
						else if(way->getTag("oneway") == "true")
							traficDirection = ONE_WAY;
						else if(way->getTag("oneway") == "1")
							traficDirection = ONE_WAY;
						else if(way->getTag("oneway") == "-1")
							traficDirection = REVERSED_ONE_WAY;
						else if(way->getTag("oneway") == "no")
							traficDirection = TWO_WAYS;
						else if(way->getTag("oneway") == "0")
							traficDirection = TWO_WAYS;
						else if(way->getTag("oneway") == "false")
							traficDirection = TWO_WAYS;
					}

					if(way->hasTag("junction") && (way->getTag("junction") == "roundabout"))
						traficDirection = ONE_WAY;

					bool nonWalkableWay(!way->isWalkable());

					// TODO: move to OSM module
					typedef std::list<std::pair<int, NodePtr> > NodeList;
					const NodeList* nodes = way->getNodes();

					if(nodes->size() < 2)
					{
						util::Log::GetInstance().warn("Ignoring way with less than 2 nodes");
						continue;
					}

					const GeometryFactory& geometryFactory(CoordinatesSystem::GetDefaultGeometryFactory());
					shared_ptr<CoordinateSequence> cs(geometryFactory.getCoordinateSequenceFactory()->create(0, 2));
					shared_ptr<Crossing> startCrossing;
					size_t rank(0);
					MetricOffset metricOffset(0);

					int nodeCount(nodes->size());
					int i(0);
					BOOST_FOREACH(const NodeList::value_type& idAndNode, *nodes)
					{
						NodePtr node = idAndNode.second;
						i++;

						shared_ptr<Point> point(CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
							*_dataSource.getActualCoordinateSystem().createPoint(
								node->getLon(),
								node->getLat()
						)	)	);

						cs->add(*point->getCoordinate());

						if(!startCrossing.get())
						{
							startCrossing = _getOrCreateCrossing(node, point);
							continue;
						}

						bool isLast = i == nodeCount;
						if(!node->isStop() && node->numConnectedWay() <= 1 && !isLast)
						{
							// Just extend the current geometry.
							continue;
						}

						shared_ptr<LineString> roadChunkLine(geometryFactory.createLineString(*cs));

						_createRoadChunk(road, startCrossing, roadChunkLine, rank, metricOffset, traficDirection, maxSpeed, nonWalkableWay);

						metricOffset += roadChunkLine->getLength();
						startCrossing = _getOrCreateCrossing(node, point);

						if(!isLast)
						{
							cs.reset(geometryFactory.getCoordinateSequenceFactory()->create(0, 2));
							cs->add(*point->getCoordinate());
						}
						++rank;
					}

					// Add last road chunk.
					_createRoadChunk(road, startCrossing, optional<shared_ptr<LineString> >(), rank, metricOffset, traficDirection, maxSpeed, nonWalkableWay);
				}

				const PreparedGeometry* cityGeom = boundary->toPreparedGeometry().get();
				BOOST_FOREACH(HousesNodesWithGeom::value_type& nodePair, housesNodesWithGeom)
				{
					NodePtr node = nodePair.first;
					if(!cityGeom->contains(nodePair.second))
					{
						continue;
					}

					_RecentlyCreatedRoadPlaces::iterator it(_recentlyCreatedRoadPlaces.find(cityName + string(" ") + _toAlphanumericString(node->getTag("addr:street"))));
					if(it != _recentlyCreatedRoadPlaces.end())
					{
						shared_ptr<RoadPlace> refRoadPlace;
						refRoadPlace = it->second;
						std::vector<MainRoadChunk*> refRoadChunks;

						// Get every road chunk of the RoadPlace
						BOOST_FOREACH(Path* path, refRoadPlace->getPaths())
						{
							if(!dynamic_cast<MainRoadPart*>(path))
								continue;

							BOOST_FOREACH(Edge* edge, path->getEdges())
							{
								refRoadChunks.push_back(static_cast<MainRoadChunk*>(edge));
							}
						}

						_projectHouseAndUpdateChunkHouseNumberBounds(node, refRoadChunks, true);
					}
				}

				if(_addCentralChunkReference && closestWayFromCentroid.first)
				{
					_RecentlyCreatedRoadParts::iterator centralRoad = _recentlyCreatedRoadParts.find(closestWayFromCentroid.first);

					if(centralRoad != _recentlyCreatedRoadParts.end())
					{
						city->addIncludedPlace(*static_cast<NamedPlace*>(centralRoad->second->getRoadPlace()));
					}
				}
			}

			util::Log::GetInstance().info("finished inserting road network");

			typedef std::map<int, RelationPtr> ChunkRelations;

			// Loop over relations
			BOOST_FOREACH(ChunkRelations::value_type rel, *(network->getRelations()))
			{
				// If it's a restriction and if it's having a restriction tag
				if(rel.second->hasTag("type") && (rel.second->getTag("type") == "restriction") && rel.second->hasTag("restriction"))
				{
					string tag(rel.second->getTag("restriction"));

					// Get standard from / via / to elements of the restriction
					string role("via");
					list<NodePtr> viaList = rel.second->getNodes(role);
					role = string("from");
					list<WayPtr> fromList = rel.second->getWays(role);
					role = string("to");
					list<WayPtr> toList = rel.second->getWays(role);

					// If we have all three of these
					if(!(viaList.empty() || fromList.empty() || toList.empty()))
					{
						// Trying to find SYNTHESE objects created above
						_CrossingsMap::iterator via = _crossingsMap.find(viaList.front()->getId());
						_RecentlyCreatedRoadParts::iterator from = _recentlyCreatedRoadParts.find(fromList.front()->getId());
						_RecentlyCreatedRoadParts::iterator to = _recentlyCreatedRoadParts.find(toList.front()->getId());

						// If we find them
						if(via != _crossingsMap.end() && from != _recentlyCreatedRoadParts.end() && to != _recentlyCreatedRoadParts.end())
						{
							// If it's a "simple" restriction, mark the road pair as unreachable in the crossing
							if((tag == "no_left_turn") || (tag == "no_right_turn") || (tag == "no_straight_on") || (tag == "no_u_turn"))
							{
								via->second->addNonReachableRoad(make_pair(from->second.get(), to->second.get()));
							}
							// If it's a "only" restriction, run through every ways connected to the "via" node.
							else if((tag == "only_right_turn") || (tag == "only_left_turn") || (tag == "only_straight_on"))
							{
								NodePtr intersection = network->getNode(viaList.front()->getId());

								BOOST_FOREACH(Way* curWay, intersection->getWays())
								{
									// If it's not the "to" way of the "only" restriction, mark the road pair as unreachable in the crossing
									if(curWay->getId() != toList.front()->getId())
									{
										_RecentlyCreatedRoadParts::iterator toRestrict = _recentlyCreatedRoadParts.find(curWay->getId());
										if(toRestrict != _recentlyCreatedRoadParts.end())
										{
											via->second->addNonReachableRoad(make_pair(from->second.get(), toRestrict->second.get()));	
										}
									}
								}
							}
						}
					}
				}
				// If it's an associatedStreet (relation between one or many ways and nodes which are house numbers)
				else if(rel.second->hasTag("type") && (rel.second->getTag("type") == "associatedStreet"))
				{
					// Get all the ways of the relation
					string role("street");
					list<WayPtr> waysList = rel.second->getWays(role);

					// If there is at least one
					if(!waysList.empty())
					{
						shared_ptr<RoadPlace> refRoadPlace;
						// Loop over every ways
						BOOST_FOREACH(WayPtr curWay, waysList)
						{
							// If we find a road place linked to this way
							_LinkBetweenWayAndRoadPlaces::iterator itWay(_linkBetweenWayAndRoadPlaces.find(curWay->getId()));
							if(itWay != _linkBetweenWayAndRoadPlaces.end())
							{
								refRoadPlace = itWay->second;
								break;
							}
						}

						// If we have our road place
						if(refRoadPlace.get())
						{
							role = string("house");
							list<NodePtr> housesList = rel.second->getNodes(role);

							std::vector<MainRoadChunk*> refRoadChunks;

							// Get every road chunk of the RoadPlace
							BOOST_FOREACH(Path* path, refRoadPlace->getPaths())
							{
								if(!dynamic_cast<MainRoadPart*>(path))
									continue;

								BOOST_FOREACH(Edge* edge, path->getEdges())
								{
									refRoadChunks.push_back(static_cast<MainRoadChunk*>(edge));
								}
							}

							// Get all the houses
							BOOST_FOREACH(NodePtr house, housesList)
							{
								if(house->hasTag("addr:housenumber"))
								{
									_projectHouseAndUpdateChunkHouseNumberBounds(house, refRoadChunks, false);
								}
							}

							BOOST_FOREACH(MainRoadChunk* chunk, refRoadChunks)
							{
								_updateHouseNumberingPolicyAccordingToAssociatedHouseNumbers(chunk);
							}
						}
					}
				}
			}

			util::Log::GetInstance().info("finished parsing relation");

			BOOST_FOREACH(const Registry<MainRoadPart>::value_type& road, _env.getEditableRegistry<MainRoadPart>())
			{
				road.second->validateGeometry();
			}

			util::Log::GetInstance().info("finished validating road geometries");
			return true;
		}



		OSMFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::DataSource& dataSource
		):	Importer(env, dataSource),
			OneFileTypeImporter<OSMFileFormat>(env, dataSource)
		{}



		void OSMFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const server::Request& request
		) const {
			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.title("Fichier");
			stream << t.cell("Fichier", t.getForm().getTextInput(PARAMETER_PATH, _pathsSet.empty() ? string() : _pathsSet.begin()->file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Association ville / chunk central", t.getForm().getOuiNonRadioInput(PARAMETER_ADD_CENTRAL_CHUNK_REFERENCE, false));
			/*
			stream << t.cell("Effacer données existantes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_CLEAN_OLD_DATA, _cleanOldData));
			stream << t.cell("Ne pas importer données anciennes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_FROM_TODAY, _fromToday));
			stream << t.cell("Calendrier",
				t.getForm().getSelectInput(
				PTDataCleanerFileFormat::PARAMETER_CALENDAR_ID,
				CalendarTemplateTableSync::GetCalendarTemplatesList(),
				optional<RegistryKeyType>(_calendarTemplate.get() ? _calendarTemplate->getKey() : RegistryKeyType(0))
				)	);
			*/
			stream << t.close();
		}



		util::ParametersMap OSMFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result;

			result.insert(PARAMETER_ADD_CENTRAL_CHUNK_REFERENCE, _addCentralChunkReference);

			return result;
		}



		void OSMFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			_addCentralChunkReference = map.getDefault<bool>(PARAMETER_ADD_CENTRAL_CHUNK_REFERENCE, false);
		}



		db::DBTransaction OSMFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			BOOST_FOREACH(const Registry<City>::value_type& city, _env.getEditableRegistry<City>())
			{
				CityTableSync::Save(city.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<Crossing>::value_type& crossing, _env.getEditableRegistry<Crossing>())
			{
				CrossingTableSync::Save(crossing.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<RoadPlace>::value_type& roadplace, _env.getEditableRegistry<RoadPlace>())
			{
				RoadPlaceTableSync::Save(roadplace.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<MainRoadPart>::value_type& road, _env.getEditableRegistry<MainRoadPart>())
			{
				RoadTableSync::Save(road.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<MainRoadChunk>::value_type& roadChunk, _env.getEditableRegistry<MainRoadChunk>())
			{
				RoadChunkTableSync::Save(roadChunk.second.get(), transaction);
			}
			return transaction;
		}



		shared_ptr<RoadPlace> OSMFileFormat::Importer_::_getOrCreateRoadPlace(
			WayPtr& way,
			shared_ptr<City> city
		) const {
			string roadName;
			string plainRoadName;

			// Search for a recently created road place
			if(way->hasTag(Element::TAG_NAME))
			{
				roadName = way->getTag(Element::TAG_NAME);
				plainRoadName = _toAlphanumericString(roadName);
			}

			if(!roadName.empty())
			{
				_RecentlyCreatedRoadPlaces::iterator it(_recentlyCreatedRoadPlaces.find(city->getName() + string(" ") + plainRoadName));
				if(it != _recentlyCreatedRoadPlaces.end())
				{
					_linkBetweenWayAndRoadPlaces[way->getId()] = it->second;
					return it->second;
				}
			}

			shared_ptr<RoadPlace> roadPlace;
			roadPlace = shared_ptr<RoadPlace>(new RoadPlace);
			roadPlace->setCity(city.get());
			roadPlace->setName(roadName);
			roadPlace->setKey(RoadPlaceTableSync::getId());
			_env.getEditableRegistry<RoadPlace>().add(roadPlace);
			if(!roadName.empty())
			{
				_recentlyCreatedRoadPlaces[city->getName() + string(" ") + plainRoadName] = roadPlace;
			}
			_linkBetweenWayAndRoadPlaces[way->getId()] = roadPlace;
			return roadPlace;
		}



		/*
		 * creates or retrieves an existing crossing for a node
		 */
		shared_ptr<Crossing> OSMFileFormat::Importer_::_getOrCreateCrossing(
			NodePtr &node,
			shared_ptr<Point> position
		) const {
			_CrossingsMap::const_iterator it = _crossingsMap.find(node->getId());
			if(it != _crossingsMap.end())
			{
				return it->second;
			}

			shared_ptr<Crossing> crossing(
				new Crossing(
					CrossingTableSync::getId(),
					position,
					lexical_cast<string>(node->getId()),
					&_dataSource
			)	);

			_crossingsMap[node->getId()] = crossing;
			_env.getEditableRegistry<Crossing>().add(crossing);
			return crossing;
		}



		void OSMFileFormat::Importer_::_createRoadChunk(
			const shared_ptr<MainRoadPart> road,
			const shared_ptr<Crossing> crossing,
			const optional<shared_ptr<LineString> > geometry,
			size_t rank,
			MetricOffset metricOffset,
			TraficDirection traficDirection,
			double maxSpeed,
			bool isNonWalkable
		) const {
			shared_ptr<MainRoadChunk> roadChunk(new MainRoadChunk);
			roadChunk->setRoad(road.get());
			roadChunk->setFromCrossing(crossing.get());
			roadChunk->setRankInPath(rank);
			roadChunk->setMetricOffset(metricOffset);
			roadChunk->setKey(RoadChunkTableSync::getId());
			if(geometry)
			{
				roadChunk->setGeometry(*geometry);
			}

			road->addRoadChunk(*roadChunk);

			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			if(isNonWalkable)
				rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			else
				rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_CAR - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();

			if(traficDirection == ONE_WAY)
			{
				roadChunk->setRules(rules);
				rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
				roadChunk->getReverseRoadChunk()->setRules(rules);
			}
			else if(traficDirection == REVERSED_ONE_WAY)
			{
				roadChunk->getReverseRoadChunk()->setRules(rules);
				rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
				roadChunk->setRules(rules);
			}
			else
			{
				roadChunk->getReverseRoadChunk()->setRules(rules);
				roadChunk->setRules(rules);
			}

			roadChunk->setCarSpeed(maxSpeed);
			roadChunk->getReverseRoadChunk()->setCarSpeed(maxSpeed);
			_env.getEditableRegistry<MainRoadChunk>().add(roadChunk);
		}

		void OSMFileFormat::Importer_::_projectHouseAndUpdateChunkHouseNumberBounds(
			const NodePtr& house,
			vector<MainRoadChunk*>& refRoadChunks,
			const bool autoUpdatePolicy
		) const {
			try
			{
				MainRoadChunk::HouseNumber num = lexical_cast<MainRoadChunk::HouseNumber>(house->getTag("addr:housenumber"));
				// Compute the house geometry
				shared_ptr<Point> houseCoord(CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
					*_dataSource.getActualCoordinateSystem().createPoint(
						house->getLon(),
						house->getLat()
					)
				));

				try
				{
					// Use Projector to get the closest road chunk according to the geometry 
					EdgeProjector<MainRoadChunk*> projector(refRoadChunks, 200);
					EdgeProjector<MainRoadChunk*>::PathNearby projection(projector.projectEdge(*houseCoord->getCoordinate()));
					MainRoadChunk* linkedRoadChunk(projection.get<1>());

					_chunkHouseNumberList[linkedRoadChunk->getKey()].push_back(num);

					MainRoadChunk::HouseNumberBounds leftBounds = linkedRoadChunk->getLeftHouseNumberBounds();

					// If we haven't set any bounds, we set a default one
					if(!leftBounds)
					{
						MainRoadChunk::HouseNumberBounds bounds(make_pair(num, num));
						linkedRoadChunk->setLeftHouseNumberBounds(bounds);
						linkedRoadChunk->setRightHouseNumberBounds(bounds);
					}
					else
					{
						// If there is one and the lower bounds is higher than the current house number, update
						if(num < leftBounds->first)
						{
							MainRoadChunk::HouseNumberBounds bounds(make_pair(num, leftBounds->second));
							linkedRoadChunk->setLeftHouseNumberBounds(bounds);
							linkedRoadChunk->setRightHouseNumberBounds(bounds);
						}
						// Or the upper bounds is lower than the current house number, update
						else if(num > leftBounds->second)
						{
							MainRoadChunk::HouseNumberBounds bounds(make_pair(leftBounds->first, num));
							linkedRoadChunk->setLeftHouseNumberBounds(bounds);
							linkedRoadChunk->setRightHouseNumberBounds(bounds);
						}
					}

					if(autoUpdatePolicy)
					{
						_updateHouseNumberingPolicyAccordingToAssociatedHouseNumbers(linkedRoadChunk);
					}
				}
				catch(EdgeProjector<MainRoadChunk*>::NotFoundException)
				{
				}
			}
			catch(bad_lexical_cast)
			{
			}
		}

		void OSMFileFormat::Importer_::_updateHouseNumberingPolicyAccordingToAssociatedHouseNumbers(
			MainRoadChunk* chunk
		) const {
			ChunkHouseNumberList::iterator itChunk = _chunkHouseNumberList.find(chunk->getKey());

			if(itChunk != _chunkHouseNumberList.end())
			{
				MainRoadChunk::HouseNumberingPolicy policy(MainRoadChunk::ALL);

				if(itChunk->second.size() > 2)
				{
					unsigned int curMod((*itChunk->second.begin()) % 2);
					bool multipleMod(false);

					BOOST_FOREACH(MainRoadChunk::HouseNumber n, itChunk->second)
					{
						if(curMod != (n % 2))
						{
							multipleMod = true;
							break;
						}
					}

					if(!multipleMod && curMod)
						policy = MainRoadChunk::ODD;
					else if(!multipleMod)
						policy = MainRoadChunk::EVEN;
				}

				chunk->setLeftHouseNumberingPolicy(policy);
				chunk->setRightHouseNumberingPolicy(policy);
			}
		}

		std::string OSMFileFormat::Importer_::_toAlphanumericString(
			const std::string& input
		) const {
			string lowerInput(lexical_matcher::FrenchPhoneticString::to_plain_lower_copy(input));
			stringstream output;
			bool first = true;

			char_separator<char> sep(" :,;.-_|/\\¦'°");
			tokenizer<char_separator<char> > words(lowerInput, sep);
			BOOST_FOREACH(string source, words)
			{
				string curWord(source);

				if(source == "10")
					curWord = "dix";
				else if(source == "11")
					curWord = "onze";
				else if(source == "12")
					curWord = "douze";
				else if(source == "13")
					curWord = "treize";
				else if(source == "14")
					curWord = "quatorze";
				else if(source == "15")
					curWord = "quinze";
				else if(source == "16")
					curWord = "seize";
				else if(source == "17")
					curWord = "dix sept";
				else if(source == "18")
					curWord = "dix huit";
				else if(source == "19")
					curWord = "dix neuf";
				else if(source == "20")
					curWord = "vingt";
				else if(source == "st")
					curWord = "saint";
				else if(source == "ste")
					curWord = "sainte";
				else if(source == "pl")
					curWord = "place";
				else if(source == "av")
					curWord = "avenue";
				else if(source == "imp")
					curWord = "impasse";
				else if(source == "bd")
					curWord = "boulevard";
				else if(source == "fg")
					curWord = "faubourg";
				else if(source == "che")
					curWord = "chemin";
				else if(source == "rte")
					curWord = "route";
				else if(source == "rpt")
					curWord = "rond point";
				else if(source == "dr")
					curWord = "docteur";
				else if(source == "pr")
					curWord = "professeur";
				else if(source == "cdt" || source == "cmdt")
					curWord = "commandant";
				else if(source == "chu" || source == "chr")
					curWord = "hopital";
				else if(source == "fac" || source == "faculte")
					curWord = "universite";
				else if(
					source == "a" ||
					source == "au" ||
					source == "d" ||
					source == "de" ||
					source == "des" ||
					source == "du" ||
					source == "en" ||
					source == "et" ||
					source == "l" ||
					source == "la" ||
					source == "le" ||
					source == "les" ||
					source == "un"
				)
					curWord = string();

				if(curWord.empty())
					continue;
				else
				{
					if(!output.str().empty())
						output << " ";
					output << curWord;
				}
			}

			return output.str();
		}
	}
}
