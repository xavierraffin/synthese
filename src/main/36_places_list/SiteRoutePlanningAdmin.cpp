
/** SiteSiteRoutePlanningAdmin class implementation.
	@file SiteRoutePlanningAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "SiteRoutePlanningAdmin.h"

#include "15_env/Journey.h"
#include "15_env/ServiceUse.h"
#include "15_env/Line.h"
#include "15_env/CommercialLine.h"
#include "15_env/LineStop.h"
#include "15_env/Road.h"
#include "15_env/RoadChunk.h"

#include "36_places_list/PlacesListModule.h"
#include "36_places_list/Site.h"
#include "36_places_list/SiteTableSync.h"
#include "36_places_list/TransportSiteAdmin.h"

#include "33_route_planner/RoutePlanner.h"

#include "05_html/SearchFormHTMLTable.h"

#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace routeplanner;
	using namespace time;
	using namespace html;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, transportwebsite::SiteRoutePlanningAdmin>::FACTORY_KEY("SiteRoutePlanningAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<transportwebsite::SiteRoutePlanningAdmin>::ICON("arrow_switch.png");
		template<> const string AdminInterfaceElementTemplate<transportwebsite::SiteRoutePlanningAdmin>::DEFAULT_TITLE("Calcul d'itin�raires");
	}

	namespace transportwebsite
	{
		const string SiteRoutePlanningAdmin::PARAMETER_DATE_TIME("dt");
		const string SiteRoutePlanningAdmin::PARAMETER_START_CITY("sc");
		const string SiteRoutePlanningAdmin::PARAMETER_START_PLACE("sp");
		const string SiteRoutePlanningAdmin::PARAMETER_END_CITY("ec");
		const string SiteRoutePlanningAdmin::PARAMETER_END_PLACE("ep");
		const string SiteRoutePlanningAdmin::PARAMETER_RESULTS_NUMBER("rn");
		const string SiteRoutePlanningAdmin::PARAMETER_ACCESSIBILITY("ac");

		SiteRoutePlanningAdmin::SiteRoutePlanningAdmin()
			: AdminInterfaceElementTemplate<SiteRoutePlanningAdmin>()
			, _resultsNumber(UNKNOWN_VALUE)
			, _dateTime(TIME_UNKNOWN)
		{ }
		
		void SiteRoutePlanningAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_site = SiteTableSync::Get(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			}
			catch (...)
			{
				throw AdminParametersException("No such site");
			}
			_startCity = map.getString(PARAMETER_START_CITY, false, FACTORY_KEY);
			_startPlace = map.getString(PARAMETER_START_PLACE, false, FACTORY_KEY);
			_endCity = map.getString(PARAMETER_END_CITY, false, FACTORY_KEY);
			_endPlace = map.getString(PARAMETER_END_PLACE, false, FACTORY_KEY);
			_dateTime = map.getDateTime(PARAMETER_DATE_TIME, false, FACTORY_KEY);
			if (_dateTime.isUnknown())
				_dateTime = DateTime(TIME_CURRENT);
			_resultsNumber = map.getInt(PARAMETER_RESULTS_NUMBER, false, FACTORY_KEY);
			_accessibility = static_cast<AccessibilityParameter>(
				map.getInt(PARAMETER_ACCESSIBILITY, false, string())
			);
		}
		
		void SiteRoutePlanningAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<SiteRoutePlanningAdmin>();
			searchRequest.setObjectId(_site->getKey());

			// Search form
			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Commune d�part", st.getForm().getTextInput(PARAMETER_START_CITY, _startCity));
			stream << st.cell("Arr�t d�part", st.getForm().getTextInput(PARAMETER_START_PLACE, _startPlace));
			stream << st.cell("Commune arriv�e", st.getForm().getTextInput(PARAMETER_END_CITY, _endCity));
			stream << st.cell("Arr�t arriv�e", st.getForm().getTextInput(PARAMETER_END_PLACE, _endPlace));
			stream << st.cell("Date/Heure", st.getForm().getCalendarInput(PARAMETER_DATE_TIME, _dateTime));
			stream << st.cell("Nombre r�ponses", st.getForm().getSelectNumberInput(PARAMETER_RESULTS_NUMBER, 1, 99, _resultsNumber, 1, "(illimit�)"));
			stream << st.cell("Accessibilit�", st.getForm().getSelectInput(PARAMETER_ACCESSIBILITY, PlacesListModule::GetAccessibilityNames(), _accessibility));
			stream << st.close();

			// No calculation without cities
			if (_startCity.empty() || _endCity.empty())
				return;

			stream << "<h1>R�sultats</h1>";

			DateTime endDate(_dateTime);
			endDate++;

			// Route planning
			RoutePlanner r(
				_site->fetchPlace(_startCity, _startPlace)
				, _site->fetchPlace(_endCity, _endPlace)
				, _site->getAccessParameters(_accessibility)
				, PlanningOrder()
				, _dateTime
				, endDate
				, _resultsNumber
				);
			const RoutePlanner::Result& jv(r.computeJourneySheetDepartureArrival());

			if (jv.journeys.empty())
			{
				stream << "Aucun r�sultat trouv�.";
				return;
			}
			
			HTMLTable::ColsVector v;
			v.push_back("D�p<br />" + _startCity + "<br />"+ _startPlace);
			v.push_back("Ligne");
			v.push_back("Arr");
			v.push_back("Correspondance");
			v.push_back("D�p");
			v.push_back("Ligne");
			v.push_back("Arr<br />" + _endCity + "<br />"+ _endPlace);
			HTMLTable t(v,"adminresults");

			int solution(1);
			stream << t.open();
			for (JourneyBoardJourneys::const_iterator it(jv.journeys.begin()); it != jv.journeys.end(); ++it)
			{
				stream << t.row();
				stream << t.col(7, string(), true) << "Solution " << solution++;

				// Departure time
				Journey::ServiceUses::const_iterator its((*it)->getServiceUses().begin());

				if ((*it)->getContinuousServiceRange() > 1)
				{
					DateTime endRange(its->getDepartureDateTime());
					endRange += (*it)->getContinuousServiceRange();
					stream << " - Service continu jusqu'� " << endRange.toString();
				}
				if ((*it)->getReservationCompliance() == true)
				{
					stream << " - R�servation obligatoire avant le " << (*it)->getReservationDeadLine().toString();
				}
				if ((*it)->getReservationCompliance() == boost::logic::indeterminate)
				{
					stream << " - R�servation facultative avant le " << (*it)->getReservationDeadLine().toString();
				}

				stream << t.row();
				stream << t.col() << its->getDepartureDateTime().toString();

				// Line
				const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
				const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
				stream << t.col((its == ((*it)->getServiceUses().end() -1)) ? 5 : 1, ls ? ls->getLine()->getCommercialLine()->getStyle() : string());
				
				stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getName());

				// Transfers
				if (its != (*it)->getServiceUses().end() -1)
				{
					while(true)
					{
						// Arrival
						stream << t.col() << its->getArrivalDateTime().toString();

						// Place
						stream << t.col() << its->getArrivalEdge()->getPlace()->getFullName();

						// Next service use
						++its;

						// Departure
						stream << t.col() << its->getDepartureDateTime().toString();
						
						// Line
						const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
						const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
						stream << t.col(1, ls ? ls->getLine()->getCommercialLine()->getStyle() : string());
						stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getName());

						// Exit if last service use
						if (its == (*it)->getServiceUses().end() -1)
							break;

						// Empty final arrival col
						stream << t.col();

						// New row and empty origin departure cols;
						stream << t.row();
						stream << t.col();
						stream << t.col();
					}
				}

				// Final arrival
				stream << t.col() << its->getArrivalDateTime().toString();
			}
			stream << t.close();
		}

		bool SiteRoutePlanningAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}
		
		AdminInterfaceElement::PageLinks SiteRoutePlanningAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == TransportSiteAdmin::FACTORY_KEY)
			{
				PageLink link;
				link.factoryKey = FACTORY_KEY;
				link.icon = ICON;
				link.name = DEFAULT_TITLE;
				link.parameterName = QueryString::PARAMETER_OBJECT_ID;
				link.parameterValue = parentLink.parameterValue;
				links.push_back(link);
			}
			return links;
		}
		
		AdminInterfaceElement::PageLinks SiteRoutePlanningAdmin::getSubPages(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;
			return links;
		}


		std::string SiteRoutePlanningAdmin::getParameterName() const
		{
			return _site.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string SiteRoutePlanningAdmin::getParameterValue() const
		{
			return _site.get() ? Conversion::ToString(_site->getKey()) : string();
		}
	}
}
