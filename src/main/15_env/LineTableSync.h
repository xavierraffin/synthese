
/** LineTableSync class header.
	@file LineTableSync.h

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

#ifndef SYNTHESE_LineTableSync_H__
#define SYNTHESE_LineTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace env
	{
		class Line;

		/** Line table synchronizer.
			@ingroup m15

		Lines table :
			- on insert : 
			- on update : 
			- on delete : X
		*/
		class LineTableSync : public db::SQLiteTableSyncTemplate<Line>
		{
		public:
			static const std::string COL_AXISID;
			static const std::string COL_COMMERCIAL_LINE_ID;
			static const std::string COL_NAME;
			static const std::string COL_TIMETABLENAME;
			static const std::string COL_DIRECTION;
			static const std::string COL_ISWALKINGLINE;
			static const std::string COL_USEINDEPARTUREBOARDS;
			static const std::string COL_USEINTIMETABLES;
			static const std::string COL_USEINROUTEPLANNING;
			static const std::string COL_ROLLINGSTOCKID;
			static const std::string COL_FAREID;
			static const std::string COL_ALARMID;
			static const std::string COL_BIKECOMPLIANCEID;
			static const std::string COL_HANDICAPPEDCOMPLIANCEID;
			static const std::string COL_PEDESTRIANCOMPLIANCEID;
			static const std::string COL_RESERVATIONRULEID;

			LineTableSync();


			/** Line search.
				(other search parameters)
				@param first First Line object to answer
				@param number Number of Line objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Line*> Founded Line objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<Line> > search(
				// other search parameters ,
				int first = 0, int number = 0);


		protected:

			/** Action to do on Line creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows, bool isFirstSync = false);

			/** Action to do on Line creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on Line deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};
	}
}

#endif // SYNTHESE_LineTableSync_H__
