
/** IntegralSearcher class header.
	@file IntegralSearcher.h

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

#ifndef SYNTHESE_routeplanner_IntegralSearcher_h__
#define SYNTHESE_routeplanner_IntegralSearcher_h__

#include "Log.h"

#include "GraphTypes.h"
#include "AccessParameters.h"

#include "JourneysResult.h"
#include "GraphModuleTemplate.h"

namespace synthese
{
	namespace graph
	{
		class Vertex;
		class VertexAccessMap;
		class Edge;
		class Journey;
		class JourneyComparator;
	}

	namespace time
	{
		class DateTime;
	}

	namespace routeplanner
	{
		class BestVertexReachesMap;

		/** IntegralSearcher class.
			@ingroup m53

			This class provides an algorithm which returns all the vertices available in an area.

			The area is defined by several constraints.

			This algorithm can be used in a route planning : it handles optionally the concept of searched place (destination).
		*/
		class IntegralSearcher
		{
		private:
			typedef graph::Edge* (graph::Edge::*PtrEdgeStep) () const;

			struct _JourneyUsefulness
			{
				bool canBeAResultPart;
				bool continueToTraverseThePath;
				_JourneyUsefulness(bool _canBeAResultPart, bool _continueToTraverseThePath);
			};

			class _JourneyComparator
			{
			public:
				bool operator()(boost::shared_ptr<graph::Journey> j1, boost::shared_ptr<graph::Journey> j2) const;
			};

			//! @name Parameters
			//@{
				const graph::AccessParameters				_accessParameters;
				const graph::AccessDirection				_accessDirection;
				const graph::GraphIdType					_whatToSearch;
				const graph::GraphIdType					_graphToUse;
				JourneysResult<graph::JourneyComparator>&	_result;
				BestVertexReachesMap&						_bestVertexReachesMap;
				const graph::VertexAccessMap&				_destinationVam;	//!< Can be a departure or an arrival, according to _accesDirection
				const time::DateTime&						_originDateTime;
				time::DateTime&								_minMaxDateTimeAtDestination;
				const boost::posix_time::time_duration		_previousContinuousServiceDuration;
				const time::DateTime&						_previousContinuousServiceLastDeparture;
				const bool									_optim;
				const bool									_inverted;	//!< Indicates that the AccessDirection is the contraty to the planning order (2nd passe)
				std::ostream* const							_logStream;
				const util::Log::Level						_logLevel;
			//@}


			void _integralSearch(
				const graph::VertexAccessMap& vertices,
				const graph::Journey& startJourney,
				const time::DateTime& originDateTime,
				int maxDepth,
				bool strictTime
			);

		public:
			IntegralSearcher(
				graph::AccessDirection			accessDirection
				, const graph::AccessParameters&	accessParameters
				, graph::GraphIdType			whatToSearch
				, graph::GraphIdType			graphToUse
				, JourneysResult<graph::JourneyComparator>&	result
				, BestVertexReachesMap&			bestVertexReachesMap,
				const graph::VertexAccessMap&	destinationVam,
				const time::DateTime&			originDateTime,
				time::DateTime&				minMaxDateTimeAtDestination
				, boost::posix_time::time_duration previousContinuousServiceDuration
				, const time::DateTime&			previousContinuousServiceLastDeparture
				, bool							optim
				, bool							inverted
				, std::ostream* const			logStream = NULL
				, util::Log::Level				logLevel = util::Log::LEVEL_NONE
			);



			//////////////////////////////////////////////////////////////////////////
			/// Integral search from the start vertex access map.
			/// @param startVam the search is launched at this vertices
			///	@param maxDepth Maximum recursion depth.
			///	@param strictTime Must the departure time be strictly equal to desired time ?
			void integralSearch(
				const graph::VertexAccessMap& vertices,
				int maxDepth,
				bool strictTime
			);



			//////////////////////////////////////////////////////////////////////////
			/// Integral search after a journey (transfer).
			/// @param startJourney connecting journey
			/// @param maxDepth limit of transfers
			void integralSearch(
				const graph::Journey& startJourney,
				int maxDepth
			);



			/** Journey utility evaluation.
				@param journey Journey to evaluate
				@param bool optimization mode
				@return pair<bool,bool> Possible values : false/false, true/true, false/true
					- first : utility to store the journey as result or a future result part.
					- second : utility to continue to traverse the rest of the path
				@author Hugues Romain
				@date 2007				
			*/
			_JourneyUsefulness evaluateJourney(
				const graph::Journey& journey
				, bool optim
			) const;

		};
	}
}

#endif // SYNTHESE_routeplanner_IntegralSearcher_h__
