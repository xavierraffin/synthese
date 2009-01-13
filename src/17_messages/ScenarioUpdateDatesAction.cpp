////////////////////////////////////////////////////////////////////////////////
/// ScenarioUpdateDatesAction class implementation.
///	@file ScenarioUpdateDatesAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ScenarioUpdateDatesAction.h"
#include "RequestMissingParameterException.h"
#include "MessagesModule.h"
#include "ScenarioTableSync.h"
#include "SentScenario.h"
#include "ScenarioTemplate.h"
#include "SentScenarioInheritedTableSync.h"
#include "MessagesLog.h"
#include "MessagesRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "TimeParseException.h"
#include "Conversion.h"
#include "DBLogModule.h"

#include <sstream>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace time;
	using namespace util;
	using namespace security;
	using namespace dblog;

	template<> const string util::FactorableTemplate<Action,messages::ScenarioUpdateDatesAction>::FACTORY_KEY("ScenarioUpdateDatesAction");
		
	namespace messages
	{
		const string ScenarioUpdateDatesAction::PARAMETER_ENABLED(Action_PARAMETER_PREFIX + "ena");
		const string ScenarioUpdateDatesAction::PARAMETER_START_DATE(Action_PARAMETER_PREFIX + "sda");
		const string ScenarioUpdateDatesAction::PARAMETER_END_DATE(Action_PARAMETER_PREFIX + "eda");
		const string ScenarioUpdateDatesAction::PARAMETER_VARIABLE(Action_PARAMETER_PREFIX + "var");
		const string ScenarioUpdateDatesAction::PARAMETER_SCENARIO_ID(Action_PARAMETER_PREFIX + "sid");

		ParametersMap ScenarioUpdateDatesAction::getParametersMap() const
		{
			ParametersMap map;
			if (_scenario.get()) map.insert(PARAMETER_SCENARIO_ID, _scenario->getKey());
			return map;
		}

		void ScenarioUpdateDatesAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				setScenarioId(map.getUid(PARAMETER_SCENARIO_ID, true, FACTORY_KEY));
				
				_enabled = map.getBool(PARAMETER_ENABLED, true, false, FACTORY_KEY);

				_startDate = map.getDateTime(PARAMETER_START_DATE, true, FACTORY_KEY);

				_endDate = map.getDateTime(PARAMETER_END_DATE, true, FACTORY_KEY);

				const ScenarioTemplate::VariablesMap& variables(_scenario->getTemplate()->getVariables());
				BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
				{
					_variables.insert(make_pair(variable.second.code, map.getString(PARAMETER_VARIABLE + variable.second.code, variable.second.compulsory, FACTORY_KEY)));
				}
			}
			catch(RequestMissingParameterException& e)
			{
				throw ActionException(e.getMessage());
			}
			catch(TimeParseException& e)
			{
				throw ActionException("Une date ou une heure est mal form�e");
			}

		}

		ScenarioUpdateDatesAction::ScenarioUpdateDatesAction()
			: FactorableTemplate<Action, ScenarioUpdateDatesAction>()
			, _startDate(TIME_UNKNOWN)
			, _endDate(TIME_UNKNOWN)
		{}

		void ScenarioUpdateDatesAction::run()
		{
			// Log message
			stringstream text;
			DBLogModule::appendToLogIfChange(text, "Affichage ", _scenario->getIsEnabled() ? "activ�" : "d�sactiv�", _enabled ? "activ�" : "d�sactiv�");
			DBLogModule::appendToLogIfChange(text, "Date de d�but", _scenario->getPeriodStart().toString(), _startDate.toString());
			DBLogModule::appendToLogIfChange(text, "Date de fin", _scenario->getPeriodEnd().toString(), _endDate.toString());

			const ScenarioTemplate::VariablesMap& variables(_scenario->getTemplate()->getVariables());
			BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
			{
				string value;
				SentScenario::VariablesMap::const_iterator it(_scenario->getVariables().find(variable.second.code));
				if (it != _scenario->getVariables().end()) value = it->second;

				DBLogModule::appendToLogIfChange(text, variable.second.code, value, _variables[variable.second.code]);
			}

			// Action
			_scenario->setIsEnabled(_enabled);
			_scenario->setPeriodStart(_startDate);
			_scenario->setPeriodEnd(_endDate);
			_scenario->setVariables(_variables);
			ScenarioTableSync::Save(_scenario.get());

			// Log
			MessagesLog::addUpdateEntry(_scenario.get(), text.str(), _request->getUser().get());
		}



		bool ScenarioUpdateDatesAction::_isAuthorized(
		) const {
			return _request->isAuthorized<MessagesRight>(WRITE);
		}



		void ScenarioUpdateDatesAction::setScenarioId(
			const util::RegistryKeyType id
		){
			try
			{
				_scenario = SentScenarioInheritedTableSync::GetEditable(id, _env, UP_LINKS_LOAD_LEVEL);
			}
			catch(ObjectNotFoundException<Scenario>& e)
			{
				throw ActionException(PARAMETER_SCENARIO_ID, id, FACTORY_KEY, e);
			}
		}
	}
}
