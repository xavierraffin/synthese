
/** SQLiteTableSync class header.
	@file SQLiteTableSync.h

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

#ifndef SYNTHESE_DB_SQLITETABLESYNC_H
#define SYNTHESE_DB_SQLITETABLESYNC_H

#include "SQLiteTableFormat.h"
#include "SQLiteResult.h"
#include "SQLiteStatement.h"

#include "FactoryBase.h"
#include "UId.h"

#include <string>
#include <vector>
#include <iostream>

#define UPDATEABLE true;
#define NON_UPDATEABLE false;

namespace synthese
{
	namespace db
	{

		class SQLiteSync;
		class SQLiteResult;
		class SQLite;

		/** @defgroup refLS Table synchronizers.
			@ingroup ref
		*/

		/// Base class for an SQLite table synchronizer.
		///	By convention, the table name must always start with the t letter
		///	followed by a unique 3 digits integer (SQLite does not allow ids starting with number).
		///
		///	@ingroup m10
		///
		class SQLiteTableSync : public util::FactoryBase<SQLiteTableSync>
		{
		private:

		    const bool _allowInsert;
		    const bool _allowRemove;
		    const std::string _triggerOverrideClause;
		    bool _ignoreCallbacksOnFirstSync;
		    bool _enableTriggers;
		    
		    const std::string _tableName;
		    SQLiteTableFormat _tableFormat;

		    std::vector<std::string> _selectOnCallbackColumns;

		public:

			virtual void initAutoIncrement();

			/// Optional arguments for factory.
			struct Args
			{
			    bool allowInsert;
			    bool allowRemove;
			    std::string triggerOverrideClause;
			    bool ignoreCallbacksOnFirstSync;
			    bool enableTriggers;

			    Args (bool allowInsertArg = true, 
				  bool allowRemoveArg = true,
				  const std::string& triggerOverrideClauseArg = "1",
				  bool ignoreCallbacksOnFirstSyncArg = false,
				  bool enableTriggersArg = true
				):	allowInsert (allowInsertArg)
				, allowRemove (allowRemoveArg)
				, triggerOverrideClause (triggerOverrideClauseArg)
				, ignoreCallbacksOnFirstSync (ignoreCallbacksOnFirstSyncArg)
				, enableTriggers (enableTriggersArg)
			    {}

			};


			SQLiteTableSync (const Args& args = Args ());

			~SQLiteTableSync ();

			virtual const std::string& getTableName () const = 0;

			/** Returns the unique integer identifying a table.
				@return The unique integer identifying a table
			*/
			int getTableId () const;

			const SQLiteTableFormat& getTableFormat () const;

			bool getIgnoreCallbacksOnFirstSync () const;
			void setIgnoreCallbacksOnFirstSync (bool ignoreCallbacksOnFirstSync);

			void setEnableTriggers (bool enableTriggers);

			void updateSchema (synthese::db::SQLite* sqlite);

			/** First synchronisation.
				This method is called when the synchronizer is created
				to synchronize it with pre-existing data in db.

				It creates ou updates all the needed tables and indexes.

				Note : It does not delete any useless table present in the database.
			*/
			void firstSync (synthese::db::SQLite* sqlite, 
					synthese::db::SQLiteSync* sync);
			
			/** This method can be overriden to invoke some code before
			    executing firstSync body.
			    Default implementation is doing nothing.
			*/
			virtual void beforeFirstSync (SQLite* sqlite, 
						      SQLiteSync* sync);

			/** This method can be overriden to invoke some code after
			    having executed firstSync body.
			    Default implementation is doing nothing.
			*/
			virtual void afterFirstSync (SQLite* sqlite, 
						     SQLiteSync* sync);

			virtual void rowsAdded (SQLite* sqlite, 
						SQLiteSync* sync,
						const SQLiteResultSPtr& rows, bool isFirstSync = false) = 0;

			virtual void rowsUpdated (SQLite* sqlite, 
						SQLiteSync* sync,
						const SQLiteResultSPtr& rows) = 0;

			virtual void rowsRemoved (SQLite* sqlite, 
						SQLiteSync* sync,
						const SQLiteResultSPtr& rows) = 0;

			/** Utility method to get a row by id.
			    This uses a precompiled statement for performance 
			*/
			SQLiteResultSPtr getRowById (synthese::db::SQLite* sqlite, const uid& id) const;

		protected:

			/** By default, return first column name.
			 */
			virtual const std::string& getPrimaryKey () const;


			void addTableColumn (const std::string& columnName, 
					     const std::string& columnType, 
					     bool updatable = true,
					     bool selectOnCallback = true
			    );

			/** Adds a multi-column index in the table description.
				@param columns Vector of column names
				@param name Name of the index. Optional : if not specified or empty, the name is the concatenation of the columns names
			*/
			void addTableIndex(const std::vector<std::string>& columns, std::string name = "");

			/** Adds a single column index in the table description.
				@param column column name
				@param name Name of the index. Optional : if not specified or empty, the name is identical to the column name.
			*/
			void addTableIndex(const std::string& column, std::string name = "");

		protected:

			static int ParseTableId (const std::string& tableName);


		private:

			std::string getTriggerOverrideClause () const;


			/** Creates table in SQLite db according to this class
			 * table format.
			 *
			 * @param tableSchema Required table schema.
			 */
			void createTable (synthese::db::SQLite* sqlite,
					  const std::string& tableSchema,
					  const std::string& triggerNoInsert,
					  const std::string& triggerNoRemove,
					  const std::string& triggerNoUpdate);

			/** Adapts table in SQLite db to conform to this class 
			 * table format.
			 * Right now, only new column addition/insertion is supported.
			 * Any other change to table schema is not supported yet.
			 *
			 * @param tableSchema Required table schema.
			 * @param dbSchema Actual table schema in db.
			 */
			void adaptTable (synthese::db::SQLite* sqlite,
					 const std::string& tableSchema,
					 const std::string& triggerNoInsert,
					 const std::string& triggerNoRemove,
					 const std::string& triggerNoUpdate);
			

			std::string getSelectOnCallbackColumnsClause () const;


		public:

			/** Creates the SQL statement to create a table in db
			 * given a certain format.
			 */
			static std::string CreateSQLSchema (const std::string& tableName,
							    const SQLiteTableFormat& format);

			/** Creates the SQL statement to crate an index in the database given a certain format.
				@param tableName The table name
				@param format The format of the table containing the indexes description
				@return std::string The SQL statement
				@author Hugues Romain
				@date 2007

				Note : the real index name in the database is the concatenation of the table name and the name specified in the table format separated by a _ character.
			*/
			static std::string CreateIndexSQLSchema(const std::string& tableName, const SQLiteTableIndexFormat& format);
			
			static std::string CreateTriggerNoInsert (
			    const std::string& tableName,
			    const std::string& triggerOverrideClause);
			
			static std::string CreateTriggerNoRemove (
			    const std::string& tableName,
			    const std::string& triggerOverrideClause);
			
			static std::string CreateTriggerNoUpdate (
			    const std::string& tableName,
			    const SQLiteTableFormat& format,
			    const std::string& triggerOverrideClause);
			



			static std::vector<std::string> 
			    GetTableColumnsDb (synthese::db::SQLite* sqlite,
					       const std::string& tableName);

			static std::string GetSQLSchemaDb (synthese::db::SQLite* sqlite,
							   const std::string& tableName);
			
			static std::string GetTriggerNoInsertDb (
			    synthese::db::SQLite* sqlite,
			    const std::string& tableName);

			static std::string GetTriggerNoRemoveDb (
			    synthese::db::SQLite* sqlite,
			    const std::string& tableName);

			static std::string GetTriggerNoUpdateDb (
			    synthese::db::SQLite* sqlite,
			    const std::string& tableName);

			/** Builds the name of the index in the sqlite master table.
				@param tableName Name of the table which belongs the index
				@param name Name of the index within the table
				@return the full name
				@author Hugues Romain
				@date 2007
				
			*/
			static std::string getIndexDBName(const std::string& tableName, const std::string& name);
		};
	}
}

#endif
