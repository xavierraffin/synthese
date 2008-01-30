
/** VinciSitesListInterfaceElement class header.
	@file VinciSitesListInterfaceElement.h
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

#ifndef SYNTHESE_VinciSitesListInterfaceElement_H__
#define SYNTHESE_VinciSitesListInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace vinci
	{
		/** VinciSitesListInterfaceElement Library Interface Element Class.
			@author Hugues Romain
			@date 2008
			@ingroup m71Library refLibrary

			@code vinci_sites_field @endcode

			Parameters :
				- 0 :
				- 1 :

			Object : 

		*/
		class VinciSitesListInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, VinciSitesListInterfaceElement>
		{
			// List of parameters to store
			//boost::shared_ptr<interfaces::LibraryInterfaceElement> _parameter1;
			//...

		public:
			/** Controls and store the internals parameters.
				@param vel Parameters list to read
			*/
			void storeParameters(interfaces::ValueElementList& vel);
			
			/* Displays the object.
				@param stream Stream to write on
				@param parameters Runtime parameters used in the internal parameters reading
				@param variables Runtime variables
				@param object Object to read at the display
				@param request Source request
			*/
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
				) const;
				
			~VinciSitesListInterfaceElement();
		};
	}
}

#endif // SYNTHESE_VinciSitesListInterfaceElement_H__
