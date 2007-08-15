
/** PublicPlaceTableSync class header.
	@file PublicPlaceTableSync.h

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

#ifndef SYNTHESE_PublicPlaceTableSync_H__
#define SYNTHESE_PublicPlaceTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace env
	{
		class PublicPlace;

		/** PublicPlace table synchronizer.
			@ingroup m15LS refLS
		*/
		class PublicPlaceTableSync : public db::SQLiteTableSyncTemplate<PublicPlace>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_CITYID;
			
			PublicPlaceTableSync();


			/** PublicPlace search.
				(other search parameters)
				@param first First PublicPlace object to answer
				@param number Number of PublicPlace objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<PublicPlace> Founded PublicPlace objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<PublicPlace> > search(
				// other search parameters ,
				int first = 0, int number = 0);


		protected:

			/** Action to do on PublicPlace creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows
				, bool);

			/** Action to do on PublicPlace creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

			/** Action to do on PublicPlace deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

		};
	}
}

#endif // SYNTHESE_PublicPlaceTableSync_H__


static const std::string PUBLICPLACES_TABLE_NAME ("t013_public_places");

