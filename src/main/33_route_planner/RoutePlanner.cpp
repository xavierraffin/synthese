
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

#include "33_route_planner/RoutePlanner.h"
#include "33_route_planner/Journey.h"
#include "33_route_planner/IntegralSearcher.h"

#include "15_env/Axis.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Edge.h"
#include "15_env/Line.h"
#include "15_env/Service.h"
#include "15_env/SquareDistance.h"
#include "15_env/Vertex.h"

#include "15_env/VertexAccessMap.h"

#include "15_env/BikeCompliance.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/PedestrianCompliance.h"
#include "15_env/ReservationRule.h"
#include "15_env/Fare.h"

#include <algorithm>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace time;
	using namespace env;

	namespace routeplanner
	{

		RoutePlanner::RoutePlanner (const Place* origin,
						const Place* destination,
						const AccessParameters& accessParameters,
						const PlanningOrder& planningOrder,
						const DateTime& journeySheetStartTime,
						const DateTime& journeySheetEndTime)
			: _origin (origin)
			, _destination (destination)
			, _accessParameters (accessParameters)
			, _planningOrder (planningOrder)
			, _journeySheetStartTime (journeySheetStartTime)
			, _journeySheetEndTime (journeySheetEndTime)
			, _bestDepartureVertexReachesMap (FROM_ORIGIN)
			, _bestArrivalVertexReachesMap (TO_DESTINATION)
			, _journeyLegComparatorForBestArrival (TO_DESTINATION)
			, _journeyLegComparatorForBestDeparture (FROM_ORIGIN)

		{
			origin->getImmediateVertices (
				_originVam
				, TO_DESTINATION
				, accessParameters
				, SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
			);
			destination->getImmediateVertices(
				_destinationVam
				, FROM_ORIGIN
				, accessParameters
				, SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
			);
		}


		   
		RoutePlanner::~RoutePlanner ()
		{

		}



		const Place* 
		RoutePlanner::getOrigin () const
		{
			return _origin;
		}



		const Place* 
		RoutePlanner::getDestination () const
		{
			return _destination;
		}


		void
		RoutePlanner::findBestJourney (Journey& result
						   , const VertexAccessMap& ovam
						   , const VertexAccessMap& dvam
						   , const AccessDirection& accessDirection
						   , const Journey& currentJourney
						   , bool strictTime
						   , bool optim
		){
			Journey candidate;
		    
			if (currentJourney.getJourneyLegCount () > 
			_accessParameters.maxTransportConnectionCount) return;

			IntegralSearcher is(
				accessDirection
				, _accessParameters
				, DO_NOT_SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
				, DO_NOT_USE_ROADS
				, USE_LINES
				, (accessDirection == TO_DESTINATION) ? _bestArrivalVertexReachesMap : _bestDepartureVertexReachesMap
				, dvam 
				, _calculationTime
				, (accessDirection == TO_DESTINATION) ? _maxArrivalTime : _minDepartureTime
				);
			Journeys journeyParts(is.integralSearch(
				ovam
				, (accessDirection == TO_DESTINATION)
					? (currentJourney.empty() ? _minDepartureTime : currentJourney.getArrivalTime())
					: (currentJourney.empty() ? _maxArrivalTime : currentJourney.getDepartureTime())
				, 0
				, strictTime
				));

			for (Journeys::const_iterator itj = journeyParts.begin ();
			 itj != journeyParts.end (); ++itj)
			{
				bool recursion(true);

				// Case the journey goes to a final destination
				const Vertex* reachedVertex(
					(accessDirection == TO_DESTINATION)
					? itj->getDestination()->getFromVertex()
					: itj->getOrigin()->getFromVertex()
					);
				if (dvam.contains(reachedVertex))
				{
					// A destination without any approach time stops the recursion
					const VertexAccess& va = dvam.getVertexAccess(reachedVertex);
					if (va.approachTime == 0)
						recursion = false;
					
					// Build of a candidate
					candidate = *itj;
				}
			

				// Recursion if needed
				if (recursion)
				{
					Journey tempJourney (currentJourney);
					Journey recursiveCandidate;
					if (accessDirection == TO_DESTINATION)
						tempJourney.append (*itj);
					else
						tempJourney.prepend(*itj);

					const Vertex* nextVertex = (accessDirection == TO_DESTINATION) 
						? tempJourney.getDestination ()->getFromVertex ()
						: tempJourney.getOrigin ()->getFromVertex ();

					VertexAccessMap nextVam;
					nextVertex->getPlace ()->getImmediateVertices (nextVam,
						accessDirection,
						_accessParameters
						, DO_NOT_SEARCH_ADDRESSES
						, SEARCH_PHYSICALSTOPS
						, nextVertex
					);

					findBestJourney (recursiveCandidate, nextVam, dvam, accessDirection, tempJourney, false, optim);

					if (!recursiveCandidate.empty())
					{
						if (accessDirection == TO_DESTINATION)
							recursiveCandidate.prepend (*itj);
						else
							recursiveCandidate.append(*itj);

						if (recursiveCandidate.isBestThan(candidate, accessDirection))
							candidate = recursiveCandidate;
					}
				}

				if (candidate.isBestThan(result, accessDirection))
					result = candidate;
			}
		}



		void RoutePlanner::computeRoutePlanningDepartureArrival(
			Journey& result
			, const VertexAccessMap& ovam
			, const VertexAccessMap& dvam
		){
			_bestArrivalVertexReachesMap.clear ();
			_maxArrivalTime = _journeySheetEndTime;
			_maxArrivalTime.addDaysDuration(7);	/// @todo Replace 7 by a parameter
		    
			for(map<const Vertex*, VertexAccess>::const_iterator itVertex(dvam.getMap().begin());
				itVertex != dvam.getMap ().end ();
				++itVertex
			){
				_bestArrivalVertexReachesMap.insert (itVertex->first, 
								 _maxArrivalTime - itVertex->second.approachTime);
			}
			for(map<const Vertex*, VertexAccess>::const_iterator itVertex(ovam.getMap().begin());
				itVertex != ovam.getMap().end();
				++itVertex
			){
				_bestArrivalVertexReachesMap.insert (itVertex->first, 
								 _minDepartureTime + itVertex->second.approachTime);
			}
		    

			Journey currentJourney;

			// Look for best arrival
			findBestJourney (result, ovam, dvam, TO_DESTINATION, currentJourney, false, false);
		    
			if (result.empty() || result.getDepartureTime() > _journeySheetEndTime)
			{
				result.clear();
				return;
			}
		    
			// If a journey was found, try to optimize by delaying departure hour as much as possible.
			_bestDepartureVertexReachesMap.clear ();

			const JourneyLegs& jl(result.getJourneyLegs());
			for (JourneyLegs::const_iterator it(jl.begin()); it != jl.end(); ++it)
			{
				_bestDepartureVertexReachesMap.insert (
					it->getDepartureEdge()->getFromVertex (),
					it->getDepartureDateTime ()
				);
			}
		    
			for(map<const Vertex*, VertexAccess>::const_iterator itVertex(dvam.getMap().begin());
				itVertex != dvam.getMap().end ();
				++itVertex
			){
				_bestDepartureVertexReachesMap.insert (
					itVertex->first, 
					result.getArrivalTime () + 
					dvam.getVertexAccess (result.getDestination ()->getFromVertex ()).approachTime - 
					itVertex->second.approachTime);
			}
			for(map<const Vertex*, VertexAccess>::const_iterator itVertex(ovam.getMap().begin());
				itVertex != ovam.getMap().end();
				++itVertex
			){
				_bestDepartureVertexReachesMap.insert (
					itVertex->first, 
					result.getDepartureTime () -
					ovam.getVertexAccess (result.getOrigin ()->getFromVertex ()).approachTime + 
					itVertex->second.approachTime);
			}
		    
			// Update bounds
			_minDepartureTime = result.getDepartureTime () -
			ovam.getVertexAccess (result.getOrigin ()->getFromVertex ()).approachTime;
			_maxArrivalTime = result.getArrivalTime () + 
			dvam.getVertexAccess (result.getDestination ()->getFromVertex ()).approachTime;

			
			// Look for best departure
			// A revoir il faut surement faire currentJourney = result
			findBestJourney (result, dvam, ovam, FROM_ORIGIN, currentJourney, true, true);

		}



		Journeys RoutePlanner::computeJourneySheetDepartureArrival()
		{
			Journey journey;

			// Create origin vam from integral search on roads
			IntegralSearcher iso(
				TO_DESTINATION
				, _accessParameters
				, DO_NOT_SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
				, USE_ROADS
				, DO_NOT_USE_LINES
				, _bestArrivalVertexReachesMap
				, _destinationVam
				, _calculationTime
				, _maxArrivalTime
				);
			Journeys originJourneys(iso.integralSearch (
				_originVam
				, _journeySheetStartTime
				, std::numeric_limits<int>::max ()
				));
			
			VertexAccessMap ovam;
			// Include physical stops from originVam into result of integral search
			// (cos not taken into account in returned journey vector).
			ovam.merge (_originVam, 
				VertexAccessMap::DO_NOT_MERGE_ADDRESSES,
				VertexAccessMap::MERGE_PHYSICALSTOPS);
			
			
			for (Journeys::const_iterator itoj = originJourneys.begin ();
			 itoj != originJourneys.end (); ++itoj)
			{
				const Journey& oj = (*itoj);
				VertexAccess va;
				va.approachTime = _originVam.getVertexAccess (
					oj.getOrigin ()->getFromVertex ()).approachTime + oj.getDuration ();

				va.approachDistance = _originVam.getVertexAccess (
					oj.getOrigin ()->getFromVertex ()).approachDistance + oj.getDistance ();
			
				ovam.insert (oj.getDestination ()->getFromVertex (), va);
			}


			// Create destination vam from integral search on roads
			IntegralSearcher isd(
				FROM_ORIGIN
				, _accessParameters
				, DO_NOT_SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
				, USE_ROADS
				, DO_NOT_USE_LINES
				, _bestDepartureVertexReachesMap
				, _originVam
				, _calculationTime
				, _minDepartureTime
				);
			Journeys destinationJourneys(isd.integralSearch (
					_destinationVam
					, _journeySheetEndTime
					, std::numeric_limits<int>::max ()
					));

			VertexAccessMap dvam;
			// Include physical stops from destinationVam into result of integral search
			// (cos not taken into account in returned journey vector).
			dvam.merge (_destinationVam, 
				VertexAccessMap::DO_NOT_MERGE_ADDRESSES,
				VertexAccessMap::MERGE_PHYSICALSTOPS);


			for (Journeys::const_iterator itdj = destinationJourneys.begin ();
			 itdj != destinationJourneys.end (); ++itdj)
			{
				const Journey& dj = (*itdj);
				VertexAccess va;
				va.approachTime = _destinationVam.getVertexAccess (
					dj.getDestination ()->getFromVertex ()).approachTime + dj.getDuration ();
				va.approachDistance = _destinationVam.getVertexAccess (
					dj.getDestination ()->getFromVertex ()).approachDistance + dj.getDistance ();
				
				dvam.insert (dj.getDestination ()->getFromVertex (), va);
			}


			_previousContinuousServiceDuration = 0;

			Journeys result;
		    
			for (_minDepartureTime = _journeySheetStartTime; 
			 _minDepartureTime < _journeySheetEndTime; )
			{
				journey.clear();
				computeRoutePlanningDepartureArrival (journey, ovam, dvam);
				
				//! <li> If no journey was found and last service is continuous, 
					//! then repeat computation after continuous service range. </li>
				if ( (journey.empty()) &&
					 (result.empty () == false) && 
					 (result.back ().getContinuousServiceRange () > 0) )
				{
					_minDepartureTime = _previousContinuousServiceLastDeparture;
					_minDepartureTime += 1;
					_previousContinuousServiceDuration = 0;
					computeRoutePlanningDepartureArrival (journey, ovam, dvam);	
				}
				
				if (journey.empty())
					break;				
				
				//! <li>If last continuous service was broken, update its range</li>
				if ( (result.empty () == false) &&
					 (result.back ().getContinuousServiceRange () > 0) &&
					 (journey.getDepartureTime () <= _previousContinuousServiceLastDeparture) )
				{
					int duration = journey.getArrivalTime () - result.back ().getArrivalTime () - 1;
					result.back ().setContinuousServiceRange (duration);
				}

				/*!	<li>En cas de nouveau service continu, enregistrement de valeur pour le calcul de la prochaine solution</li>	*/
				if (journey.getContinuousServiceRange() > 1)
				{
					_previousContinuousServiceDuration = journey.getArrivalTime() - journey.getDepartureTime();
					_previousContinuousServiceLastDeparture = journey.getDepartureTime();
					_previousContinuousServiceLastDeparture += journey.getContinuousServiceRange();
				}
				else
					_previousContinuousServiceDuration = 0;


				result.push_back (journey);
				
				_minDepartureTime = journey.getDepartureTime ();
				_minDepartureTime += 1;
			}
			
			return result;
			
		}
	}
}
