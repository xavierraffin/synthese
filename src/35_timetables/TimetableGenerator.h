
/** TimetableGenerator class header.
	@file TimetableGenerator.h

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

#ifndef SYNTHESE_timetables_TimetableGenerator_h__
#define SYNTHESE_timetables_TimetableGenerator_h__

#include <vector>
#include <set>

#include "15_env/Calendar.h"

#include "35_timetables/TimetableColumn.h"
#include "35_timetables/TimetableWarning.h"
#include "35_timetables/TimetableRow.h"

namespace synthese
{
	namespace time
	{
		class Schedule;
	}

	namespace env
	{
		class Line;
		class CommercialLine;
	}

	namespace timetables
	{
		/** TimetableGenerator class.
			@ingroup m55
		*/
		class TimetableGenerator
		{
		public:
			typedef std::vector<TimetableWarning>			Warnings;
			typedef std::vector<TimetableColumn>			Columns;
			typedef std::vector<TimetableRow>				Rows;
			typedef std::set<const env::CommercialLine*>	Lines;

		private:

			//! @name Content definition
			//@{
				Rows			_rows;
				env::Calendar	_baseCalendar;
				Lines			_excludedLines;
				bool			_withContinuousServices;
			//@}

			//! @name Rendering parameters
			//@{
				int				_maxColumnsNumber;
			//@}

			//! @name Results
			//@{
				Columns			_columns;
				Warnings		_warnings;
			//@}
			

			//! @name Algorithms
			//@{
				bool	_isLineSelected(const env::Line& line)	const;
				void	_insert(const TimetableColumn& col);
				void	_buildWarnings();
			//@}

		public:
			TimetableGenerator();

			//! @name Getters
			//@{
				const Warnings&	getWarnings()	const;
				const Rows&		getRows()		const;
			//@}

			//! @name Actions
			//@{
				void build();
			//@}

			//! @name Output by row
			//@{
				std::vector<time::Schedule>		getSchedulesByRow(Rows::const_iterator row)	const;
				std::vector<const env::Line*>	getLines()									const;
				std::vector<tTypeOD>			getOriginTypes()							const;
				std::vector<tTypeOD>			getDestinationTypes()						const;
				std::vector<std::vector<TimetableWarning>::const_iterator>	getColumnsWarnings()	const;
			//@}

			//! @name Setters
			//@{
				void setRows(const Rows& rows);
				void setBaseCalendar(const env::Calendar& value);
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_TimetableGenerator_h__
