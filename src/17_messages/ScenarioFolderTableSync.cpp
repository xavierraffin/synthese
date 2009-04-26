
/** ScenarioFolderTableSync class implementation.
	@file ScenarioFolderTableSync.cpp
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

#include <sstream>

#include "ScenarioFolderTableSync.h"
#include "ScenarioFolder.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,ScenarioFolderTableSync>::FACTORY_KEY("17 ScenarioFolder");
	}

	namespace messages
	{
		const string ScenarioFolderTableSync::COL_NAME("name");
		const string ScenarioFolderTableSync::COL_PARENT_ID("parent_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<ScenarioFolderTableSync>::TABLE(
				"t051_scenario_folder"
				);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<ScenarioFolderTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ScenarioFolderTableSync::COL_NAME, SQL_INTEGER),
			SQLiteTableSync::Field(ScenarioFolderTableSync::COL_PARENT_ID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};
		
		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<ScenarioFolderTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(ScenarioFolderTableSync::COL_PARENT_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<ScenarioFolderTableSync,ScenarioFolder>::Load(
			ScenarioFolder* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(ScenarioFolderTableSync::COL_NAME));

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType id(rows->getLongLong(ScenarioFolderTableSync::COL_PARENT_ID));

				if(id > 0)
				{
					object->setParent(
						ScenarioFolderTableSync::GetEditable(
							id,
							env,
							linkLevel
						).get()
					);
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<ScenarioFolderTableSync,ScenarioFolder>::Save(
			ScenarioFolder* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getName())
				<< "," << (object->getParent() ? Conversion::ToString(object->getParent()->getKey()) : "0")
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<ScenarioFolderTableSync,ScenarioFolder>::Unlink(
			ScenarioFolder* obj
		){
		}
	}
	
	
	
	namespace messages
	{
		ScenarioFolderTableSync::ScenarioFolderTableSync()
			: SQLiteNoSyncTableSyncTemplate<ScenarioFolderTableSync,ScenarioFolder>()
		{
		}



		void ScenarioFolderTableSync::Search(
			Env& env,
			optional<RegistryKeyType> parentFolderId
			, optional<string> name
			, int first /*= 0*/
			, int number, /*= 0*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 "
			;
			if(parentFolderId)
			{
				query << " AND " << COL_PARENT_ID << "=" << Conversion::ToString(*parentFolderId);
			}
			if(name)
			{
				query << " AND " << COL_NAME << " LIKE " << Conversion::ToSQLiteString(*name);
			}
			query << " ORDER BY " << COL_NAME << " ASC";
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
