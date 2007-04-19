﻿
/** AlarmTableSync class header.
	@file AlarmTableSync.h

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

#ifndef SYNTHESE_ENVLSSQL_ALARMTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ALARMTABLESYNC_H

#include "02_db/SQLiteTableSyncTemplate.h"

#include "04_time/DateTime.h"

#include "17_messages/Alarm.h"

#include <string>

namespace synthese
{
	namespace messages
	{
		class ScenarioSentAlarm;
		class SingleSentAlarm;
		class AlarmTemplate;
		class SentScenario;
		class Scenario;


		/** Alarm SQLite table synchronizer.
			@ingroup m17LS refLS
			@warning (for the future sqlite node synchronizer) The alarm table sync insertion hook must always be run after the one for the scenario

			Only the sent alarms are loaded in ram.

			@note As Alarm is an abstract class, do not use the get static method. Use getAlarm instead.
		*/
		class AlarmTableSync : public db::SQLiteTableSyncTemplate<Alarm>
		{

		public:
			static const std::string COL_LEVEL;
			static const std::string COL_IS_TEMPLATE;
			static const std::string COL_SHORT_MESSAGE;
			static const std::string COL_LONG_MESSAGE;
			static const std::string COL_PERIODSTART;
			static const std::string COL_PERIODEND;
			static const std::string COL_SCENARIO_ID;
			static const std::string COL_ENABLED;
			
			AlarmTableSync ();
			~AlarmTableSync ();

		protected:


			/** Action to do on alarm creation.
				
			*/
			void rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows, bool isFirstSync = false);

			/** Action to do on alarm update.
			
			*/
			void rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on alarm deletion.
			
			*/
			void rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		public:
			static std::vector<boost::shared_ptr<Alarm> > search(
				const Scenario* scenario
				, int first = 0
				, int number = -1
				, bool orderByLevel = false
				, bool raisingOrder = false
				);

			static std::vector<boost::shared_ptr<ScenarioSentAlarm> > searchScenarioSent(
				const SentScenario* scenario
				, int first = 0
				, int number = 0
				, bool orderByLevel = false
				, bool orderByStatus = false
				, bool orderByConflict = false
				, bool raisingOrder = false
				);
			
			static std::vector<boost::shared_ptr<SingleSentAlarm> > searchSingleSent(
				time::DateTime startDate
				, time::DateTime endDate
				, int first = 0
				, int number = 0
				, bool orderByDate = true
				, bool orderByLevel = false
				, bool orderByStatus = false
				, bool orderByConflict = false
				, bool raisingOrder = false
				);
			
			static std::vector<boost::shared_ptr<AlarmTemplate> > searchTemplates(
				const SentScenario* scenario
				, int first = 0
				, int number = 0
				, bool orderByLevel = false
				, bool raisingOrder = false
				);

			static boost::shared_ptr<Alarm> getAlarm(uid key);
		};

	}
}

#endif
