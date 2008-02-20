
/** DisplayMaintenanceAdmin class implementation.
	@file DisplayMaintenanceAdmin.cpp

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
#include "34_departures_table/DisplayMaintenanceAdmin.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/UpdateDisplayMaintenanceAction.h"
#include "34_departures_table/DisplaySearchAdmin.h"
#include "34_departures_table/DisplayAdmin.h"
#include "34_departures_table/DisplayMaintenanceLog.h"
#include "34_departures_table/DisplayDataControlLog.h"

#include "05_html/PropertiesHTMLTable.h"
#include "05_html/Constants.h"
#include "05_html/HTMLList.h"

#include "13_dblog/DBLogViewer.h"

#include "30_server/ActionFunctionRequest.h"
#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace dblog;
	using namespace departurestable;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DisplayMaintenanceAdmin>::FACTORY_KEY("dmaint");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplayMaintenanceAdmin>::ICON("monitor_lightning.png");
		template<> const string AdminInterfaceElementTemplate<DisplayMaintenanceAdmin>::DEFAULT_TITLE("Supervision et maintenance");
	}

	namespace departurestable
	{
		void DisplayMaintenanceAdmin::setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));

			try
			{
				_displayScreen = DisplayScreenTableSync::Get(id);
			}
			catch(...)
			{
				throw AdminParametersException("Specified display screen not found");
			}
		}


		void DisplayMaintenanceAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			ActionFunctionRequest<UpdateDisplayMaintenanceAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<DisplayMaintenanceAdmin>();
			updateRequest.setObjectId(_displayScreen->getKey());

			FunctionRequest<AdminRequest> goToLogRequest(request);
			goToLogRequest.getFunction()->setPage<DBLogViewer>();
			static_pointer_cast<DBLogViewer,AdminInterfaceElement>(goToLogRequest.getFunction()->getPage())->setLogKey(DisplayMaintenanceLog::FACTORY_KEY);
			goToLogRequest.setObjectId(request->getObjectId());

			FunctionRequest<AdminRequest> goToDataLogRequest(request);
			goToDataLogRequest.getFunction()->setPage<DBLogViewer>();
			static_pointer_cast<DBLogViewer,AdminInterfaceElement>(goToDataLogRequest.getFunction()->getPage())->setLogKey(DisplayDataControlLog::FACTORY_KEY);
			goToDataLogRequest.setObjectId(request->getObjectId());

			stream << "<h1>Param�tres de maintenance</h1>";

			PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));

			stream << t.open();
			stream << t.cell("Nombre de contr�les par jour", t.getForm().getSelectNumberInput(UpdateDisplayMaintenanceAction::PARAMETER_CONTROLS, 0, 1440, _displayScreen->getMaintenanceChecksPerDay(), 10));
			stream << t.cell("Afficheur d�clar� en service", t.getForm().getOuiNonRadioInput(UpdateDisplayMaintenanceAction::PARAMETER_ONLINE, _displayScreen->getIsOnline()));
			stream << t.cell("Message de maintenance", t.getForm().getTextAreaInput(UpdateDisplayMaintenanceAction::PARAMETER_MESSAGE, _displayScreen->getMaintenanceMessage(), 3, 30));
			stream << t.close();

			stream << "<h1>Contr�le de coh�rence des donn�es</h1>";
			
			HTMLList l;
			stream << l.open();

			stream << l.element() << "Etat : ";
			if (_displayScreen->getComplements().dataControl == DISPLAY_DATA_CORRUPTED)
				stream << HTMLModule::getHTMLImage(IMG_URL_ERROR, _displayScreen->getComplements().dataControlText);
			if (_displayScreen->getComplements().dataControl == DISPLAY_DATA_NO_LINES)
				stream << HTMLModule::getHTMLImage(IMG_URL_WARNING, _displayScreen->getComplements().dataControlText);
			if (_displayScreen->getComplements().dataControl == DISPLAY_DATA_OK)
				stream << HTMLModule::getHTMLImage(IMG_URL_INFO, "OK");

			stream << l.element() << "D�tail : ";
			stream << _displayScreen->getComplements().dataControlText;

			stream << l.element() << "Date du dernier contr�le positif : ";
			stream << _displayScreen->getComplements().lastOKDataControl.toString();

			stream << l.element("log") << HTMLModule::getHTMLLink(goToDataLogRequest.getURL(), "Acc�der au journal de surveillance des donn�es de l'afficheur");

			stream << l.close();

			stream << "<h1>Contr�le du mat�riel d'affichage</h1>";

			stream << l.open();

			stream << l.element() << "Etat : ";
			if (_displayScreen->getComplements().status == DISPLAY_STATUS_NO_NEWS_WARNING
				|| _displayScreen->getComplements().status == DISPLAY_STATUS_HARDWARE_WARNING)
				stream << HTMLModule::getHTMLImage(IMG_URL_WARNING, _displayScreen->getComplements().statusText);
			if (_displayScreen->getComplements().status == DISPLAY_STATUS_NO_NEWS_ERROR
				|| _displayScreen->getComplements().status == DISPLAY_STATUS_HARDWARE_ERROR)
				stream << HTMLModule::getHTMLImage(IMG_URL_ERROR, _displayScreen->getComplements().statusText);
			if (_displayScreen->getComplements().status == DISPLAY_STATUS_OK)
				stream << HTMLModule::getHTMLImage(IMG_URL_INFO, "OK");

			stream << l.element() << "D�tail : ";
			stream << _displayScreen->getComplements().statusText;

			stream << l.element() << "Date du dernier contr�le : ";
			stream << _displayScreen->getComplements().lastControl.toString();

			stream << l.element() << "Date du dernier contr�le positif : ";
			stream << _displayScreen->getComplements().lastOKStatus.toString();

			stream << l.element("log") << HTMLModule::getHTMLLink(goToLogRequest.getURL(), "Acc�der au journal de maintenance de l'afficheur");

			stream << l.close();
		}

		bool DisplayMaintenanceAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}

		DisplayMaintenanceAdmin::DisplayMaintenanceAdmin()
			: AdminInterfaceElementTemplate<DisplayMaintenanceAdmin>()
		{
	
		}

		AdminInterfaceElement::PageLinks DisplayMaintenanceAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			return AdminInterfaceElement::PageLinks();
		}

		AdminInterfaceElement::PageLinks DisplayMaintenanceAdmin::getSubPages( const AdminInterfaceElement& currentPage, const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return AdminInterfaceElement::PageLinks();
		}

		boost::shared_ptr<const DisplayScreen> DisplayMaintenanceAdmin::getDisplayScreen() const
		{
			return _displayScreen;
		}

		std::string DisplayMaintenanceAdmin::getParameterName() const
		{
			return _displayScreen.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string DisplayMaintenanceAdmin::getParameterValue() const
		{
			return _displayScreen.get() ? Conversion::ToString(_displayScreen->getKey()) : string();
		}

		bool DisplayMaintenanceAdmin::isPageVisibleInTree(const AdminInterfaceElement& currentPage) const
		{
			return currentPage.getFactoryKey() == DisplayAdmin::FACTORY_KEY
				&& currentPage.getPageLink().parameterValue == getPageLink().parameterValue;
		}
	}
}
