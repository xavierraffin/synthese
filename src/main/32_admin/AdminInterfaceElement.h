
/** AdminInterfaceElement class header.
	@file AdminInterfaceElement.h

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

#ifndef SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H
#define SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H

#include <string>

#include "01_util/Factorable.h"

namespace synthese
{
	namespace admin
	{
		/** Composant d'administration.
			@ingroup m32
			
			Un composant d'administration est un formulaire param�trable, destin� � effectuer une �criture en temps r�el dans la base SYNTHESE, dans un but de param�trage de l'application (il ne s'agit pas du seul moyen d'�criture).
			Une m�me donn�e peut �tre mise � jour par autant de composants que n�cessaire, d�fini avant tout sur la base d'une ergonomie de qualit�.
			Chaque composant est rattach� au module correspondant aux donn�es � modifier sous forme d'une sous-classe de AdminInterfaceElement. 
			Le comportement des composants d'administration est en g�n�ral d�fini d'apr�s les @ref defRight "habilitations" de l'utilisateur connect�.
			Par exemple :
				- un utilisateur anonyme n'a acc�s � aucun composant d'administration
				- un administrateur a acc�s � tous les composants d'administration.
			
		*/
		class AdminInterfaceElement : public util::Factorable
		{
		public:
			typedef enum { EVER_DISPLAYED, DISPLAYED_IF_CURRENT, NEVER_DISPLAYED } DisplayMode;

		private:
			const std::string _superior;
			const DisplayMode _everDisplayed;

		public:
			/** Constructor.
			*/
			AdminInterfaceElement(const std::string& superior, DisplayMode everDisplayed);

			const std::string& getSuperior() const;
			const DisplayMode getDisplayMode() const;
			virtual std::string getTitle() const = 0;

			std::string getHTMLLink(const server::Request* request) const;

			//static const Right* requiredRight();

		};
	}
}

#endif

