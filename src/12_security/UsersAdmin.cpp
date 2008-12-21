
/** UsersAdmin class implementation.
	@file UsersAdmin.cpp

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

#include "SearchFormHTMLTable.h"
#include "ActionResultHTMLTable.h"
#include "HTMLModule.h"

#include "InterfaceModule.h"

#include "SecurityModule.h"
#include "UserAdmin.h"
#include "AddUserAction.h"
#include "DelUserAction.h"
#include "ProfileTableSync.h"
#include "UserTableSync.h"
#include "UsersAdmin.h"
#include "SecurityRight.h"

#include "Session.h"
#include "ServerModule.h"
#include "ActionFunctionRequest.h"

#include "AdminModule.h"
#include "ModuleAdmin.h"
#include "AdminRequest.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace interfaces;
	using namespace admin;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,UsersAdmin>::FACTORY_KEY("users");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<UsersAdmin>::ICON("user.png");
		template<> const string AdminInterfaceElementTemplate<UsersAdmin>::DEFAULT_TITLE("Utilisateurs");
	}
	
	namespace security
	{
		const std::string UsersAdmin::PARAM_SEARCH_PROFILE_ID = "sp";
		const std::string UsersAdmin::PARAM_SEARCH_NAME = "sn";
		const std::string UsersAdmin::PARAM_SEARCH_LOGIN = "sl";
		const std::string UsersAdmin::PARAM_SEARCH_SURNAME = "ss";


		void UsersAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_searchLogin = map.getString(PARAM_SEARCH_LOGIN, false, FACTORY_KEY);
			_searchName = map.getString(PARAM_SEARCH_NAME, false, FACTORY_KEY);
			_searchSurname = map.getString(PARAM_SEARCH_SURNAME, false, FACTORY_KEY);

			// Searched profile
			uid id(map.getUid(PARAM_SEARCH_PROFILE_ID, false, FACTORY_KEY));
			if (id != UNKNOWN_VALUE && Env::GetOfficialEnv()->getRegistry<Profile>().contains(id))
				_searchProfile = ProfileTableSync::Get(id);

			// Table Parameters
			_requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_LOGIN, 30);
		}

		bool UsersAdmin::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return request->isAuthorized<SecurityRight>(READ);
		}

		void UsersAdmin::display( std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			// Search
			Env env;
			UserTableSync::Search(
				env,
				"%"+_searchLogin+"%"
				, "%"+_searchName+"%"
				, "%"+_searchSurname+"%"
				, "%"
				, _searchProfile.get() ? _searchProfile->getKey() : UNKNOWN_VALUE
				, false
				, _requestParameters.first
				, _requestParameters.maxSize
				, _requestParameters.orderField == PARAM_SEARCH_LOGIN
				, _requestParameters.orderField == PARAM_SEARCH_NAME
				, _requestParameters.orderField == PARAM_SEARCH_PROFILE_ID
				, _requestParameters.raisingOrder,
				UP_LINKS_LOAD_LEVEL
			);
			ResultHTMLTable::ResultParameters	_resultParameters;
			_resultParameters.setFromResult(_requestParameters, env.getEditableRegistry<User>());


			// Request for search form
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<UsersAdmin>();
			SearchFormHTMLTable searchTable(searchRequest.getHTMLForm("search"));
			
			// Request for add user action form
			ActionFunctionRequest<AddUserAction, AdminRequest> addUserRequest(request);
			addUserRequest.getFunction()->setPage<UserAdmin>();
			addUserRequest.getFunction()->setActionFailedPage<UsersAdmin>();
			
			// Request for delete action form
			ActionFunctionRequest<DelUserAction, AdminRequest> deleteUserRequest(request);
			deleteUserRequest.getFunction()->setPage<UsersAdmin>();
			
			// Request for user link
			FunctionRequest<AdminRequest> userRequest(request);
			userRequest.getFunction()->setPage<UserAdmin>();

			// Search form
			stream << "<h1>Recherche d'utilisateur</h1>";
				
			stream << searchTable.open();
			stream << searchTable.cell("Login", searchTable.getForm().getTextInput(PARAM_SEARCH_LOGIN, _searchLogin));
			stream << searchTable.cell("Nom", searchTable.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName));
			stream << searchTable.cell("Pr�nom", searchTable.getForm().getTextInput(PARAM_SEARCH_SURNAME, _searchSurname));
			stream << searchTable.cell("Profil", searchTable.getForm().getSelectInput(AddUserAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(true), _searchProfile.get() ? _searchProfile->getKey() : uid(0)));
			stream << searchTable.close();

			stream << "<h1>R�sultats de la recherche</h1>";

			if (env.getRegistry<User>().empty())
				stream << "Aucun utilisateur trouv�";


			ActionResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAM_SEARCH_LOGIN, "Login"));
			v.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			v.push_back(make_pair(PARAM_SEARCH_PROFILE_ID, "Profil"));
			v.push_back(make_pair(string(), "Actions"));
			ActionResultHTMLTable t(v, searchTable.getForm(), _requestParameters, _resultParameters, addUserRequest.getHTMLForm("add"),"", InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));

			stream << t.open();

			BOOST_FOREACH(shared_ptr<User> user, env.getRegistry<User>())
			{
				userRequest.setObjectId(user->getKey());
				deleteUserRequest.setObjectId(user->getKey());
				stream << t.row();
				stream << t.col() << HTMLModule::getHTMLLink(userRequest.getURL(), user->getLogin());
				stream << t.col() << HTMLModule::getHTMLLink(userRequest.getURL(), user->getName());
				stream << t.col() << user->getProfile()->getName();
				stream << t.col() << userRequest.getHTMLForm().getLinkButton("Modifier") << "&nbsp;"
					<< deleteUserRequest.getHTMLForm().getLinkButton("Supprimer", "Etes-vous s�r(e) de vouloir supprimer l\\'utilisateur " + user->getLogin() + " ?");
			}

			stream << t.row();
			stream << t.col() << t.getActionForm().getTextInput(AddUserAction::PARAMETER_LOGIN, "", "Entrez le login ici");
			stream << t.col() << t.getActionForm().getTextInput(AddUserAction::PARAMETER_NAME, "", "Entrez le nom ici");
			stream << t.col() << t.getActionForm().getSelectInput(AddUserAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(), uid(0));
			stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.close();
		}

		UsersAdmin::UsersAdmin()
			: AdminInterfaceElementTemplate<UsersAdmin>()
		{
		
		}

		AdminInterfaceElement::PageLinks UsersAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage 		, const server::FunctionRequest<admin::AdminRequest>* request
			) const
		{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == SecurityModule::FACTORY_KEY)
			{
				links.push_back(getPageLink());
			}
			return links;
		}
	}
}
