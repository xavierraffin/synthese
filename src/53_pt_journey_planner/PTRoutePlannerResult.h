
/** PTRoutePlannerResult class header.
	@file PTRoutePlannerResult.h

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

#ifndef SYNTHESE_routeplanner_PTRoutePlannerResult_h__
#define SYNTHESE_routeplanner_PTRoutePlannerResult_h__

#include <boost/optional.hpp>

#include "Journey.h"
#include "TimeSlotRoutePlanner.h"
#include "PlacesList.hpp"
#include "MessagesTypes.h"

namespace synthese
{
	namespace html
	{
		class HTMLForm;
	}

	namespace geography
	{
		class NamedPlace;
		class Place;
	}

	namespace pt_journey_planner
	{
		//////////////////////////////////////////////////////////////////////////
		/// Public transportation route planner result class.
		///	@ingroup m53
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// The route planner result contains 3 main informations :
		///	<ul>
		///		<li>Some informations about the computing process</li>
		///		<li>The list of journeys found by the route planner</li>
		///		<li>A list of places used as the first column of the schedules sheet</li>
		///	</ul>
		///
		///	<h2>List of places for the schedules sheet</h2>
		///
		/// The list of places respects the following rules :
		///	<ul>
		///		<li>Each place used by the journey is present in the list</li>
		///		<li>The order of places in the list is consistant for each journey</li>
		///		<li>If necessary a place can be present several times in the list.</li>
		///		<li>Place repetitions are avoided as much as possible</li>
		///		<li>Origin places are always on the top of the list</li>
		///		<li>Destination places are always on the bottom of the list</li>
		///	</ul>
		///
		/// The list of places is generated by the _buildPlacesList method.
		///
		/// Coding guidelines :
		///  - always run _createOrderedPlaces immediately after every update of _journeys (segfault issue)
		class PTRoutePlannerResult
		{
		public:
			typedef algorithm::TimeSlotRoutePlanner::Result Journeys;

			typedef algorithm::PlacesList<const geography::NamedPlace*, Journeys::const_iterator> PlacesListConfiguration;

		private:
			//! @name Informations about the computing process
			//@{
				const geography::Place* const		_departurePlace;
				const geography::Place* const		_arrivalPlace;
				bool _samePlaces;	//!< Whether the route planning was attempted between to identical places (in this case the result is always empty)
				bool _filtered;
			//@}

			//! @name Journeys found
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// List of the result journeys, ordered by departure time.
				/// Always run _createOrderedPlaces immediately after every update of _journeys (segfault issue)
				Journeys	_journeys;



				//////////////////////////////////////////////////////////////////////////
				/// Indexation of the places used by the journeys.
				/// Run it after each every update of _journeys (segfault issue)
				/// <pre>
				/// Stops D---a---a---S---S---a---S---S---S---a---A
				/// Ped.?   1   1   1   0   1   1   0   0   1   1
				///	pr.pe.? 0   1   1   1   0   1   1   0   0   1
				///	nx.pe.? 1   1   0   1   1   0   0   1   1   0
				///	first?  1   0   0   0   0   0   0   0   0   0
				///	last?   0   0   0   0   0   0   0   0   0   1
				/// avoidDoubles = false :
				/// Edges  X-o o-o o-X X-X X-o o-X X-X X-X X-o o-X
				/// DEP     1   0   0   1   1   0   1   1   1   0
				/// ARR     0   0   1   1   0   1   1   1   0   1
				/// avoidDoubles = true :
				/// Edges  X-o o-o o-X o-X o-o o-X o-X o-X o-o o-X
				/// DEP     1   0   0   0   0   0   0   0   0   0
				/// ARR     0   0   1   1   0   1   1   1   0   1
				/// Formula :
				/// Dep = first || !avoidDoubles && (!ped || pr.ped)
				/// Arr : !ped || last
				/// </pre>
				void _createOrderedPlaces();
			//@}

			//! @name Places list
			//@{
				PlacesListConfiguration _orderedPlaces;
			//@}

		public:
			void operator=(const PTRoutePlannerResult& other);

			PTRoutePlannerResult(
				const geography::Place* departurePlace,
				const geography::Place* arrivalPlace,
				bool samePlaces,
				const algorithm::TimeSlotRoutePlanner::Result& journeys
			);

			//! @name Getters
			//@{
				bool getSamePlaces() const { return _samePlaces; }
				const Journeys& getJourneys() const { return _journeys; }
				Journeys& getJourneys() { return _journeys; }
				const PlacesListConfiguration& getOrderedPlaces() const { return _orderedPlaces; }
				const geography::Place* getDeparturePlace() const { return _departurePlace; }
				const geography::Place* getArrivalPlace() const { return _arrivalPlace; }
				bool getFiltered() const { return _filtered; }
			//@}

			//! @name Modifiers
			//@{
				void removeFirstJourneys(size_t value);


				//////////////////////////////////////////////////////////////////////////
				/// Removes the too long journeys compared to the shortest one.
				/// The journeys which are too long are removed from the result.
				/// If at least one journey was removed, then the _filtered attribute is
				/// set to true.
				/// @param maximal duration ratio between longest and shortest journeys
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				void filterOnDurationRatio(double ratio);



				void filterOnWaitingTime(
					boost::posix_time::time_duration minWaitingTime
				);
			//@}

			//! @name Services
			//@{
				struct MaxAlarmLevels
				{
					messages::AlarmLevel lineLevel;
					messages::AlarmLevel stopLevel;

					MaxAlarmLevels();
				};

				//////////////////////////////////////////////////////////////////////////
				/// Gets the biggest alarm level of all lines and stop used by the journeys
				/// @return the biggest alarm level of all lines and stop used by the journeys
				MaxAlarmLevels getMaxAlarmLevels() const;
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Generates an HTML table to display on admin console.
			/// @param stream stream to write the result on
			/// @param resaForm reservation HTML Form. If not defined, the table does not
			///		allow to book any seat.
			/// @param resaRadioFieldName name of radio fields to display if a journey is
			///		bookable.
			void displayHTMLTable(
				std::ostream& stream,
				boost::optional<html::HTMLForm&> resaForm,
				const std::string& resaRadioFieldName,
				bool ignoreReservationDeadline
			) const;

			//////////////////////////////////////////////////////////////////////////
			/// Reads the named place from one of the
			static const geography::NamedPlace* GetNamedPlaceForDeparture(
				bool isPedestrian,
				const graph::ServicePointer* arrivalLeg,
				const graph::ServicePointer& departureLeg,
				const geography::NamedPlace& defaultValue
			);

			//////////////////////////////////////////////////////////////////////////
			/// Reads the named place from one of the
			static const geography::NamedPlace* GetNamedPlaceForArrival(
				bool isPedestrian,
				const graph::ServicePointer& arrivalLeg,
				const graph::ServicePointer* departureLeg,
				const geography::NamedPlace& defaultValue
			);

			static const geography::NamedPlace* getNamedPlace(
				const geography::Place* place
			);

			static bool HaveToDisplayDepartureStopOnGrid(
				graph::Journey::ServiceUses::const_iterator itl,
				const graph::Journey::ServiceUses& jl,
				bool avoidDoubles
			);

			static bool HaveToDisplayArrivalStopOnGrid(
				graph::Journey::ServiceUses::const_iterator itl,
				const graph::Journey::ServiceUses& jl
			);
		};
	}
}

#endif // SYNTHESE_routeplanner_PTRoutePlannerResult_h__
