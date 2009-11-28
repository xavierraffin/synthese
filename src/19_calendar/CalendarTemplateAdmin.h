
/** CalendarTemplateAdmin class header.
	@file CalendarTemplateAdmin.h
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

#ifndef SYNTHESE_CalendarTemplateAdmin_H__
#define SYNTHESE_CalendarTemplateAdmin_H__

#include "AdminInterfaceElementTemplate.h"

#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;

		/** CalendarTemplateAdmin Class.
			@ingroup m19Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class CalendarTemplateAdmin:
			public admin::AdminInterfaceElementTemplate<CalendarTemplateAdmin>
		{
		public:
			static const std::string TAB_SOURCE;
			static const std::string TAB_RESULT;
			static const std::string PARAMETER_RESULT_START;
			static const std::string PARAMETER_RESULT_END;

		private:
			boost::shared_ptr<const CalendarTemplate>	_calendar;
			boost::gregorian::date	_resultStartDate;
			boost::gregorian::date	_resultEndDate;

		public:
			CalendarTemplateAdmin();
			
			void setCalendar(boost::shared_ptr<CalendarTemplate> value);
			boost::shared_ptr<const CalendarTemplate> getCalendar() const;
			
			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			void setFromParametersMap(
				const server::ParametersMap& map,
				bool objectWillBeCreatedLater
			);
			
			
			
			/** Parameters map generator, used when building an url to the admin page.
					@return server::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007					
				*/
			virtual server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param request The current request (can be used to determinate the
			///        current user rights.)
			/// @author Hugues
			/// @date 2009
			virtual void _buildTabs(
				const server::FunctionRequest<admin::AdminRequest>& request
			) const;



			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@author Hugues Romain
				@date 2008
			*/
			void display(
				std::ostream& stream,
				interfaces::VariablesMap& variables,
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const;
			
			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const;
			
			
			
			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getTitle() const;
			
			
			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;
			
		};
	}
}

#endif // SYNTHESE_CalendarTemplateAdmin_H__
