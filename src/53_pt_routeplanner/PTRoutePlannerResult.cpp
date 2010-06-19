
/** PTRoutePlannerResult class implementation.
	@file PTRoutePlannerResult.cpp

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

#include "PTRoutePlannerResult.h"
#include "Journey.h"
#include "Edge.h"
#include "Crossing.h"
#include "ResultHTMLTable.h"
#include "LineStop.h"
#include "Road.h"
#include "City.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "CommercialLine.h"
#include "RoadPlace.h"
#include "Line.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace graph;
	using namespace road;
	using namespace geography;
	using namespace algorithm;
	using namespace html;
	using namespace pt;

	namespace ptrouteplanner
	{
		PTRoutePlannerResult::PTRoutePlannerResult(
			const geography::Place* departurePlace,
			const geography::Place* arrivalPlace,
			bool samePlaces,
			const TimeSlotRoutePlanner::Result& journeys
		):	_departurePlace(departurePlace),
			_arrivalPlace(arrivalPlace),
			_samePlaces(samePlaces),
			_journeys(journeys)
		{
			_buildPlacesList();
		}



		void PTRoutePlannerResult::operator=( const PTRoutePlannerResult& other )
		{
			assert(_departurePlace == other._departurePlace);
			assert(_arrivalPlace == other._arrivalPlace);
			_journeys = other._journeys;
			_orderedPlaces = other._orderedPlaces;
			_samePlaces = other._samePlaces;
		}



		void PTRoutePlannerResult::displayHTMLTable(
			ostream& stream,
			optional<HTMLForm&> resaForm,
			const string& resaRadioFieldName
		) const {

			if(_journeys.empty())
			{
				stream << "Aucun r�sultat trouv� de " << (
						dynamic_cast<const NamedPlace*>(_departurePlace) ?
						dynamic_cast<const NamedPlace*>(_departurePlace)->getFullName() :
						dynamic_cast<const City*>(_departurePlace)->getName()
					) << " � " << (
						dynamic_cast<const NamedPlace*>(_arrivalPlace) ?
						dynamic_cast<const NamedPlace*>(_arrivalPlace)->getFullName() :
						dynamic_cast<const City*>(_arrivalPlace)->getName()
					);
				return;
			}

			HTMLTable::ColsVector v;
			v.push_back("D�part<br />" + (
					dynamic_cast<const NamedPlace*>(_departurePlace) ?
					dynamic_cast<const NamedPlace*>(_departurePlace)->getFullName() :
					dynamic_cast<const City*>(_departurePlace)->getName()
			)	);
			v.push_back("Ligne");
			v.push_back("Arriv�e");
			v.push_back("Correspondance");
			v.push_back("D�part");
			v.push_back("Ligne");
			v.push_back("Arriv�e<br />" + (
					dynamic_cast<const NamedPlace*>(_arrivalPlace) ?
					dynamic_cast<const NamedPlace*>(_arrivalPlace)->getFullName() :
					dynamic_cast<const City*>(_arrivalPlace)->getName()
			)	);
			HTMLTable t(v, ResultHTMLTable::CSS_CLASS);

			// Solutions display loop
			int solution(1);
			ptime now(second_clock::local_time());
			stream << t.open();
			bool firstReservableFound(false);
			for (PTRoutePlannerResult::Journeys::const_iterator it(_journeys.begin()); it != _journeys.end(); ++it)
			{
				stream << t.row();
				stream << t.col(7, string(), true);
				if(	resaForm &&
					it->getReservationCompliance() &&
					it->getReservationDeadLine() > now
				){
					string resaTime(to_simple_string(it->getFirstDepartureTime().date()) +" "+to_simple_string(it->getFirstDepartureTime().time_of_day()));
					stream <<
						resaForm->getRadioInput(
							resaRadioFieldName,
							optional<string>(resaTime),
							firstReservableFound ? optional<string>() : resaTime,
							"Solution "+ lexical_cast<string>(solution)
						);
					firstReservableFound = true;
				}
				else
					stream << "Solution " << solution;
				++solution;

				// Departure time
				Journey::ServiceUses::const_iterator its(it->getServiceUses().begin());

				if (it->getContinuousServiceRange().total_seconds() > 60)
				{
					ptime endRange(its->getDepartureDateTime());
					endRange += it->getContinuousServiceRange();
					stream << " - Service continu jusqu'� " << endRange;
				}
				if (it->getReservationCompliance() != false)
				{
					stream << " - ";
					
					if(it->getReservationCompliance() == true)
					{
						stream << HTMLModule::getHTMLImage("resa_compulsory.png", "R�servation obligatoire") << " R�servation obligatoire";
					}
					else
					{
						stream << HTMLModule::getHTMLImage("resa_optional.png", "R�servation facultative") << " R�servation facultative";
					}

					const ptime deadline(it->getReservationDeadLine());
					stream << " avant le " << 
						deadline.date().day() << "/" << deadline.date().month() << "/" << deadline.date().year() << " � " <<
						deadline.time_of_day().hours() << ":" << deadline.time_of_day().minutes()
					;
				}
				if(dynamic_cast<const City*>(_departurePlace) || dynamic_cast<const City*>(_arrivalPlace))
				{
					stream << " (";
					if(dynamic_cast<const City*>(_departurePlace))
					{
						stream << "d�part de " << 
							static_cast<const PublicTransportStopZoneConnectionPlace*>(
								its->getDepartureEdge()->getHub()
							)->getFullName()
						;
					}
					if(dynamic_cast<const City*>(_arrivalPlace))
					{
						if(dynamic_cast<const City*>(_departurePlace)) stream << " - ";
						Journey::ServiceUses::const_iterator ite(it->getServiceUses().end() - 1);
						stream << "arriv�e � " << 
							static_cast<const PublicTransportStopZoneConnectionPlace*>(
								ite->getArrivalEdge()->getHub()
							)->getFullName()
						;
					}
					stream << ")";
				}


				stream << t.row();
				stream << t.col() << "<b>" << its->getDepartureDateTime() << "</b>";

				// Line
				const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
				const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
				stream << t.col(1, ls ? ls->getLine()->getCommercialLine()->getStyle() : string());
				stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getRoadPlace()->getName());

				// Transfers
				if (its == it->getServiceUses().end() -1)
				{
					stream << t.col(4) << "(trajet direct)";
				}
				else
				{
					while(true)
					{
						// Arrival
						stream << t.col() << its->getArrivalDateTime();

						// Place
						stream << t.col();
						if(dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(its->getArrivalEdge()->getHub()))
						{
							stream <<
								static_cast<const PublicTransportStopZoneConnectionPlace*>(
									its->getArrivalEdge()->getHub()
								)->getFullName();
						}

						// Next service use
						++its;

						// Departure
						stream << t.col() << its->getDepartureDateTime();

						// Line
						const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
						const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
						stream << t.col(1, ls ? ls->getLine()->getCommercialLine()->getStyle() : string());
						stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getRoadPlace()->getName());

						// Exit if last service use
						if (its == it->getServiceUses().end() -1)
							break;

						// Empty final arrival col
						stream << t.col();

						// New row and empty origin departure cols;
						stream << t.row();
						stream << t.col();
						stream << t.col();
					}
				}

				// Final arrival
				stream << t.col() << "<b>" << its->getArrivalDateTime() << "</b>";
			}
			stream << t.close();

		}



		void PTRoutePlannerResult::removeFirstJourneys( size_t value )
		{
			for(size_t i(0); i<value; ++i)
			{
				_journeys.pop_front();
			}
		}



		void PTRoutePlannerResult::_buildPlacesList()
		{
			_orderedPlaces.clear();

			for(Journeys::const_iterator itj(_journeys.begin()); itj != _journeys.end(); ++itj)
			{
				const Journey::ServiceUses& jl(itj->getServiceUses());
				vector<PlacesList::iterator> placePositions;
				PlacesList::iterator minPos(_orderedPlaces.begin());

				for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
				{
					const ServiceUse& leg(*itl);

					if(itl == jl.begin())
					{
						PlacesList::iterator pos(
							_putPlace(
								PlacesList::value_type(
									dynamic_cast<const Crossing*>(leg.getDepartureEdge()->getHub()) ?
										dynamic_cast<const NamedPlace*>(_departurePlace) :
										dynamic_cast<const NamedPlace*>(leg.getDepartureEdge()->getHub()),
									true,
									false
								), minPos
						)	);
						placePositions.push_back(pos);
						minPos = ++pos;
					}

					if(	!dynamic_cast<const Crossing*>(leg.getArrivalEdge()->getHub()) ||
						itl+1 == jl.end()
					){
						PlacesList::iterator pos(
							_putPlace(
								PlacesList::value_type(
									dynamic_cast<const Crossing*>(leg.getArrivalEdge()->getHub()) ?
										dynamic_cast<const NamedPlace*>(_arrivalPlace) :
										dynamic_cast<const NamedPlace*>(leg.getArrivalEdge()->getHub()),
									false,
									itl+1 == jl.end()
								), minPos
						)	);
						placePositions.push_back(pos);
						minPos = ++pos;
					}
				}
				_journeysPlacePositions.insert(make_pair(itj, placePositions));
			}
		}



		PTRoutePlannerResult::PlacesList::iterator PTRoutePlannerResult::_putPlace(
			PlacesList::value_type value,
			PlacesList::iterator minPos
		){
			if(!_orderedPlaces.empty())
			{
				PlacesList::iterator testPos;
				// Search of the place after the minimal position
				for(testPos = minPos;
					testPos != _orderedPlaces.end() && testPos->place != value.place;
					++testPos) ;

				// If found, return of the position
				if(testPos != _orderedPlaces.end())
				{
					return testPos;
				}

				// If not found, search of the place before the minimal position
				if(minPos != _orderedPlaces.begin())
				{
					testPos = minPos;
					for(--testPos;
						testPos != _orderedPlaces.begin() && testPos->place != value.place;
						--testPos) ;

					// If found, try to swap items
					if(testPos != _orderedPlaces.begin() && _canBeSwapped(testPos, minPos))
					{
						_swap(testPos, minPos);
						return testPos;
					}
				}
			}

			// Else insert a new row
			PlacesList::iterator position;
			if(value.isDestination)
			{
				position = _orderedPlaces.end();
				PlacesList::iterator previous(position);
				--previous;
				while(previous->place == _arrivalPlace)
				{
					--position;
					--previous;
				}
			}
			else if(value.isOrigin)
			{
				position = minPos;
				while(position != _orderedPlaces.end() && position->place == _departurePlace)
				{
					++position;
				}
			}
			else
			{
				position = minPos;
			}
			return _orderedPlaces.insert(
				position,
				value
			);
		}



		PTRoutePlannerResult::PlacesList::iterator PTRoutePlannerResult::_getHighestPosition(
			PlacesList::iterator source,
			PlacesList::iterator target
		) const {
			PlacesList::iterator result(source);
			PlacesList::const_iterator brake(_orderedPlaces.end());
			BOOST_FOREACH(const JourneysPlacePositions::value_type& its, _journeysPlacePositions)
			{
				const JourneysPlacePositions::mapped_type& sequence(its.second);

				// Search of the source iterator in the sequence
				JourneysPlacePositions::mapped_type::const_iterator curPos;
				for(curPos = sequence.begin(); curPos != sequence.end() && *curPos != source; ++curPos) ;

				// If not found or if it is the last place, ok
				if(curPos == sequence.end() || curPos + 1 == sequence.end())
				{
					continue;
				}

				// If found, search where the row can be pushed
				PlacesList::iterator itNewPos;
				for(itNewPos = (*curPos);
					itNewPos != brake && itNewPos != *(curPos+1);
					++itNewPos)
				{
					result = itNewPos;
					++result;
					if(result == target)
					{
						break;
					}
				}
				
				// If next element of the journey was found, it is the new brake
				if(itNewPos == *(curPos + 1))
				{
					brake = *(curPos + 1);
					continue;
				}
			}

			return result;
		}



		bool PTRoutePlannerResult::_canBeSwapped(
			PlacesList::iterator source,
			PlacesList::iterator target
		) const {
			PlacesList::iterator maxPos(_getHighestPosition(source, target));
			if(maxPos == target)
			{
				return true;
			}
			PlacesList::iterator nextMaxPos(maxPos);
			++nextMaxPos;
			if(maxPos != target && nextMaxPos != _orderedPlaces.end() && nextMaxPos != target)
			{
				return _canBeSwapped(maxPos, target);
			}
			return false;
		}



		void PTRoutePlannerResult::_swap(
			PlacesList::iterator source,
			PlacesList::iterator target
		){
			PlacesList::iterator maxPos(_getHighestPosition(source, target));
			if(maxPos == target)
			{
				_orderedPlaces.splice(target, _orderedPlaces, source);
				return;
			}
			if(maxPos != target)
			{
				_swap(maxPos, target);
				_orderedPlaces.splice(maxPos, _orderedPlaces, source);
			}
		}
	}
}
