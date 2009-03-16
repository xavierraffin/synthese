////////////////////////////////////////////////////////////////////////////////
/// MessageAdmin class implementation.
///	@file MessageAdmin.cpp
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

#include "PropertiesHTMLTable.h"
#include "HTMLTable.h"
#include "MessageAdmin.h"
#include "MessagesModule.h"
#include "17_messages/Types.h"
#include "AlarmRecipient.h"
#include "UpdateAlarmAction.h"
#include "UpdateAlarmMessagesFromTemplateAction.h"
#include "UpdateAlarmMessagesAction.h"
#include "AlarmAddLinkAction.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmTemplate.h"
#include "SingleSentAlarm.h"
#include "ScenarioSentAlarm.h"
#include "ScenarioTemplate.h"
#include "SentScenario.h"
#include "AlarmTableSync.h"
#include "MessagesAdmin.h"
#include "MessagesLibraryAdmin.h"
#include "MessagesScenarioAdmin.h"
#include "MessagesRight.h"
#include "MessagesLibraryRight.h"
#include "ActionFunctionRequest.h"
#include "QueryString.h"
#include "AdminParametersException.h"
#include "AdminRequest.h"
#include "ActionException.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace messages;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,MessageAdmin>::FACTORY_KEY("message");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<MessageAdmin>::ICON("note.png");
		template<> const string AdminInterfaceElementTemplate<MessageAdmin>::DEFAULT_TITLE("Message inconnu");
	}

	namespace messages
	{
		void MessageAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			if (_request->getObjectId() == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			try
			{
				_alarm = AlarmTableSync::Get(_request->getObjectId(), _env, UP_LINKS_LOAD_LEVEL);
			}
			catch(...)
			{
				throw AdminParametersException("Specified alarm not found");
			}

			_parameters = map;
		}
		
		


		server::ParametersMap MessageAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}



		void MessageAdmin::display(ostream& stream, interfaces::VariablesMap& variables) const
		{
			ActionFunctionRequest<UpdateAlarmAction,AdminRequest> updateRequest(_request);
			updateRequest.getFunction()->setSamePage(this);
			updateRequest.getAction()->setAlarmId(_alarm->getKey());

			shared_ptr<const SingleSentAlarm> salarm = dynamic_pointer_cast<const SingleSentAlarm, const Alarm>(_alarm);
			
			stream << "<h1>Param�tres</h1>";
			PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
			
			stream << t.open();
			stream << t.cell("Type", t.getForm().getRadioInput(UpdateAlarmAction::PARAMETER_TYPE, MessagesModule::getLevelLabels(), _alarm->getLevel()));

			if (salarm.get())
			{
				stream << t.cell("D�but diffusion", t.getForm().getCalendarInput(UpdateAlarmAction::PARAMETER_START_DATE, salarm->getPeriodStart()));
				stream << t.cell("Fin diffusion", t.getForm().getCalendarInput(UpdateAlarmAction::PARAMETER_END_DATE, salarm->getPeriodEnd()));
				stream << t.cell("Actif", t.getForm().getOuiNonRadioInput(UpdateAlarmAction::PARAMETER_ENABLED, salarm->getIsEnabled()));
			}
			stream << t.close();

			if (_alarm->getLevel() != ALARM_LEVEL_UNKNOWN)
			{
				stream << "<h1>Contenu</h1>";

				ActionFunctionRequest<UpdateAlarmMessagesFromTemplateAction,AdminRequest> templateRequest(_request);
				templateRequest.getFunction()->setSamePage(this);
				templateRequest.getAction()->setAlarmId(_alarm->getKey());

				vector<pair<uid, string> > tl(MessagesModule::getTextTemplateLabels(_alarm->getLevel()));
				if(!tl.empty())
				{
					HTMLForm fc(templateRequest.getHTMLForm("template"));
					stream << fc.open() << "<p>";
					stream << "Mod�le : ";
					stream << fc.getSelectInput(UpdateAlarmMessagesFromTemplateAction::PARAMETER_TEMPLATE_ID, tl, uid());
					stream << fc.getSubmitButton("Copier contenu");
					stream << "</p>" << fc.close();
				}

				ActionFunctionRequest<UpdateAlarmMessagesAction,AdminRequest> updateMessagesRequest(_request);
				updateMessagesRequest.getFunction()->setSamePage(this);
				updateMessagesRequest.getAction()->setAlarmId(_alarm->getKey());

				PropertiesHTMLTable tu(updateMessagesRequest.getHTMLForm("messages"));
				stream << tu.open();
				stream << tu.cell("Message court", tu.getForm().getTextAreaInput(UpdateAlarmMessagesAction::PARAMETER_SHORT_MESSAGE, _alarm->getShortMessage(), 2, 60));
				stream << tu.cell("Message long", tu.getForm().getTextAreaInput(UpdateAlarmMessagesAction::PARAMETER_LONG_MESSAGE, _alarm->getLongMessage(), 6, 60));
				stream << tu.close();

				FunctionRequest<AdminRequest> searchRequest(_request);
				searchRequest.getFunction()->setSamePage(this);

				ActionFunctionRequest<AlarmAddLinkAction,AdminRequest> addRequest(_request);
				addRequest.getFunction()->setSamePage(this);
				addRequest.getAction()->setAlarm(_alarm);

				ActionFunctionRequest<AlarmRemoveLinkAction,AdminRequest> removeRequest(_request);
				removeRequest.getFunction()->setSamePage(this);
				removeRequest.getAction()->setAlarmId(_alarm->getKey());
				
				// Alarm messages destinations loop
				vector<shared_ptr<AlarmRecipient> > recipients(Factory<AlarmRecipient>::GetNewCollection());
				BOOST_FOREACH(shared_ptr<AlarmRecipient> recipient, recipients)
				{
					addRequest.getAction()->setRecipientKey(recipient->getFactoryKey());
				
					stream << "<h1>Diffusion sur " << recipient->getTitle() << "</h1>";

					recipient->displayBroadcastListEditor(stream, _alarm.get(), _parameters, searchRequest, addRequest, removeRequest);
				}
			}
		}

		bool MessageAdmin::isAuthorized() const
		{
			if (_alarm.get() == NULL) return false;
			if (dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm).get() == NULL) return _request->isAuthorized<MessagesRight>(READ);
			return _request->isAuthorized<MessagesLibraryRight>(READ);
		}

		MessageAdmin::MessageAdmin()
			: AdminInterfaceElementTemplate<MessageAdmin>()
		{
		
		}

		AdminInterfaceElement::PageLinks MessageAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if (currentPage.getFactoryKey() == FACTORY_KEY)
			{
				const MessageAdmin& currentSPage(static_cast<const MessageAdmin&>(currentPage));
				shared_ptr<const SingleSentAlarm> alarm(dynamic_pointer_cast<const SingleSentAlarm, const Alarm>(currentSPage._alarm));
				if (alarm.get())
				{
					if (parentLink.factoryKey == MessagesAdmin::FACTORY_KEY)
						links.push_back(currentPage.getPageLink());
				}
			}
			return links;
		}

		AdminInterfaceElement::PageLinks MessageAdmin::getSubPages( const AdminInterfaceElement& currentPage
		) const	{
			return AdminInterfaceElement::PageLinks();
		}

		boost::shared_ptr<const Alarm> MessageAdmin::getAlarm() const
		{
			return _alarm;
		}

		std::string MessageAdmin::getTitle() const
		{
			return _alarm.get() ? _alarm->getShortMessage() : DEFAULT_TITLE;
		}

		std::string MessageAdmin::getParameterName() const
		{
			return _alarm.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string MessageAdmin::getParameterValue() const
		{
			return _alarm.get() ? Conversion::ToString(_alarm->getKey()) : string();
		}

		bool MessageAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			if (currentPage.getFactoryKey() != MessagesScenarioAdmin::FACTORY_KEY)
				return false;

			const shared_ptr<const ScenarioSentAlarm> salarm(dynamic_pointer_cast<const ScenarioSentAlarm, const Alarm>(_alarm));
			if (salarm.get())
				return salarm->getScenario()->getKey() == Conversion::ToLongLong(currentPage.getPageLink().parameterValue);

			const shared_ptr<const AlarmTemplate> talarm(dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm));
			if (talarm.get())
				return talarm->getScenario()->getKey() == Conversion::ToLongLong(currentPage.getPageLink().parameterValue);

			return false;
		}
	}
}
