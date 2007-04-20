
/** SQLiteSync class implementation.
	@file SQLiteSync.cpp

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

#include <sqlite/sqlite3.h>
#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteException.h"
#include "02_db/SQLiteSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace db
	{


		SQLiteSync::SQLiteSync (const std::string& idColumnName)
		: _idColumnName (idColumnName)
		, _isRegistered (false)
		{

		}



		SQLiteSync::~SQLiteSync ()
		{
		}

		 

		void 
		SQLiteSync::addTableSynchronizer (const string& rank, shared_ptr<SQLiteTableSync> synchronizer)
		{
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);

			// assert (synchronizer->getTableFormat ().empty () == false);
			_tableSynchronizers.insert (std::make_pair (synchronizer->getTableName (), synchronizer));
			_rankedTableSynchronizers.insert(make_pair(rank, synchronizer));
		}



		bool 
		SQLiteSync::hasTableSynchronizer (const std::string& tableName) const
		{
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);
			return _tableSynchronizers.find (tableName) != _tableSynchronizers.end ();
		}


		   
		shared_ptr<SQLiteTableSync>
		SQLiteSync::getTableSynchronizer (const std::string& tableName) const
		{
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);
			if (hasTableSynchronizer (tableName) == false)
			{
			throw SQLiteException ("No synchronizer for table '" + tableName + "'");
			}
			return _tableSynchronizers.find (tableName)->second;
		}

	    
	    
	    std::map<std::string, shared_ptr<SQLiteTableSync> >
	    SQLiteSync::getTableSynchronizers () const
	    {
		boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);
		return _tableSynchronizers;
	    }
	    


		void 
		SQLiteSync::registerCallback (const SQLiteQueueThreadExec* emitter)
		{
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);

			_isRegistered = true;
			
			// Call the init sequence on all synchronizers.
			for (std::map<std::string, shared_ptr<SQLiteTableSync> >::const_iterator it = 
				 _rankedTableSynchronizers.begin (); 
			     it != _rankedTableSynchronizers.end (); ++it)
			{
			    Log::GetInstance().info("Loading table " + it->first);
			    try 
			    {
				it->second->firstSync (emitter, this);
			    }
			    catch (std::exception e)
			    {
				Log::GetInstance().error ("Error during first sync of " + it->first + 
							  ". In-memory data might be inconsistent.", e);
			    }
			}
			
		}
		 


		   
		void 
		SQLiteSync::eventCallback (const SQLiteQueueThreadExec* emitter,
					const SQLiteEvent& event)
		{
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);

			for (std::map<std::string, shared_ptr<SQLiteTableSync> >::const_iterator it 
				 = _tableSynchronizers.begin ();
			     it != _tableSynchronizers.end (); ++it)
			{
			    shared_ptr<SQLiteTableSync> tableSync = it->second;
			    if (tableSync->getTableName () != event.tbName) continue;
			    
			    if (event.opType == SQLITE_INSERT) 
			    {
				// Query for the modified row
				SQLiteResult result = emitter->execQuery ("SELECT * FROM " + event.tbName + " WHERE " 
									  + _idColumnName + "=" + Conversion::ToString (event.rowId));
				
				tableSync->rowsAdded (emitter, this, result);
			    }
			    else if (event.opType == SQLITE_UPDATE) 
			    {
				// Query for the modified row
				SQLiteResult result = emitter->execQuery ("SELECT * FROM " + event.tbName + " WHERE " 
									  + _idColumnName + "=" + Conversion::ToString (event.rowId));
				
				tableSync->rowsUpdated (emitter, this, result);
			    }
			    else if (event.opType == SQLITE_DELETE) 
			    {
				// Query for the modified row
				SQLiteResult result;
				std::vector<std::string> columns;
				std::vector<std::string> values;
				columns.push_back (_idColumnName);
				values.push_back (Conversion::ToString (event.rowId));
				result.addRow (values, columns);
				
				tableSync->rowsRemoved (emitter, this, result);
			    }
			}
			
		}
	    
	}
}

