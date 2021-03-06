
/** DesignatedLinePhysicalStop class implementation.
	@file DesignatedLinePhysicalStop.cpp

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

#include "DesignatedLinePhysicalStop.hpp"

#include "Hub.h"
#include "JourneyPatternCopy.hpp"
#include "LineStopTableSync.h"
#include "PTModule.h"
#include "StopPoint.hpp"

#include <geos/geom/LineString.h>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	
	namespace pt
	{
		DesignatedLinePhysicalStop::DesignatedLinePhysicalStop(
			util::RegistryKeyType id,
			JourneyPattern* line,
			std::size_t rankInPath,
			bool isDeparture,
			bool isArrival,
			double metricOffset,
			StopPoint* stop,
			bool scheduleInput,
			bool reservationNeeded
		):	Registrable(id),
			LinePhysicalStop(id, line, rankInPath, isDeparture, isArrival, metricOffset, stop),
			_scheduleInput(scheduleInput),
			_reservationNeeded(reservationNeeded)
		{}

		DesignatedLinePhysicalStop::~DesignatedLinePhysicalStop()
		{
			if(!getLine())
				return;
			// Collecting all line stops to unlink including journey pattern copies
			typedef vector<pair<JourneyPattern*, LinePhysicalStop*> > ToClean;
			ToClean toClean;
			toClean.push_back(make_pair(getLine(), this));
			BOOST_FOREACH(JourneyPatternCopy* copy, getLine()->getSubLines())
			{
				if (copy->getLineStop(getRankInPath()))
				{
					toClean.push_back(
						make_pair(
							copy,
							const_cast<LinePhysicalStop*>(static_cast<const LinePhysicalStop*>(
								copy->getEdge(getRankInPath()))
							)
					)	);
				}
			}
			
			BOOST_FOREACH(const ToClean::value_type& it, toClean)
			{
				// Removing edge from journey pattern
				it.first->removeEdge(*it.second);
				
				// Removing edge from stop point
				if (getPhysicalStop())
				{
					if(it.second->getIsArrival())
					{
						getPhysicalStop()->removeArrivalEdge(it.second);
					}
					if(it.second->getIsDeparture())
					{
						getPhysicalStop()->removeDepartureEdge(it.second);
					}
				}
				
				if(it.second != this)
				{
					delete it.second;
				}
			}
		}



		void DesignatedLinePhysicalStop::toParametersMap( util::ParametersMap& pm, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			if(!getPhysicalStop()) throw Exception("Linestop save error. Missing physical stop");
			if(!getLine()) throw Exception("Linestop Save error. Missing line");

			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + LineStopTableSync::COL_PHYSICALSTOPID,
				getPhysicalStop()->getKey()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_LINEID,
				getLine()->getKey()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_RANKINPATH,
				getRankInPath()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_ISDEPARTURE,
				isDepartureAllowed()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_ISARRIVAL,
				isArrivalAllowed()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_METRICOFFSET,
				getMetricOffset()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_SCHEDULEINPUT,
				getScheduleInput()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_INTERNAL_SERVICE,
				false
			);
			pm.insert(
				prefix + LineStopTableSync::COL_RESERVATION_NEEDED,
				getReservationNeeded()
			);

			if(hasGeometry())
			{
				boost::shared_ptr<geos::geom::Geometry> projected(getGeometry());
				if(	CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() !=
					static_cast<CoordinatesSystem::SRID>(getGeometry()->getSRID())
				){
					projected = CoordinatesSystem::GetStorageCoordinatesSystem().convertGeometry(*getGeometry());
				}

				geos::io::WKTWriter writer;
				pm.insert(
					prefix + TABLE_COL_GEOMETRY,
					writer.write(projected.get())
				);
			}
			else
			{
				pm.insert(prefix + TABLE_COL_GEOMETRY, string());
			}
		}



		void DesignatedLinePhysicalStop::_unlink()
		{
			StopPoint* stop(NULL);
            if (getPhysicalStop())
                stop = getPhysicalStop();

			// Collecting all line stops to unlink including journey pattern copies
			typedef vector<pair<JourneyPattern*, LinePhysicalStop*> > ToClean;
			ToClean toClean;
			BOOST_FOREACH(JourneyPatternCopy* copy, getLine()->getSubLines())
			{
				toClean.push_back(
					make_pair(
						copy,
						const_cast<LinePhysicalStop*>(static_cast<const LinePhysicalStop*>(
							copy->getEdge(getRankInPath()))
						)
				)	);
			}

			BOOST_FOREACH(const ToClean::value_type& it, toClean)
			{
				// Removing edge from journey pattern
				it.first->removeEdge(*it.second);

				// Removing edge from stop point
				if (stop)
				{
					if(it.second->getIsArrival())
					{
						stop->removeArrivalEdge(it.second);
					}
					if(it.second->getIsDeparture())
					{
						stop->removeDepartureEdge(it.second);
					}
				}

				it.second->setFromVertex(NULL);
			}

			
			// Useful transfer calculation
			if(stop)
			{
				stop->getHub()->clearAndPropagateUsefulTransfer(PTModule::GRAPH_ID);
			}

			clearPhysicalStopLinks();
		}
}	}
