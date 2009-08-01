////////////////////////////////////////////////////////////////////////////////
/// CommercialLineAdmin class implementation.
///	@file CommercialLineAdmin.cpp
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

#include "CommercialLineAdmin.h"
#include "TransportNetworkAdmin.h"
#include "EnvModule.h"
#include "NonConcurrencyRuleTableSync.h"
#include "NonConcurrencyRule.h"
#include "TransportNetwork.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "Line.h"
#include "LineAdmin.h"
#include "LineTableSync.h"
#include "TransportNetworkRight.h"
#include "TridentExportFunction.h"
#include "AdminInterfaceElement.h"
#include "AdminFunctionRequest.hpp"

#include "AdminParametersException.h"
#include "SearchFormHTMLTable.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace security;
	using namespace html;
	using namespace time;
	using namespace pt;
	
	

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, CommercialLineAdmin>::FACTORY_KEY("CommercialLineAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<CommercialLineAdmin>::ICON("chart_line.png");
		template<> const string AdminInterfaceElementTemplate<CommercialLineAdmin>::DEFAULT_TITLE("Ligne inconnue");
	}

	namespace env
	{
		const string CommercialLineAdmin::TAB_DATES("da");
		const string CommercialLineAdmin::TAB_ROUTES("ro");
		const string CommercialLineAdmin::TAB_NON_CONCURRENCY("nc");
		const string CommercialLineAdmin::TAB_EXPORT("ex");
		const string CommercialLineAdmin::PARAMETER_SEARCH_NAME("na");
		const string CommercialLineAdmin::PARAMETER_DATES_START("ds");
		const string CommercialLineAdmin::PARAMETER_DATES_END("de");

		
		void CommercialLineAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			_searchName = map.getString(PARAMETER_SEARCH_NAME, false, FACTORY_KEY);
			_startDate = map.getOptionalDate(PARAMETER_DATES_START);
			_endDate = map.getOptionalDate(PARAMETER_DATES_END);

			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_NAME, 100);

			try
			{
				_cline = CommercialLineTableSync::Get(map.getUid(Request::PARAMETER_OBJECT_ID, true, FACTORY_KEY), _getEnv(), UP_LINKS_LOAD_LEVEL);
			}
			catch (...)
			{
				throw AdminParametersException("No such line");
			}
		}
		
		
		
		server::ParametersMap CommercialLineAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_cline.get()) m.insert(Request::PARAMETER_OBJECT_ID, _cline->getKey());
			return m;
		}



		void CommercialLineAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const {
			////////////////////////////////////////////////////////////////////
			// TAB STOPS
			if (openTabContent(stream, TAB_ROUTES))
			{
				// Requests
				AdminFunctionRequest<CommercialLineAdmin> searchRequest(_request);
				
				// Search form
				stream << "<h1>Recherche</h1>";
				
				SearchFormHTMLTable s(searchRequest.getHTMLForm("search"));
				stream << s.open();
				stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, _searchName));
				HTMLForm sortedForm(s.getForm());
				stream << s.close();


				// Results display
				stream << "<h1>Parcours de la ligne</h1>";

				LineTableSync::SearchResult routes(
					LineTableSync::Search(
						_getEnv(),
						_cline->getKey(),
						UNKNOWN_VALUE,
						_requestParameters.first,
						_requestParameters.maxSize,
						_requestParameters.orderField == PARAMETER_SEARCH_NAME,
						_requestParameters.raisingOrder
				)	);
				
				ResultHTMLTable::HeaderVector h;
				h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
				h.push_back(make_pair(string(), "Actions"));
				
				ResultHTMLTable t(h,sortedForm,_requestParameters, routes);

				stream << t.open();
				AdminFunctionRequest<LineAdmin> lineOpenRequest(_request);
 				BOOST_FOREACH(shared_ptr<Line> line, routes)
				{
					lineOpenRequest.getPage()->setLine(line);
					stream << t.row();
					stream << t.col();
					stream << line->getName();
					stream << t.col();
					stream << HTMLModule::getLinkButton(lineOpenRequest.getURL(), "Ouvrir", string(), "chart_line_edit.png");
				}
				stream << t.close();
			}
			////////////////////////////////////////////////////////////////////
			// TAB HOURS
			if (openTabContent(stream, TAB_DATES))
			{
				RunHours _runHours(
					getCommercialLineRunHours(_getEnv(), _cline->getKey(), _startDate, _endDate)
				);

				stream << "<style>td.red {background-color:red;width:8px; height:8px; color:white; text-align:center; } td.green {background-color:#008000;width:10px; height:10px; color:white; text-align:center; }</style>"; 
				HTMLTable::ColsVector cols;
				cols.push_back("Date");
				for(int i(0); i<=23; ++i)
				{
					cols.push_back(Conversion::ToString(i));
				}
				optional<Date> date;
				int lastHour;
				HTMLTable t(cols, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const RunHours::value_type& it, _runHours)
				{
					if(!date || date != it.first.first)
					{
						if (date)
						{
							for(int i(lastHour+1); i<=23; ++i)
							{
								stream << t.col(1, "red") << "0";
							}
							for((*date)++; *date < it.first.first; (*date)++)
							{
								stream << t.row();
								stream << t.col(1, string(), true) << date->toString();
								for(int i(0); i<=23; ++i)
								{
									stream << t.col(1, "red") << "0";
								}
							}
						}
						date = it.first.first;
						stream << t.row();
						stream << t.col(1, string(), true) << date->toString();
						lastHour = -1;
					}
					for(int i(lastHour+1); i<it.first.second; ++i)
					{
						stream << t.col(1, "red") << "0";
					}
					stream << t.col(1, "green") << it.second;
					lastHour = it.first.second;
				}
				if(date)
				{
					for(int i(lastHour+1); i<=23; ++i)
					{
						stream << t.col(1, "red") << "0";
					}
				}
				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB NON CONCURRENCY
			if (openTabContent(stream, TAB_NON_CONCURRENCY))
			{
				stream << "<h1>Lignes prioritaires</h1>";

				NonConcurrencyRuleTableSync::SearchResult rules(NonConcurrencyRuleTableSync::Search(_getEnv(), _cline->getKey()));
				HTMLTable::ColsVector cols;
				cols.push_back("R�seau");
				cols.push_back("Ligne");
				cols.push_back("D�lai");
				cols.push_back("Action");
				HTMLTable t(cols, ResultHTMLTable::CSS_CLASS);
				stream << t.open();

				BOOST_FOREACH(shared_ptr<NonConcurrencyRule> rule, rules)
				{
					stream << t.row();
					stream << t.col() << rule->getPriorityLine()->getNetwork()->getName();
					stream << t.col(1, rule->getPriorityLine()->getStyle()) << rule->getPriorityLine()->getShortName();
					stream << t.col() << rule->getDelay().minutes();
					stream << t.col() << "Supprimer";
				}

				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB EXPORT
			if (openTabContent(stream, TAB_EXPORT))
			{
				FunctionRequest<TridentExportFunction> tridentExportFunction(&_request);
				tridentExportFunction.getFunction()->setCommercialLine(_cline);
				stream << "<h1>Formats Trident</h1>";
				stream << "<p>";
				stream << HTMLModule::getLinkButton(tridentExportFunction.getURL(), "Export Trident standard", string(), "page_white_go.png");
				stream << " ";
				tridentExportFunction.getFunction()->setWithTisseoExtension(true);
				stream << HTMLModule::getLinkButton(tridentExportFunction.getURL(), "Export Trident Tiss�o", string(), "page_white_go.png");
				stream << "</p>";
			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}

		bool CommercialLineAdmin::isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const
		{
			if (_cline.get() == NULL) return false;
			return _request.isAuthorized<TransportNetworkRight>(READ);
		}
		
		
		std::string CommercialLineAdmin::getTitle() const
		{
			return _cline.get() ? "<span class=\"linesmall " + _cline->getStyle() +"\">" + _cline->getShortName() + "</span>" : DEFAULT_TITLE;
		}

		AdminInterfaceElement::PageLinks CommercialLineAdmin::getSubPages(
			shared_ptr<const AdminInterfaceElement> currentPage,
				const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			const LineAdmin* la(
				dynamic_cast<const LineAdmin*>(currentPage.get())
			);
			
			const CommercialLineAdmin* ca(
				dynamic_cast<const CommercialLineAdmin*>(currentPage.get())
			);
			
			if(	la &&
				la->getLine().get() &&
				la->getLine()->getCommercialLine() &&
				la->getLine()->getCommercialLine()->getKey() == _cline->getKey() ||
				ca &&
				ca->getCommercialLine().get() &&
				ca->getCommercialLine()->getKey() == _cline->getKey()
			){
				LineTableSync::SearchResult routes(
					LineTableSync::Search(*_env, _cline->getKey())
				);
				BOOST_FOREACH(shared_ptr<Line> line, routes)
				{
					if(	la &&
						la->getLine()->getKey() == line->getKey()
					){
						AddToLinks(links, currentPage);
					}
					else
					{
						shared_ptr<LineAdmin> p(
							getNewOtherPage<LineAdmin>()
						);
						p->setLine(line);
						AddToLinks(links, p);
					}
				}
			}
			return links;
		}

		void CommercialLineAdmin::_buildTabs(
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Parcours", TAB_ROUTES, true, LineAdmin::ICON));
			_tabs.push_back(Tab("Dates de fonctionnement", TAB_DATES, true, "calendar.png"));
			_tabs.push_back(Tab("Non concurrence", TAB_NON_CONCURRENCY, true, "lock.png"));
			_tabs.push_back(Tab("Export", TAB_EXPORT, true, "page_white_go.png"));

			_tabBuilded = true;
		}

		boost::shared_ptr<const CommercialLine> CommercialLineAdmin::getCommercialLine() const
		{
			return _cline;
		}
		
		void CommercialLineAdmin::setCommercialLine(boost::shared_ptr<CommercialLine> value)
		{
			_cline = const_pointer_cast<CommercialLine>(value);
		}
	}
}
