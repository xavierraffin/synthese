
/** PT Use Ruler class header.
	@file UseRulerAdmin.hpp

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

#ifndef SYNTHESE_graph_UseRulerAdmin_hpp__
#define SYNTHESE_graph_UseRulerAdmin_hpp__

#include "PTUseRule.h"
#include "HTMLForm.h"
#include "ResultHTMLTable.h"
#include "Request.h"
#include "AdminActionFunctionRequest.hpp"
#include "RuleUserUpdateAction.hpp"
#include "CommercialLineTableSync.h"
#include "PTModule.h"
#include "GraphConstants.h"

#include <deque>

namespace synthese
{
	namespace pt
	{
		/** UseRulerAdmin class.
			@ingroup m35
		*/
		template<class ObjectType, class AdminPage>
		class PTRuleUserAdmin
		{
		public:
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const ObjectType> object,
				const admin::AdminRequest& request
			);
		};



		template<class ObjectType, class AdminPage>
		void PTRuleUserAdmin<ObjectType,AdminPage>::Display(
			std::ostream& stream,
			boost::shared_ptr<const ObjectType> object,
			const admin::AdminRequest& request
		){
			stream << "<h1>Conditions d'utilisation par profil</h1>";

			html::HTMLTable::ColsVector c;
			c.push_back(std::string());
			c.push_back("Pi�tons");
			c.push_back("Handicap�s");
			c.push_back("V�los");

			admin::AdminActionFunctionRequest<graph::RuleUserUpdateAction,AdminPage> updateRequest(request);
			updateRequest.getAction()->setId(object->getKey());

			html::HTMLForm f(updateRequest.getHTMLForm());

			stream << f.open();
			html::HTMLTable t(c, html::ResultHTMLTable::CSS_CLASS);
			stream << t.open();

			std::deque<const graph::RuleUser*> users;
			for(const graph::RuleUser* user(object->_getParentRuleUser()); user; user = user->_getParentRuleUser())
			{
				users.push_front(user);
			}

			BOOST_FOREACH(const graph::RuleUser* user, users)
			{
				stream << t.row();
				stream << t.col(1, std::string(), true) << user->getRuleUserName();
				stream << t.col() << (user->getRule(graph::USER_PEDESTRIAN) ? user->getRule(graph::USER_PEDESTRIAN)->getUseRuleName() : "(non d�fini)");
				stream << t.col() << (user->getRule(graph::USER_HANDICAPPED) ? user->getRule(graph::USER_HANDICAPPED)->getUseRuleName() : "(non d�fini)");
				stream << t.col() << (user->getRule(graph::USER_BIKE) ? user->getRule(graph::USER_BIKE)->getUseRuleName() : "(non d�fini)");
			}

			stream << t.row();
			stream << t.col(1, std::string(), true) << object->getRuleUserName();
			stream << t.col() << f.getSelectInput(graph::RuleUserUpdateAction::PARAMETER_VALUE_PREFIX + CommercialLineTableSync::COL_PEDESTRIAN_USE_RULE, PTModule::GetPTUseRuleLabels(), dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_PEDESTRIAN)) ? dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_PEDESTRIAN))->getKey() : util::RegistryKeyType(0));
			stream << t.col() << f.getSelectInput(graph::RuleUserUpdateAction::PARAMETER_VALUE_PREFIX + CommercialLineTableSync::COL_HANDICAPPED_USE_RULE, PTModule::GetPTUseRuleLabels(), dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_HANDICAPPED)) ? dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_HANDICAPPED))->getKey() : util::RegistryKeyType(0));
			stream << t.col() << f.getSelectInput(graph::RuleUserUpdateAction::PARAMETER_VALUE_PREFIX + CommercialLineTableSync::COL_BIKE_USE_RULE, PTModule::GetPTUseRuleLabels(), dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_BIKE)) ? dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_BIKE))->getKey() : util::RegistryKeyType(0));

			stream << t.row();
			stream << t.col(4, std::string(), true) << f.getSubmitButton("Enregistrer les modifications");

			stream << t.close();
			stream << f.close();
		}
	}
}

#endif // SYNTHESE_graph_UseRulerAdmin_hpp__
