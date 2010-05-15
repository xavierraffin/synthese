
/** TimetableNoteInterfacePage class header.
	@file TimetableNoteInterfacePage.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_TimetableNoteInterfacePage_H__
#define SYNTHESE_TimetableNoteInterfacePage_H__

#include <ostream>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace transportwebsite
	{
		class WebPage;
	}

	namespace server
	{
		class Request;
	}
	
	namespace timetables
	{
		class TimetableWarning;
	}

	namespace timetables
	{
		//////////////////////////////////////////////////////////////////////////
		/// 55.11 Interface : Timetable note display.
		///	@ingroup m55Pages refPages
		///	@author Hugues Romain
		///	@date 2009-2010
		//////////////////////////////////////////////////////////////////////////
		/// @copydoc TimetableNoteInterfacePage::Display
		class TimetableNoteInterfacePage
		{
		public:
			static const std::string DATA_NUMBER;
			static const std::string DATA_TEXT;
			static const std::string DATA_CALENDAR;
			static const std::string DATA_FIRST_YEAR;
			static const std::string DATA_FIRST_MONTH;
			static const std::string DATA_FIRST_DAY;
			static const std::string DATA_LAST_YEAR;
			static const std::string DATA_LAST_MONTH;
			static const std::string DATA_LAST_DAY;



			//////////////////////////////////////////////////////////////////////////
			/// Timetable note display.
			///	@param stream Stream to write on
			///	@param page display template to use for the display of the note
			///	@param calendarDatePage page to use to generate the display of each date of the calendar. If not defined, the calendar will be empty.
			///	@param request Source request
			///	@param object the note
			/// @author Hugues Romain
			//////////////////////////////////////////////////////////////////////////
			///	<h3>Parameters sent to the display template</h3>
			///	<ul>
			///		<li>number : Note number</li>
			///		<li>text : Text</li>
			///		<li>calendar : Calendar representing the active days of the calendar, generated by a call to calendar::CalendarDateInterfacePage for each day between the first day of the first month containing an active date and the last day of the last month containing an active date.</li>
			///		<li>first-year : Year of the first day when the service runs</li>
			///		<li>first-month : Month of the first day when the service runs</li>
			///		<li>first-day : Number of the first day when the service runs</li>
			///		<li>last-year : Year of the last day when the service runs</li>
			///		<li>last-month : Month of the last day when the service runs</li>
			///		<li>last-day : Number of the last day when the service runs</li>
			///	</li>
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const transportwebsite::WebPage> page,
				boost::shared_ptr<const transportwebsite::WebPage> calendarDatePage,
				const server::Request& request,
				const timetables::TimetableWarning& object
			);
		};
	}
}

#endif // SYNTHESE_TimetableNoteInterfacePage_H__
