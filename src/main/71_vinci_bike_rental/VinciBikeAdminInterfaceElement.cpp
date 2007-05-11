
/** VinciBikeAdminInterfaceElement class implementation.
	@file VinciBikeAdminInterfaceElement.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Rapha�l Murat - Vinci Park <rmurat@vincipark.com>

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
#include "71_vinci_bike_rental/VinciBikeSearchAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciBikeAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciUpdateBikeAction.h"
#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"

#include "05_html/HTMLTable.h"
#include "05_html/HTMLForm.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/AdminParametersException.h"

#include <string>

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace util;
	using namespace html;
	using namespace db;
	using namespace vinci;

	namespace util
	{
		const string FactorableTemplate<AdminInterfaceElement, VinciBikeAdminInterfaceElement>::FACTORY_KEY("vincibike");
	}

	namespace admin
	{
		const string AdminInterfaceElementTemplate<VinciBikeAdminInterfaceElement>::ICON("tag_blue.png");
		const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<VinciBikeAdminInterfaceElement>::DISPLAY_MODE(AdminInterfaceElement::DISPLAYED_IF_CURRENT);
		string AdminInterfaceElementTemplate<VinciBikeAdminInterfaceElement>::getSuperior()
		{
			return VinciBikeSearchAdminInterfaceElement::FACTORY_KEY;
		}
	}

	namespace vinci
	{
		VinciBikeAdminInterfaceElement::VinciBikeAdminInterfaceElement()
			: AdminInterfaceElementTemplate<VinciBikeAdminInterfaceElement>() {}


		std::string VinciBikeAdminInterfaceElement::getTitle() const
		{
			return "V�lo " + _bike->getNumber();
		}

		void VinciBikeAdminInterfaceElement::display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request /*= NULL*/ ) const
		{
			// Update bike request
			ActionFunctionRequest<VinciUpdateBikeAction, AdminRequest> updateBikeRequest(request);
			updateBikeRequest.getFunction()->setPage<VinciBikeAdminInterfaceElement>();
			
			// Display of data board
			stream << "<h1>Donn�es</h1>";
			HTMLForm form(updateBikeRequest.getHTMLForm("update"));
			form.addHiddenField(VinciUpdateBikeAction::PARAMETER_BIKE_ID, Conversion::ToString(_bike->getKey()));
			stream << form.open();
			HTMLTable t;
			stream << t.open();
			stream << t.row();
			stream << t.col() << "Num�ro";
			stream << t.col() << form.getTextInput(VinciUpdateBikeAction::PARAMETER_NUMBER, _bike->getNumber());
			stream << t.row();
			stream << t.col() << "Cadre";
			stream << t.col() << form.getTextInput(VinciUpdateBikeAction::PARAMETER_MARKED_NUMBER, _bike->getMarkedNumber());
			stream << t.row();
			stream << t.col(2) << form.getSubmitButton("Enregistrer");
			stream << t.close();
			stream << form.close();

			// Display of history
			stream << "<h1>Historique</h1>";
		}

		void VinciBikeAdminInterfaceElement::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
				if (it != map.end())
				{
					if (Conversion::ToLongLong(it->second) == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)
						return;
					_bike = VinciBikeTableSync::get(Conversion::ToLongLong(it->second));
				}
			}
			catch(DBEmptyResultException<VinciBike>)
			{
				throw AdminParametersException("Specified bike not found");
			}
		}

		bool VinciBikeAdminInterfaceElement::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return true;
		}
	}
}
