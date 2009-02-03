
/** NewScenarioSendAction class implementation.
	@file NewScenarioSendAction.cpp

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

#include "NewScenarioSendAction.h"
#include "SingleSentAlarmInheritedTableSync.h"
#include "SentScenario.h"
#include "ScenarioTemplate.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "SentScenarioInheritedTableSync.h"
#include "ScenarioTableSync.h"
#include "AlarmObjectLinkTableSync.h"
#include "MessagesLog.h"
#include "AlarmTemplateInheritedTableSync.h"
#include "MessagesRight.h"
#include "ActionException.h"
#include "Request.h"
#include "QueryString.h"
#include "ParametersMap.h"
#include "SingleSentAlarm.h"
#include "AlarmTemplate.h"
#include "AdminModule.h"
#include "MessagesScenarioAdmin.h"
#include "MessageAdmin.h"
#include "MessagesLibraryLog.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace dblog;
	using namespace util;
	using namespace security;
	using namespace admin;

	template<> const string util::FactorableTemplate<Action, messages::NewScenarioSendAction>::FACTORY_KEY(
		"nssa"
	);
	
	namespace messages
	{
		const string NewScenarioSendAction::PARAMETER_TEMPLATE = Action_PARAMETER_PREFIX + "tpl";
		const string NewScenarioSendAction::PARAMETER_MESSAGE_TO_COPY(Action_PARAMETER_PREFIX + "mt");


		ParametersMap NewScenarioSendAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void NewScenarioSendAction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType id(map.getUid(PARAMETER_MESSAGE_TO_COPY, false, FACTORY_KEY));
			if(id > 0)
			{
				if(decodeTableId(id) == ScenarioTableSync::TABLE.ID)
				{
					try
					{
						_scenarioToCopy = SentScenarioInheritedTableSync::Get(id, _env);
					}
					catch(Exception& e)
					{
						throw ActionException("scenario to copy", id, FACTORY_KEY, e);
					}
				} else {
					try
					{
						_messageToCopy = SingleSentAlarmInheritedTableSync::Get(id, _env);
					}
					catch(Exception& e)
					{
						throw ActionException("message to copy", id, FACTORY_KEY, e);
					}
				}
			} else {
				// Template to source
				id = map.getUid(PARAMETER_TEMPLATE, false, FACTORY_KEY);
				if(id > 0)
				{
					try
					{
						_template = ScenarioTemplateInheritedTableSync::Get(id, _env);
					}
					catch(Exception& e)
					{
						throw ActionException("scenario template", id, FACTORY_KEY, e);
					}
				}
			}
			
			// Anti error
			_request->setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void NewScenarioSendAction::run()
		{
			if(_scenarioToCopy.get()) // Copy of an existing scenario
			{
				// The action on the scenario
				SentScenario scenario(_scenarioToCopy->getTemplate()->getName());
				ScenarioTableSync::Save(&scenario);
	
				// Remember of the id of created object to view it after the action
				_request->setObjectId(scenario.getKey());
				AdminModule::ChangePageInRequest(
					*_request,
					MessageAdmin::FACTORY_KEY,
					MessagesScenarioAdmin::FACTORY_KEY
				);
					
				SentScenarioInheritedTableSync::CopyMessagesFromTemplate(
	 				_scenarioToCopy->getTemplate()->getKey(),
	 				scenario
	 			);
	 			
				// The log
				MessagesLog::AddNewSentScenarioEntry(
					*_scenarioToCopy, scenario, _request->getUser().get()
				);
				MessagesLibraryLog::AddTemplateInstanciationEntry(
					scenario, _request->getUser().get()
				);
			}
			else if(_messageToCopy.get()) // Copy of an existing message
			{
				SingleSentAlarm alarm(*_messageToCopy);
				
				AlarmTableSync::Save(&alarm);
				
				AlarmObjectLinkTableSync::CopyRecipients(
					_messageToCopy->getKey(),
					alarm.getKey()
				);

				_request->setObjectId(alarm.getKey());
				AdminModule::ChangePageInRequest(
					*_request,
					MessagesScenarioAdmin::FACTORY_KEY,
					MessageAdmin::FACTORY_KEY
				);
				
				MessagesLog::AddNewSingleMessageEntry(
					alarm,
					*_messageToCopy,
					_request->getUser().get()
				);
			}
			else if(_template.get()) // New scenario from template
			{
				// The action on the scenario
				SentScenario scenario(*_template);
				ScenarioTableSync::Save(&scenario);
	
				// Remember of the id of created object to view it after the action
				_request->setObjectId(scenario.getKey());
				AdminModule::ChangePageInRequest(
					*_request,
					MessageAdmin::FACTORY_KEY,
					MessagesScenarioAdmin::FACTORY_KEY
				);
				
				// The action on the alarms
				SentScenarioInheritedTableSync::CopyMessagesFromTemplate(
	 				_template->getKey(),
	 				scenario
	 			);
			
				// The log
				MessagesLog::AddNewSentScenarioEntry(
					*_template, scenario, _request->getUser().get()
				);
				MessagesLibraryLog::AddTemplateInstanciationEntry(
					scenario, _request->getUser().get()
				);
			}
			else	// New message from scratch
			{
				SingleSentAlarm alarm;
				
				AlarmTableSync::Save(&alarm);
				
				_request->setObjectId(alarm.getKey());
				
				AdminModule::ChangePageInRequest(
					*_request,
					MessagesScenarioAdmin::FACTORY_KEY,
					MessageAdmin::FACTORY_KEY
				);
				
				MessagesLog::AddNewSingleMessageEntry(
					alarm,
					_request->getUser().get()
				);
			}
		}



		bool NewScenarioSendAction::_isAuthorized(
		) const {
			return _request->isAuthorized<MessagesRight>(WRITE);
		}
	}
}
