
/** RoutePlanner class implementation.
	@file RoutePlanner.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "RoutePlanner.h"

#include "IntegralSearcher.h"
#include "BestVertexReachesMap.h"
#include "RoadModule.h"
#include "PTModule.h"
#include "Hub.h"
#include "Edge.h"
#include "Line.h"
#include "Service.h"
#include "Vertex.h"
#include "PhysicalStop.h"
#include "Journey.h"
#include "VertexAccessMap.h"
#include "JourneyComparator.h"
#include "RoadPlace.h"
// To be removed by a log class
#include "LineStop.h"
#include "Road.h"
#include "Line.h"
#include "CommercialLine.h"

#include "SquareDistance.h"

#include "Log.h"

#include <algorithm>
#include <set>
#include <sstream>

#undef max
#undef min

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace time;
	using namespace env;	
	using namespace geography;
	using namespace geometry;
	using namespace util;
	using namespace graph;
	using namespace road;
	using namespace pt;
	

	namespace routeplanner
	{

		RoutePlanner::RoutePlanner(
			const Place* origin,
			const Place* destination,
			const AccessParameters& accessParameters,
			const PlanningOrder& planningOrder,
			const DateTime& journeySheetStartTime,
			const DateTime& journeySheetEndTime
			, int maxSolutionsNumber
			, std::ostream* 			logStream
			, util::Log::Level			logLevel
		)	: _accessParameters (accessParameters)
			, _planningOrder (planningOrder)
			, _journeySheetStartTime (journeySheetStartTime)
			, _journeySheetEndTime (journeySheetEndTime)
			, _maxSolutionsNumber(maxSolutionsNumber)
			, _minDepartureTime(TIME_UNKNOWN)
			, _maxArrivalTime(TIME_UNKNOWN)
			, _previousContinuousServiceLastDeparture(TIME_UNKNOWN)
			, _logStream(logStream)
			, _logLevel(logLevel)
		{
			origin->getVertexAccessMap(
				_originVam
				, DEPARTURE_TO_ARRIVAL
				, accessParameters
				, RoadModule::GRAPH_ID
			);
			origin->getVertexAccessMap(
				_originVam
				, DEPARTURE_TO_ARRIVAL
				, accessParameters
				, PTModule::GRAPH_ID
			);
			destination->getVertexAccessMap(
				_destinationVam
				, ARRIVAL_TO_DEPARTURE
				, accessParameters
				, RoadModule::GRAPH_ID
			);
			destination->getVertexAccessMap(
				_destinationVam
				, ARRIVAL_TO_DEPARTURE
				, accessParameters
				, PTModule::GRAPH_ID
			);
		}


		   
		RoutePlanner::~RoutePlanner ()
		{
		}


// --------------------------------------------------------- Journey sheet calculation

		const RoutePlanner::Result& RoutePlanner::computeJourneySheetDepartureArrival()
		{
			_result.clear();

			// Log
			if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
				|| _logLevel <= Log::LEVEL_TRACE
			){
				stringstream s;
				s << "<h2>Origin access map calculation</h2>";

				if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
					Log::GetInstance().trace(s.str());
				if (_logLevel <= Log::LEVEL_TRACE && _logStream)
					*_logStream << s.str();
			}

			// Control if the departure place and the arrival place have a common point
			_result.samePlaces = false;
			for (VertexAccessMap::VamMap::const_iterator itd(_originVam.getMap().begin()); itd != _originVam.getMap().end(); ++itd)
				for (VertexAccessMap::VamMap::const_iterator ita(_destinationVam.getMap().begin()); ita != _destinationVam.getMap().end(); ++ita)
					if (itd->first->getHub() == ita->first->getHub())
					{
						_result.samePlaces = true;
						return _result;
					}



			// Create origin vam from integral search on roads
			JourneysResult<JourneyComparator> originJourneys;
			BestVertexReachesMap bvrmd(DEPARTURE_TO_ARRIVAL, true);
			DateTime dt3(_journeySheetEndTime);
			DateTime dt4(TIME_UNKNOWN);
			IntegralSearcher iso(
				DEPARTURE_TO_ARRIVAL
				, _accessParameters
				, PTModule::GRAPH_ID
				, RoadModule::GRAPH_ID
				, originJourneys
				, bvrmd
				, _destinationVam
				, dt3
				, 0
				, dt4
				, false
				, false
				, _logStream
				, _logLevel
			);

			// Best arrival at starting places is start time
			for (VertexAccessMap::VamMap::const_iterator it(_originVam.getMap().begin()); it != _originVam.getMap().end(); ++it)
				bvrmd.insert(it->first, _journeySheetStartTime, false);
			
			iso.integralSearch (
				_originVam
				, _journeySheetStartTime
				, Journey()
				, std::numeric_limits<int>::max ()
			);
			
			VertexAccessMap ovam;
			// Include physical stops from originVam into result of integral search
			// (cos not taken into account in returned journey vector).
			ovam.mergeWithFilter(_originVam, PTModule::GRAPH_ID);

			Journey* candidate(new Journey());
			for(JourneysResult<JourneyComparator>::ResultSet::const_iterator itoj(originJourneys.getJourneys().begin());
				itoj != originJourneys.getJourneys().end ();
				++itoj
			){
				const Journey& oj = (**itoj);
			
				// Store each reached physical stop with full approach time addition :
				//	- approach time in departure place
				//	- duration of the approach journey
				//	- transfer delay between approach journey end address and physical stop
				int commonApproachTime(
					_originVam.getVertexAccess(oj.getOrigin()->getFromVertex()).approachTime
					+ oj.getDuration ()
				);
				int commonApproachDistance(
					_originVam.getVertexAccess(oj.getOrigin()->getFromVertex()).approachDistance
					+ oj.getDistance ()
				);
				VertexAccessMap vam;
				const Hub* cp(oj.getDestination()->getHub());
				const Vertex& v(*oj.getDestination()->getFromVertex());
				cp->getVertexAccessMap(
					vam
					, DEPARTURE_TO_ARRIVAL
					, PTModule::GRAPH_ID
					, v
				);
				for (VertexAccessMap::VamMap::const_iterator it(vam.getMap().begin()); it != vam.getMap().end(); ++it)
				{
					if (_destinationVam.contains(it->first))
						continue;

					ovam.insert(
						it->first
						, VertexAccess(
							commonApproachTime + cp->getTransferDelay(v, *it->first)
							, commonApproachDistance
							, oj
						)
					);
				}

				// Store the journey as a candidate if it goes directly to the destination
				if(	_destinationVam.contains(oj.getDestination()->getFromVertex())
					&& oj.isBestThan(*candidate)
				)	*candidate = oj;
			}

			// If a candidate was elected, store it in the result array
			if (!candidate->empty())
				_result.journeys.push_back(candidate);
			else
				delete candidate;

			// Log
			if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
				|| _logLevel <= Log::LEVEL_TRACE
				){
					stringstream s;
					s << "<h3>Origins</h3><table class=\"adminresults\"><tr><th>Connection Place</th><th>Physical Stop</th><th>Dst.</th><th>Time</th></tr>";

					BOOST_FOREACH(VertexAccessMap::VamMap::value_type it, ovam.getMap())
					{
						s	<<
							"<tr><td>" <<
							dynamic_cast<const NamedPlace*>(it.first->getHub())->getFullName() <<
							"</td><td>" << static_cast<const PhysicalStop*>(it.first)->getName() <<
							"</td><td>" << it.second.approachDistance <<
							"</td><td>" << it.second.approachTime <<
							"</td></tr>"
						;
					}
					s << "</table>";

					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
						Log::GetInstance().trace(s.str());
					if (_logLevel <= Log::LEVEL_TRACE && _logStream)
						*_logStream << s.str();
			}

			// Log
			if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
				|| _logLevel <= Log::LEVEL_TRACE
				){
					stringstream s;
					s << "<h2>Destination access map calculation</h2>";

					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
						Log::GetInstance().trace(s.str());
					if (_logLevel <= Log::LEVEL_TRACE && _logStream)
						*_logStream << s.str();
			}

			// Create destination vam from integral search on roads
			JourneysResult<JourneyComparator> destinationJourneys;
			BestVertexReachesMap bvrmo(DEPARTURE_TO_ARRIVAL, true);
			DateTime dt1(_journeySheetStartTime);
			DateTime dt2(TIME_UNKNOWN);
			IntegralSearcher isd(
				ARRIVAL_TO_DEPARTURE
				, _accessParameters
				, PTModule::GRAPH_ID
				, RoadModule::GRAPH_ID
				, destinationJourneys
				, bvrmo
				, _originVam
				, dt1
				, 0
				, dt2
				, false
				, true
				, _logStream
				, _logLevel
			);

			// Best departure at starting places is start time
			for (VertexAccessMap::VamMap::const_iterator it(_destinationVam.getMap().begin()); it != _destinationVam.getMap().end(); ++it)
				bvrmo.insert(it->first, _journeySheetEndTime, false);

			isd.integralSearch (
				_destinationVam
				, _journeySheetEndTime
				, Journey()
				, std::numeric_limits<int>::max ()
			);

			VertexAccessMap dvam;
			// Include physical stops from destinationVam into result of integral search
			// (cos not taken into account in returned journey vector).
			dvam.mergeWithFilter(_destinationVam, PTModule::GRAPH_ID);

			for(JourneysResult<JourneyComparator>::ResultSet::const_iterator itdj(destinationJourneys.getJourneys().begin());
				itdj != destinationJourneys.getJourneys().end ();
				++itdj
			){
				const Journey& j(**itdj);

				// Store each reached physical stop with full approach time addition :
				//	- approach time in destination place
				//	- duration of the approach journey
				//	- transfer delay between approach journey end address and physical stop
				int commonApproachTime(
					_destinationVam.getVertexAccess(j.getDestination()->getFromVertex()).approachTime
					+ j.getDuration ()
				);
				int commonApproachDistance(
					_destinationVam.getVertexAccess(j.getDestination()->getFromVertex()).approachDistance
					+ j.getDistance ()
				);
				VertexAccessMap vam;
				const Hub* cp(j.getOrigin()->getHub());
				const Vertex& v(*j.getOrigin()->getFromVertex());
				cp->getVertexAccessMap(
					vam,
					ARRIVAL_TO_DEPARTURE,
					PTModule::GRAPH_ID,
					v
				);
				for (VertexAccessMap::VamMap::const_iterator it(vam.getMap().begin()); it != vam.getMap().end(); ++it)
				{
					if (!_originVam.contains(it->first))
						dvam.insert(
							it->first
							, VertexAccess(
								commonApproachTime + cp->getTransferDelay(*it->first, v)
								, commonApproachDistance
								, j
							)
						);
				}
			}

			// Log
			if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
				|| _logLevel <= Log::LEVEL_TRACE
				){
					stringstream s;
					s << "<h3>Destinations</h3><table class=\"adminresults\"><tr><th>Connection Place</th><th>Physical Stop</th><th>Dst.</th><th>Time</th></tr>";

					BOOST_FOREACH(VertexAccessMap::VamMap::value_type it, dvam.getMap())
					{
						s	<<
							"<tr><td>" <<
							dynamic_cast<const NamedPlace*>(it.first->getHub())->getFullName() <<
							"</td><td>" <<
							static_cast<const PhysicalStop* const>(it.first)->getName() <<
							"</td><td>" <<
							it.second.approachDistance <<
							"</td><td>" <<
							it.second.approachTime <<
							"</td></tr>"
						;
					}
					s << "</table>";

					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
						Log::GetInstance().trace(s.str());
					if (_logLevel <= Log::LEVEL_TRACE && _logStream)
						*_logStream << s.str();
			}

			if (_result.journeys.empty())
			{
				_previousContinuousServiceDuration = 0;
			}
			else
			{
				const Journey* journey(_result.journeys.front());
				_previousContinuousServiceDuration = journey->getDuration();
				_previousContinuousServiceLastDeparture = journey->getDepartureTime();
				_previousContinuousServiceLastDeparture += journey->getContinuousServiceRange();
			}

			// Time loop
			int solutionNumber(0);
			for(_minDepartureTime = _journeySheetStartTime; 
				(_minDepartureTime <= _journeySheetEndTime
				&&	(_maxSolutionsNumber == UNKNOWN_VALUE 
					|| _maxSolutionsNumber > _result.journeys.size()
					)
				);
			){

				if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
					|| _logLevel <= Log::LEVEL_TRACE
					){
						stringstream s;
						s << "<h2>Route planning " << ++solutionNumber << " at " << _minDepartureTime.toString() << "</h2>";

						if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
							Log::GetInstance().trace(s.str());
						if (_logLevel <= Log::LEVEL_TRACE && _logStream)
							*_logStream << s.str();
				}


				Journey* journey(new Journey());
				computeRoutePlanningDepartureArrival(*journey, ovam, dvam);
				
				//! <li> If no journey was found and last service is continuous, 
					//! then repeat computation after continuous service range. </li>
				if(	!_result.journeys.empty()
				&&	(_result.journeys.back ()->getContinuousServiceRange () > 0)
				&&	(journey->empty() || journey->getDepartureTime() > _previousContinuousServiceLastDeparture)
				){
					_minDepartureTime = _previousContinuousServiceLastDeparture;
					_minDepartureTime += 1;
					_previousContinuousServiceDuration = 0;
					computeRoutePlanningDepartureArrival (*journey, ovam, dvam);	
				}
				
				if (journey->empty())
					break;				
				
				//! <li>If last continuous service was broken, update its range</li>
				if(	!_result.journeys.empty ()
				&&	(_result.journeys.back ()->getContinuousServiceRange () > 0)
				&&	(journey->getDepartureTime () <= _previousContinuousServiceLastDeparture)
				){
					int duration(journey->getArrivalTime() - _result.journeys.back()->getArrivalTime () - 1);
					_result.journeys.back()->setContinuousServiceRange (duration);
				}

				/*!	<li>En cas de nouveau service continu, enregistrement de valeur pour le calcul de la prochaine solution</li>	*/
				if (journey->getContinuousServiceRange() > 1)
				{
					_previousContinuousServiceDuration = journey->getDuration();
					_previousContinuousServiceLastDeparture = journey->getDepartureTime();
					_previousContinuousServiceLastDeparture += journey->getContinuousServiceRange();
				}
				else
					_previousContinuousServiceDuration = 0;


				_result.journeys.push_back(journey);
				
				_minDepartureTime = journey->getDepartureTime ();
				_minDepartureTime += 1;
			}
			
			return _result;
			
		}


// -------------------------------------------------------------------- Column computing

		void RoutePlanner::computeRoutePlanningDepartureArrival(
			Journey& result
			, const VertexAccessMap& ovam
			, const VertexAccessMap& dvam
		){
			_maxArrivalTime = _journeySheetEndTime;
			_maxArrivalTime.addDaysDuration(7);	/// @todo Replace 7 by a parameter
		    
			// Look for best arrival
			findBestJourney(DEPARTURE_TO_ARRIVAL, result, ovam, dvam, _minDepartureTime, false, false);
		    
			if (result.empty() || result.getDepartureTime() > _journeySheetEndTime)
			{
				result.clear();
				return;
			}
		    
			// If a journey was found, try to optimize by delaying departure hour as much as possible.
			// Update bounds
			_minDepartureTime = result.getDepartureTime ();
			_maxArrivalTime = result.getArrivalTime ();

			result.reverse();
		
			// Look for best departure
			findBestJourney(ARRIVAL_TO_DEPARTURE, result, dvam, ovam, _maxArrivalTime, true, true);

			if (result.getDepartureTime() > _journeySheetEndTime)
			{
				result.clear();
				return;
			}

			// Inclusion of approach journeys
			if (result.getStartApproachDuration())
			{
				result.setStartApproachDuration(0);
				Journey goalApproachJourney(
					dvam.getVertexAccess(result.getDestination()->getFromVertex()).approachJourney
				);
				if (!goalApproachJourney.empty())
				{
					goalApproachJourney.shift(
						(result.getArrivalTime() - goalApproachJourney.getDepartureTime()) +
						result.getDestination()->getHub()->getTransferDelay(
							*result.getDestination()->getFromVertex(),
							*goalApproachJourney.getOrigin()->getFromVertex()
						),
						result.getContinuousServiceRange()
					);
					goalApproachJourney.setContinuousServiceRange(result.getContinuousServiceRange());
					result.append(goalApproachJourney);
				}
			}

			result.reverse();

			if (result.getStartApproachDuration())
			{
				result.setStartApproachDuration(0);
				Journey originApproachJourney(
					ovam.getVertexAccess(result.getOrigin()->getFromVertex()).approachJourney
				);
				if (!originApproachJourney.empty())
				{
					originApproachJourney.shift(
						(result.getDepartureTime() - originApproachJourney.getDepartureTime()) -
						originApproachJourney.getDuration() -
						result.getOrigin()->getHub()->getTransferDelay(
							*originApproachJourney.getDestination()->getFromVertex(),
							*result.getOrigin()->getFromVertex()
						),
						result.getContinuousServiceRange()
					);
					originApproachJourney.setContinuousServiceRange(result.getContinuousServiceRange());
					result.prepend(originApproachJourney);
				}
			}
		}

// -------------------------------------------------------------------------- Recursion

		void RoutePlanner::findBestJourney(
			AccessDirection accessDirection,
			Journey& result,
			const VertexAccessMap& startVam,
			const VertexAccessMap& endVam,
			const time::DateTime& startTime,
			bool strictTime,
			bool inverted
		){
			assert(accessDirection != UNDEFINED_DIRECTION);
			assert(result.getMethod() == UNDEFINED_DIRECTION || result.getMethod() == accessDirection);

			JourneysResult<JourneyComparator> todo;
			int integralSerachsNumber(1);
			
			DateTime& bestEndTime((accessDirection == DEPARTURE_TO_ARRIVAL) ? _maxArrivalTime : _minDepartureTime);
			DateTime lastBestEndTime(bestEndTime);
			
			// Initialization of the best vertex reaches map
			BestVertexReachesMap bestVertexReachesMap(accessDirection, strictTime);
			for(VertexAccessMap::VamMap::const_iterator itVertex(startVam.getMap().begin());
				itVertex != startVam.getMap().end();
				++itVertex
			){
				bestVertexReachesMap.insert(
					itVertex->first
					, (accessDirection == DEPARTURE_TO_ARRIVAL)
						? _minDepartureTime + itVertex->second.approachTime
						: _maxArrivalTime - itVertex->second.approachTime
				);
			}
			for(VertexAccessMap::VamMap::const_iterator itVertex(endVam.getMap().begin());
				itVertex != endVam.getMap ().end ();
				++itVertex
			){
				bestVertexReachesMap.insert(
					itVertex->first
					, (accessDirection == DEPARTURE_TO_ARRIVAL)
						? _maxArrivalTime - itVertex->second.approachTime
						: _minDepartureTime + itVertex->second.approachTime
				);
			}

			if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
				|| _logLevel <= Log::LEVEL_TRACE
				){
					stringstream s;
					s << "<h3>Integral search " << integralSerachsNumber << "</h3>";

					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
						Log::GetInstance().trace(s.str());
					if (_logLevel <= Log::LEVEL_TRACE && _logStream)
						*_logStream << s.str();
			}

			// Initialization of the integral searcher
			IntegralSearcher is(
				accessDirection
				, _accessParameters
				, PTModule::GRAPH_ID
				, PTModule::GRAPH_ID
				, todo
				, bestVertexReachesMap
				, endVam
				, bestEndTime
				, _previousContinuousServiceDuration
				, _previousContinuousServiceLastDeparture
				, strictTime
				, inverted
				, _logStream
				, _logLevel
			);

			is.integralSearch(startVam, startTime, Journey(), 0, strictTime);
			++integralSerachsNumber;
			
			while(true)
			{

				// Take into account of the end reached journeys
				for(JourneysResult<JourneyComparator>::ResultSet::const_iterator it(todo.getJourneys().begin());
					it != todo.getJourneys().end();
				){
					JourneysResult<JourneyComparator>::ResultSet::const_iterator next(it);
					++next;
					const Journey& journey(**it);
					
					if (!journey.getEndReached())
						break;
					
					// A destination without any approach time stops the recursion
					const Vertex* reachedVertex(journey.getEndEdge()->getFromVertex());
					const VertexAccess& va = endVam.getVertexAccess(reachedVertex);
					
					// Attempt to elect the solution as the result
					bool saved(journey.isBestThan(result));
					if (saved)
						result = journey;

					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
					{
						Log::GetInstance().trace(
							dynamic_cast<const NamedPlace*>(reachedVertex->getHub())->getFullName() +
							" was found at " +
							journey.getEndTime().toString() +
							(saved ? " (accepted)" : " (rejected)")
						);
						if (_logLevel <= Log::LEVEL_TRACE && _logStream)
						{
							*_logStream <<
								"<p>" <<
								dynamic_cast<const NamedPlace*>(reachedVertex->getHub())->getFullName() <<
								" was found at " << journey.getEndTime().toString() <<
								(saved ? " (accepted)" : " (rejected)") << "</p>"
							;
						}
					}

					if (va.approachTime == 0)
						todo.remove(*it);
										
					it = next;
				}

				todo.cleanup(lastBestEndTime != bestEndTime, bestEndTime, bestVertexReachesMap);

				// Loop exit
				if (todo.empty())
					break;

				if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
					|| _logLevel <= Log::LEVEL_TRACE
				){
					string s("<table class=\"adminresults\">"+todo.getLog());
					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
						Log::GetInstance().trace(s);
					if (_logLevel <= Log::LEVEL_TRACE && _logStream)
						*_logStream << s << "</table>";
				}

				lastBestEndTime = bestEndTime;

				const Journey* journey(todo.front());

				if(	!journey->empty()
					&&	(Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
					|| _logLevel <= Log::LEVEL_TRACE
					)
					){
						stringstream stream;
						stream
							<< "<h3>Integral search " << integralSerachsNumber++ << "</h3>"
							<< "<table class=\"adminresults\"><tr>"
							<< "<th colspan=\"7\">Journey</th>"
							<< "</tr>"
							;

						// Departure time
						Journey::ServiceUses::const_iterator its(journey->getServiceUses().begin());

						/*					if (journey->getContinuousServiceRange() > 1)
						{
						DateTime endRange(its->getDepartureDateTime());
						endRange += journey->getContinuousServiceRange();
						stream << " - Service continu jusqu'� " << endRange.toString();
						}
						if (journey->getReservationCompliance() == true)
						{
						stream << " - R�servation obligatoire avant le " << journey->getReservationDeadLine().toString();
						}
						if (journey->getReservationCompliance() == boost::logic::indeterminate)
						{
						stream << " - R�servation facultative avant le " << journey->getReservationDeadLine().toString();
						}
						*/
						stream << "<tr>";
						stream << "<td>" << its->getDepartureDateTime().toString() << "</td>";

						// Line
						const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
						const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
						stream << "<td";
						if (ls)
							stream << " class=\"" + ls->getLine()->getCommercialLine()->getStyle() << "\"";
						stream << ">";
						stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getRoadPlace()->getName()) << "</td>";

						// Transfers
						if (its == journey->getServiceUses().end() -1)
						{
							stream << "<td colspan=\"4\">(trajet direct)</td>";
						}
						else
						{
							while(true)
							{
								// Arrival
								stream << "<td>" << its->getArrivalDateTime().toString() << "</td>";

								// Place
								stream <<
									"<td>" <<
									dynamic_cast<const NamedPlace*>(its->getArrivalEdge()->getHub())->getFullName() <<
									"</td>"
								;

								// Next service use
								++its;

								// Departure
								stream << "<td>" << its->getDepartureDateTime().toString() << "</td>";

								// Line
								const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
								const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
								stream << "<td";
								if (ls)
									stream << " class=\"" << ls->getLine()->getCommercialLine()->getStyle() << "\"";
								stream << ">";
								stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getRoadPlace()->getName()) << "</td>";

								// Exit if last service use
								if (its == journey->getServiceUses().end() -1)
									break;

								// Empty final arrival col
								stream << "<td></td>";

								// New row and empty origin departure cols;
								stream << "</tr><tr>";
								stream << "<td></td>";
								stream << "<td></td>";
							}
						}

						// Final arrival
						stream << "<td>" << its->getArrivalDateTime().toString() << "</td>";


						string s(todo.getLog());
						if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
						{
							Log::GetInstance().trace(stream.str());
							Log::GetInstance().trace(s);
							Log::GetInstance().trace("</table>");
						}
						if (_logLevel <= Log::LEVEL_TRACE && _logStream)
						{
							*_logStream << stream.str();
							*_logStream << s << "</table>";
						}
				}

				VertexAccessMap vertices;
				const Vertex& vertex(*journey->getEndEdge()->getFromVertex());
				vertex.getHub()->getVertexAccessMap(
					vertices
					, accessDirection
					, PTModule::GRAPH_ID
					, vertex
				);

				is.integralSearch(
					vertices
					, journey->getEndTime()
					, *journey
					, 0
					, false
				);

				delete journey;
			}

			if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
				|| _logLevel <= Log::LEVEL_TRACE
			){
				stringstream s;
				s << "<p>End of findJourney computing. Was made with " << --integralSerachsNumber << " integral searches.</p>";

				if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
					Log::GetInstance().trace(s.str());
				if (_logLevel <= Log::LEVEL_TRACE && _logStream)
					*_logStream << s.str();
			}
		}

		RoutePlanner::Result::~Result()
		{
			clear();
		}

		void RoutePlanner::Result::clear()
		{
			for(JourneyBoardJourneys::iterator it(journeys.begin()); it != journeys.end(); ++it)
				delete *it;
		}
	}
}
