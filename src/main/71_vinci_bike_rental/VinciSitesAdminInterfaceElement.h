
/** VinciSitesAdminInterfaceElement class header.
	@file VinciSitesAdminInterfaceElement.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_VinciSitesAdminInterfaceElement_H__
#define SYNTHESE_VinciSitesAdminInterfaceElement_H__

#include "05_html/ActionResultHTMLTable.h"

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace vinci
	{
		class VinciSite;

		/** VinciSitesAdminInterfaceElement Class.
			@ingroup m71Admin refAdmin
			@author Hugues Romain
			@date 2007
		*/
		class VinciSitesAdminInterfaceElement : public admin::AdminInterfaceElement
		{
			std::vector<boost::shared_ptr<VinciSite> >		_sites;
			html::ActionResultHTMLTable::RequestParameters	_requestParameters;
			html::ActionResultHTMLTable::ResultParameters	_resultParameters;

		public:
			VinciSitesAdminInterfaceElement();
			
			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2007
			*/
			void setFromParametersMap(const server::ParametersMap& map);

			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@param request The current request
				@author Hugues Romain
				@date 2007
			*/
			void display(std::ostream& stream, const server::FunctionRequest<admin::AdminRequest>* request=NULL) const;
			
			/** Title of the admin compound.
				@return The title of the admin compound, for display purposes.
				@author Hugues Romain
				@date 2007
			*/
			std::string getTitle() const;
			
			/** Authorization control.
				@param request The current request
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2007
			*/
			bool isAuthorized(const server::FunctionRequest<admin::AdminRequest>* request) const;
			
		};
	}
}

#endif // SYNTHESE_VinciSitesAdminInterfaceElement_H__
