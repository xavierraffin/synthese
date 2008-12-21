
/** ServiceDateTableSync class header.
	@file ServiceDateTableSync.h

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

#ifndef SYNTHESE_ServiceDateTableSync_H__
#define SYNTHESE_ServiceDateTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include "Date.h"

#include "SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace env
	{
		/** Service dates table synchronizer.
			@ingroup m35LS refLS
		*/
		class ServiceDateTableSync
		:	public db::SQLiteTableSyncTemplate<ServiceDateTableSync>
		{
		private:
			static void _updateServiceCalendar (const db::SQLiteResultSPtr& rows, bool marked) ;

		public:
			/** Writing of the dates of a service.
				@param service pointer to the service from which save the dates
				@author Hugues Romain
				@date 2008
				@todo implement it
			*/
			static void Save(env::NonPermanentService* service);

			static const std::string COL_SERVICEID;
			static const std::string COL_DATE;
			
			ServiceDateTableSync();

			static std::vector<time::Date> GetDatesOfService(uid serviceId);

			/** Action to do on Service Date creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows, bool isFirstSync = false);

			/** Action to do on Service Date creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

			/** Action to do on Service Date deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

		};
	}
}

#endif // SYNTHESE_ServiceDateTableSync_H__
