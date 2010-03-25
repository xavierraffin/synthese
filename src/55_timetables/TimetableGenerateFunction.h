
//////////////////////////////////////////////////////////////////////////////////////////
/// TimetableGenerateFunction class header.
///	@file TimetableGenerateFunction.h
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_TimetableGenerateFunction_H__
#define SYNTHESE_TimetableGenerateFunction_H__

#include "FactorableTemplate.h"
#include "FunctionWithSite.h"

namespace synthese
{
	namespace env
	{
		class Line;
	}

	namespace timetables
	{
		class Timetable;

		/** Generates timetables.
			@author Hugues Romain
			@date 2009
			@ingroup m55Functions refFunctions

			A timetable can be generated by 3 ways.
			
			<h3>Usage 1 : loading a pre-configured timetable</h3>

			Parameters :
			<ul>
				<li>roid : id of the pre-configured timetable</li>
			</ul>

			<h3>Usage 2 : building the full timetable of a line route</h3>

			Parameters :
			<ul>
				<li>roid : id of a Line object</li>
				<li>cid : id of the CalendarTemplate object</li>
				<li>sid : id of the site to use</li>
			</ul>

			<h3>Usage 3 : building a timetable from a list of stops</h3>

			Parameters :
			<ul>
				<li>city1 : first city</li>
				<li>stop1 : first stop</li>
				<li>city2 : second city</li>
				<li>stop2 : second stop</li>
				<li>...</li>
				<li>cityn : nth city</li>
				<li>stopn : nth stop</li>
				<li>cid : id of the CalendarTemplate object</li>
				<li>sid : id of the interface to use</li>
			</ul>
		*/
		class TimetableGenerateFunction:
			public util::FactorableTemplate<transportwebsite::FunctionWithSite<false>,TimetableGenerateFunction>
		{
		public:
			static const std::string PARAMETER_CALENDAR_ID;
			static const std::string PARAMETER_STOP_PREFIX;
			static const std::string PARAMETER_CITY_PREFIX;
			
		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const Timetable>	_timetable;
				boost::shared_ptr<const env::Line> _line;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues
			/// @date 2009
			server::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues
			/// @date 2009
			virtual void _setFromParametersMap(
				const server::ParametersMap& map
			);
			
			
		public:
			TimetableGenerateFunction();

			void setTimetable(boost::shared_ptr<const Timetable> value) { _timetable = value; }
			void setLine(boost::shared_ptr<const env::Line> value) { _line = value; }

			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @author Hugues
			/// @date 2009
			virtual void run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @return true if the function can be run
			/// @author Hugues
			/// @date 2009
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues
			/// @date 2009
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_TimetableGenerateFunction_H__
