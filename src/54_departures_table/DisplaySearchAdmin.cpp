
/** DisplaySearchAdmin class implementation.
	@file DisplaySearchAdmin.cpp

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

#include "DisplaySearchAdmin.h"
#include "AdvancedSelectTableSync.h"
#include "DisplayScreenTableSync.h"
#include "CreateDisplayScreenAction.h"
#include "DisplayAdmin.h"
#include "DisplayScreenContentRequest.h"
#include "DisplayScreen.h"
#include "DisplayType.h"
#include "ArrivalDepartureTableRight.h"
#include "DeparturesTableModule.h"
#include "BroadcastPointsAdmin.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayMonitoringStatusTableSync.h"
#include "InterfaceModule.h"
#include "Conversion.h"
#include "ActionResultHTMLTable.h"
#include "SearchFormHTMLTable.h"
#include "ActionFunctionRequest.h"
#include "AdminModule.h"
#include "AdminRequest.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"
#include "City.h"
#include "SentAlarm.h"
#include "Request.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUAdmin.h"
#include "DisplayScreenCPUCreateAction.h"
#include "DisplayScreenCPUTableSync.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace html;
	using namespace departurestable;
	using namespace security;
	using namespace messages;
	

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,DisplaySearchAdmin>::FACTORY_KEY("0displays");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplaySearchAdmin>::ICON("computer.png");
		template<> const string AdminInterfaceElementTemplate<DisplaySearchAdmin>::DEFAULT_TITLE("Equipements");
	}

	namespace departurestable
	{
		const string DisplaySearchAdmin::TAB_CPU("cp");
		const string DisplaySearchAdmin::TAB_DISPLAY_SCREENS("ds");
		const string DisplaySearchAdmin::PARAMETER_SEARCH_CITY = "dsascity";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_STOP = "dsaslid";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_NAME = "dsasloc";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_LINE_ID = "dsasli";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_TYPE_ID = "dsasti";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_STATE = "dsass";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_MESSAGE = "dsasm";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_LOCALIZATION_ID("dsapsli");
		
		DisplaySearchAdmin::DisplaySearchAdmin()
			: AdminInterfaceElementTemplate<DisplaySearchAdmin>()
			, _searchLineId(UNKNOWN_VALUE)
			, _searchTypeId(UNKNOWN_VALUE)
			, _searchState(UNKNOWN_VALUE)
			, _searchMessage(UNKNOWN_VALUE)
		{}

		void DisplaySearchAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			setPlace(map.getUid(PARAMETER_SEARCH_LOCALIZATION_ID, false, FACTORY_KEY));
			if (!_place)
			{
				_searchCity = map.getString(PARAMETER_SEARCH_CITY, false, FACTORY_KEY);
				_searchStop = map.getString(PARAMETER_SEARCH_STOP, false, FACTORY_KEY);
				_searchName = map.getString(PARAMETER_SEARCH_NAME, false, FACTORY_KEY);
				_searchLineId = map.getUid(PARAMETER_SEARCH_LINE_ID, false, FACTORY_KEY);
				_searchTypeId = map.getUid(PARAMETER_SEARCH_TYPE_ID, false, FACTORY_KEY);
				_searchState = map.getInt(PARAMETER_SEARCH_STATE, false, FACTORY_KEY);
				_searchMessage = map.getInt(PARAMETER_SEARCH_MESSAGE, false, FACTORY_KEY);
			}

			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_CITY, 30);

			if(!doDisplayPreparationActions) return;
			
				DisplayScreenTableSync::Search(
					_env,
					_request->getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>()
					, _request->getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ
					, READ
					, UNKNOWN_VALUE
					, _place ? (*_place ? (*_place)->getKey() : 0) : UNKNOWN_VALUE
					, _searchLineId
					, _searchTypeId
					, _searchCity
					, _searchStop
					, _searchName
					, _searchState
					, _searchMessage
					, _requestParameters.first
					, _requestParameters.maxSize
					, false
					, _requestParameters.orderField == PARAMETER_SEARCH_CITY
					, _requestParameters.orderField == PARAMETER_SEARCH_STOP
					, _requestParameters.orderField == PARAMETER_SEARCH_NAME
					, _requestParameters.orderField == PARAMETER_SEARCH_TYPE_ID
					, _requestParameters.orderField == PARAMETER_SEARCH_STATE
					, _requestParameters.orderField == PARAMETER_SEARCH_MESSAGE
					, _requestParameters.raisingOrder
					);
				_resultParameters.setFromResult(_requestParameters, _env.getEditableRegistry<DisplayScreen>());

				DisplayScreenCPUTableSync::Search(
					_env,
					_place ? (_place->get() ? (*_place)->getKey() : 0) : optional<RegistryKeyType>(),
					optional<string>(),
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAMETER_SEARCH_NAME,
					_requestParameters.raisingOrder
				);
		}
		
		
		
		server::ParametersMap DisplaySearchAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_place)
			{
				m.insert(PARAMETER_SEARCH_LOCALIZATION_ID, _place->get() ? (*_place)->getKey() : 0);
			} else {
				m.insert(PARAMETER_SEARCH_CITY, _searchCity);
				m.insert(PARAMETER_SEARCH_STOP, _searchStop);
				m.insert(PARAMETER_SEARCH_NAME, _searchName);
				m.insert(PARAMETER_SEARCH_LINE_ID, _searchLineId);
			}
			m.insert(PARAMETER_SEARCH_TYPE_ID, _searchTypeId);
			m.insert(PARAMETER_SEARCH_STATE, _searchState);
			m.insert(PARAMETER_SEARCH_MESSAGE, _searchMessage);
			return m;
		}


		void DisplaySearchAdmin::display(ostream& stream, interfaces::VariablesMap& variables
		) const	{
			
			///////////////////////////////////////////////
			/// TAB SCREENS
			if (openTabContent(stream, TAB_DISPLAY_SCREENS))
			{
				ActionFunctionRequest<CreateDisplayScreenAction,AdminRequest> createDisplayRequest(
					_request
				);
				createDisplayRequest.getFunction()->setPage<DisplayAdmin>();
				createDisplayRequest.getFunction()->setActionFailedPage<DisplaySearchAdmin>();
				if(_place)
				{
					createDisplayRequest.getAction()->setPlace(_place->get() ? (*_place)->getKey() : 0);
				}
				createDisplayRequest.setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);


				FunctionRequest<AdminRequest> searchRequest(_request);
				searchRequest.getFunction()->setSamePage(this);

				FunctionRequest<AdminRequest> updateRequest(_request);
				updateRequest.getFunction()->setPage<DisplayAdmin>();

				FunctionRequest<DisplayScreenContentRequest> viewRequest(_request);

				if (!_place)
				{
					stream << "<h1>Recherche</h1>";

					stream << getHtmlSearchForm(searchRequest.getHTMLForm(), _searchCity, _searchStop, _searchName,  _searchLineId, _searchTypeId, _searchState, _searchMessage);
				}

				stream << "<h1>" << (_place ? "Afficheurs" : "R�sultats de la recherche") << "</h1>";

				ActionResultHTMLTable::HeaderVector v;
				if (!_place)
				{
					v.push_back(make_pair(PARAMETER_SEARCH_CITY, "Commune"));
					v.push_back(make_pair(PARAMETER_SEARCH_STOP, "Arr�t"));
				}
				v.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
				v.push_back(make_pair(PARAMETER_SEARCH_TYPE_ID, "Type"));
				v.push_back(make_pair(PARAMETER_SEARCH_STATE, "Etat"));
				v.push_back(make_pair(PARAMETER_SEARCH_MESSAGE, "Contenu"));
				v.push_back(make_pair(string(), "Actions"));
				v.push_back(make_pair(string(), "Actions"));

				ActionResultHTMLTable t(
					v
					, searchRequest.getHTMLForm()
					, _requestParameters
					, _resultParameters
					, createDisplayRequest.getHTMLForm("create")
					, CreateDisplayScreenAction::PARAMETER_TEMPLATE_ID
					, InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE)
				);
				t.getActionForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();

				BOOST_FOREACH(shared_ptr<DisplayScreen> screen, _env.getRegistry<DisplayScreen>())
				{
					updateRequest.setObjectId(screen->getKey());
					viewRequest.setObjectId(screen->getKey());
					
					vector<shared_ptr<SentAlarm> > alarms(
						DisplayScreenTableSync::GetCurrentDisplayedMessage(_env, screen->getKey(), 1)
					);
					shared_ptr<SentAlarm> alarm(alarms.empty() ? shared_ptr<SentAlarm>() : alarms.front());

					stream << t.row(Conversion::ToString(screen->getKey()));
					if (!_place || !_place->get())
					{
						stream <<
							t.col() <<
							(	screen->getLocalization() ?
								screen->getLocalization()->getCity()->getName() :
								"(ind�termin�)"
							)
						;
						stream <<
							t.col() <<
							(	screen->getLocalization() ?
								screen->getLocalization()->getName() :
								"(ind�termin�)"
							)
						;
					}
					stream << t.col() << screen->getLocalizationComment();
					stream <<
						t.col() <<
						(	screen->getType() ?
							screen->getType()->getName() :
							HTMLModule::getHTMLImage("error.png", "Type non d�fini")
						)
					;

					// Monitoring status
					bool monitored(screen->isMonitored());
					stream <<
						t.col();

					if(screen->getType() == NULL)
					{
						stream <<
							HTMLModule::getHTMLImage(
								"error.png",
								"Veuillez d�finir le type d'afficheur dans l'�cran de configuration."
							)
						;
					}
					else if(!screen->getIsOnline())
					{
						stream <<
							HTMLModule::getHTMLImage(
								"cross.png",
								"D�sactiv� par la maintenance : "+ screen->getMaintenanceMessage()
							)
						;
					}
					else if(!monitored)
					{
						stream << HTMLModule::getHTMLImage(
								"help.png",
								"Ce type d'afficheur n'est pas supervis�. Voir la d�finition du type."
							)
						;
					}
					else
					{
						shared_ptr<DisplayMonitoringStatus> status(
							DisplayMonitoringStatusTableSync::GetStatus(screen->getKey())
						);

						if(status.get() == NULL)
						{
							stream <<
								HTMLModule::getHTMLImage("exclamation.png", "Cet afficheur n'est jamais entr� en contact.")
							;
						}
						else if(screen->isDown(*status)
						){
							stream <<
								HTMLModule::getHTMLImage("exclamation.png", "Cet afficheur n'est plus en contact alors qu'il est d�clar� online.")
							;
						}
						else
						{
							stream <<
								HTMLModule::getHTMLImage(
									DisplayMonitoringStatus::GetStatusIcon(status->getGlobalStatus()),
									DisplayMonitoringStatus::GetStatusString(status->getGlobalStatus())
								)
							;
						}
					}

					// Content
					stream << t.col();
					if (!screen->getIsOnline())
					{
						stream <<
							HTMLModule::getHTMLImage(
								"cross.png",
								"D�sactiv� par la maintenance : "+ screen->getMaintenanceMessage()
							)
						;
					}
					else
					{
						if (alarm.get() != NULL)
						{
							stream <<
								HTMLModule::getHTMLImage(
									(	(alarm->getLevel() == ALARM_LEVEL_WARNING) ?
										"full_screen_message_display.png" :
										"partial_message_display.png"
									),
									"Message : " + alarm->getShortMessage()
								)
							;
						}
						else
						{
							if (DisplayScreenTableSync::GetIsAtLeastALineDisplayed(screen->getKey()))
							{
								stream <<
									HTMLModule::getHTMLImage("times_display.png", "Affichage d'horaires en cours")
								;
							}
							else
							{
								stream <<
									HTMLModule::getHTMLImage("empty_display.png", "Aucune ligne affich�e, �cran vide")
								;
							}
						}
					}
					stream << t.col() << HTMLModule::getLinkButton(updateRequest.getURL(), "Modifier", string(), "monitor_edit.png");
					stream << t.col() << HTMLModule::getLinkButton(viewRequest.getURL(), "Simuler", string(), "monitor_go.png");
				}

				if (tabHasWritePermissions())
				{
					stream << t.row();
					stream << t.col(_place.get() ? 4 : 6) << "(s�lectionner un afficheur existant pour copier ses&nbsp;propri�t�s dans le nouvel �l�ment)";
					stream << t.col(3) << t.getActionForm().getSubmitButton("Cr�er un nouvel afficheur");
				}

				stream << t.close();
			}
			
			///////////////////////////////////////////////
			/// TAB CPU
			if (openTabContent(stream, TAB_CPU))
			{
				ActionFunctionRequest<DisplayScreenCPUCreateAction,AdminRequest> createCPURequest(
					_request
				);
				createCPURequest.getFunction()->setPage<DisplayScreenCPUAdmin>();
				createCPURequest.getFunction()->setActionFailedPage<DisplaySearchAdmin>();
				if(_place)
				{
					createCPURequest.getAction()->setPlace(*_place);
				}
				createCPURequest.setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);


				FunctionRequest<AdminRequest> searchRequest(_request);
				searchRequest.getFunction()->setSamePage(this);

				FunctionRequest<AdminRequest> updateRequest(_request);
				updateRequest.getFunction()->setPage<DisplayScreenCPUAdmin>();

				if (!_place)
				{
					stream << "<h1>Recherche</h1>";

					stream << getHtmlSearchForm(searchRequest.getHTMLForm(), _searchCity, _searchStop, _searchName,  _searchLineId, _searchTypeId, _searchState, _searchMessage);
				}

				stream << "<h1>" << (_place && _place->get() ? "Unit�s centrales" : "R�sultats de la recherche") << "</h1>";

				ActionResultHTMLTable::HeaderVector v;
				if (!_place)
				{
					v.push_back(make_pair(PARAMETER_SEARCH_CITY, "Commune"));
					v.push_back(make_pair(PARAMETER_SEARCH_STOP, "Arr�t"));
				}
				v.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
				v.push_back(make_pair(PARAMETER_SEARCH_STATE, "Etat"));
				v.push_back(make_pair(string(), "Actions"));

				ActionResultHTMLTable t(
					v
					, searchRequest.getHTMLForm()
					, _requestParameters
					, _resultParameters
					, createCPURequest.getHTMLForm("createCPU")
					, DisplayScreenCPUCreateAction::PARAMETER_TEMPLATE_ID
					, InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE)
				);
				t.getActionForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();

				BOOST_FOREACH(shared_ptr<DisplayScreenCPU> cpu, _env.getRegistry<DisplayScreenCPU>())
				{
					updateRequest.setObjectId(cpu->getKey());
					
					stream << t.row(Conversion::ToString(cpu->getKey()));
					if (!_place)
					{
						stream <<
							t.col() <<
							(	cpu->getPlace() ?
							cpu->getPlace()->getCity()->getName() :
						"(ind�termin�)"
							)
							;
						stream <<
							t.col() <<
							(	cpu->getPlace() ?
							cpu->getPlace()->getName() :
						"(ind�termin�)"
							)
							;
					}
					stream << t.col() << cpu->getName();

					stream << t.col();
					
					stream << t.col() << HTMLModule::getLinkButton(updateRequest.getURL(), "Modifier", string(), "monitor_edit.png");
					
				}

				if (tabHasWritePermissions())
				{
					stream << t.row();
					stream << t.col(_place.get() ? 4 : 6) << "(s�lectionner une unit� centrale existante pour copier ses&nbsp;propri�t�s dans le nouvel �l�ment)";
					stream << t.col(3) << t.getActionForm().getSubmitButton("Cr�er une nouvelle unit� centrale");
				}

				stream << t.close();

			}
			closeTabContent(stream);
		}

		std::string DisplaySearchAdmin::getHtmlSearchForm(const HTMLForm& form
			, const std::string& cityName
			, const std::string& stopName
			, const std::string& displayName
			, uid lineUid, uid typeUid, int state, int message
		){
			vector<pair<int, string> > states;
			states.push_back(make_pair(UNKNOWN_VALUE, "(tous)"));
			states.push_back(make_pair(1, "OK"));
			states.push_back(make_pair(2, "Warning"));
			states.push_back(make_pair(3, "Warning+Error"));
			states.push_back(make_pair(4, "Error"));

			vector<pair<int, string> > messages;
			messages.push_back(make_pair(UNKNOWN_VALUE, "(tous)"));
			messages.push_back(make_pair(1, "Un message"));
			messages.push_back(make_pair(2, "Conflit"));
			messages.push_back(make_pair(3, "Messages"));

			stringstream stream;
			SearchFormHTMLTable s(form);
			stream << s.open();
			stream << s.cell("Commune", s.getForm().getTextInput(PARAMETER_SEARCH_CITY, cityName));
			stream << s.cell("Arr�t", s.getForm().getTextInput(PARAMETER_SEARCH_STOP, stopName));
			stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, displayName));
			stream << s.cell("Ligne", s.getForm().getSelectInput(PARAMETER_SEARCH_LINE_ID, DeparturesTableModule::getCommercialLineWithBroadcastLabels(true), lineUid));
			stream << s.cell("Type", s.getForm().getSelectInput(PARAMETER_SEARCH_TYPE_ID, DeparturesTableModule::getDisplayTypeLabels(true, true), typeUid));
			stream << s.cell("Etat", s.getForm().getSelectInput(PARAMETER_SEARCH_TYPE_ID, states, state));
			stream << s.cell("Message", s.getForm().getSelectInput(PARAMETER_SEARCH_MESSAGE, messages, message));
			stream << s.close();

			return stream.str();
		}

		bool DisplaySearchAdmin::isAuthorized(
		) const	{
			return _request->isAuthorized<ArrivalDepartureTableRight>(READ);
		}

		AdminInterfaceElement::PageLinks DisplaySearchAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const {
			AdminInterfaceElement::PageLinks links;

			// General search page
			if (parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == DeparturesTableModule::FACTORY_KEY)
			{
				links.push_back(getPageLink());
			}


			// Spare store page
			if(parentLink.factoryKey == ModuleAdmin::FACTORY_KEY &&
				parentLink.parameterValue == DeparturesTableModule::FACTORY_KEY
			){
				DisplaySearchAdmin page;
				page.setPlace(0);
				page.setRequest(_request);
				links.push_back(page.getPageLink());
			}


			// Broadcast points search
			if (parentLink.factoryKey == BroadcastPointsAdmin::FACTORY_KEY && currentPage.getFactoryKey() == FACTORY_KEY && static_cast<const DisplaySearchAdmin&>(currentPage)._place && static_cast<const DisplaySearchAdmin&>(currentPage)._place->get())
			{
				links.push_back(currentPage.getPageLink());
			}
			return links;
		}

		std::string DisplaySearchAdmin::getTitle() const
		{
			return _place ? (_place->get() ? (*_place)->getFullName() : "Equipements en stock") : DEFAULT_TITLE;
		}

		std::string DisplaySearchAdmin::getParameterName() const
		{
			return _place ? PARAMETER_SEARCH_LOCALIZATION_ID : string();
		}

		std::string DisplaySearchAdmin::getParameterValue() const
		{
			return _place ? (_place->get() ? lexical_cast<string>((*_place)->getKey()) : "0") : string();
		}

		bool DisplaySearchAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			return true;
		}



		void DisplaySearchAdmin::_buildTabs(
		) const {
			bool writeRight(
				_place ?
				_request->isAuthorized<ArrivalDepartureTableRight>(
					WRITE,
					UNKNOWN_RIGHT_LEVEL,
					(_place->get() ? lexical_cast<string>((*_place)->getKey()) : string("0"))) :
				false
			);
			_tabs.clear();
			_tabs.push_back(Tab("Afficheurs", TAB_DISPLAY_SCREENS, writeRight, "monitor.png"));
			_tabs.push_back(Tab("Unit�s centrales", TAB_CPU, writeRight, "server.png"));
			_tabBuilded = true;
		}

		void DisplaySearchAdmin::setPlace( const util::RegistryKeyType id )
		{
			if(id == UNKNOWN_VALUE)
			{
				return;
			}
			if(id == 0)
			{
				_place = shared_ptr<const PublicTransportStopZoneConnectionPlace>();
			}
			else
			{
				try
				{
					_place = ConnectionPlaceTableSync::Get(id, _env);
				}
				catch (...)
				{
					throw AdminParametersException("Specified place not found");
				}
			}
		}
	}
}
