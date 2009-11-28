
/** CalendarTemplateElementTableSync class header.
	@file CalendarTemplateElementTableSync.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_CalendarTemplateElementTableSync_H__
#define SYNTHESE_CalendarTemplateElementTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "SQLiteNoSyncTableSyncTemplate.h"

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplateElement;

		/** CalendarTemplateElement table synchronizer.
			@ingroup m19LS refLS
		*/
		class CalendarTemplateElementTableSync:
			public db::SQLiteNoSyncTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>
		{
		public:
			static const std::string COL_CALENDAR_ID;
			static const std::string COL_RANK;
			static const std::string COL_MIN_DATE;
			static const std::string COL_MAX_DATE;
			static const std::string COL_INTERVAL;
			static const std::string COL_POSITIVE;
			static const std::string COL_INCLUDE_ID;
			
			/** CalendarTemplateElement search.
				@param env Environment to populate
				@param calendarId ID of the calendar which the searched elements must belong
				@param first First CalendarTemplateElement object to answer
				@param number Number of CalendarTemplateElement objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@param linkLevel level of link of the objects in the environment
				@return found objects
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				uid calendarId
				, int first = 0
				, boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);


			static void Shift(
				uid calendarId
				, int rank
				, int delta
			);

			static int GetMaxRank(uid calendarId);
		};
	}
}

#endif // SYNTHESE_CalendarTemplateElementTableSync_H__
