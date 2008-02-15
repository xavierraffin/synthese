
/** ScenarioTableSync class header.
	@file ScenarioTableSync.h

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

#ifndef SYNTHESE_ScenarioTableSync_H__
#define SYNTHESE_ScenarioTableSync_H__

#include "17_messages/Scenario.h"
#include "17_messages/ScenarioSubclassTemplate.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/AlarmTemplate.h"
#include "17_messages/ScenarioSentAlarm.h"

#include "04_time/DateTime.h"

#include "02_db/SQLiteInheritanceTableSyncTemplate.h"

#include <vector>
#include <string>
#include <iostream>

namespace synthese
{
	namespace messages
	{
		class SentScenario;
		class ScenarioTemplate;

		/** Scenario table synchronizer.
			@ingroup m17LS refLS

			@note As Scenario is an abstract class, do not use the get static method. Use getAlarm instead.
		*/
		class ScenarioTableSync : public db::SQLiteInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>
		{
		public:
			static const std::string COL_IS_TEMPLATE;
			static const std::string COL_ENABLED;
			static const std::string COL_NAME;
			static const std::string COL_PERIODSTART;
			static const std::string COL_PERIODEND;
			static const std::string COL_FOLDER_ID;

			ScenarioTableSync();


			/** Sent scenario search.
				@param name Name of the template scenario
				@param first First Scenario object to answer
				@param number Number of Scenario objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Scenario*> Founded Scenario objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<SentScenario> > searchSent(
				time::DateTime startDate = time::DateTime(time::TIME_UNKNOWN)
				, time::DateTime endDate = time::DateTime(time::TIME_UNKNOWN)
				, const std::string name = std::string()
				, int first = 0
				, int number = -1
				, bool orderByDate = true
				, bool orderByName = false
				, bool orderByStatus = false
				, bool orderByConflict = false
				, bool raisingOrder = false
			);

			/** Template scenario search.
				@param name Name of the template
				@param first First Scenario object to answer
				@param number Number of Scenario objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Scenario*> Founded Scenario objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<ScenarioTemplate> > searchTemplate(
				uid folderId
				, const std::string name = std::string()
				, const ScenarioTemplate* scenarioToBeDifferentWith = NULL
				, int first = 0
				, int number = -1
				, bool orderByName = true
				, bool raisingOrder = false
			);
		};



	}
}

#endif // SYNTHESE_ScenarioTableSync_H__
