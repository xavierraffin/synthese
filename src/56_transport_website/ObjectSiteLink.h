
/** ObjectSiteLink class header.
	@file ObjectSiteLink.h

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

#ifndef SYNTHESE_transportwebsite_ObjectSiteLink_h__
#define SYNTHESE_transportwebsite_ObjectSiteLink_h__

#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace transportwebsite
	{
		class TransportWebsite;

		/** ObjectSiteLink class.
			@ingroup m56
		*/
		class ObjectSiteLink
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<ObjectSiteLink>	Registry;

		protected:
			util::RegistryKeyType			_objectId;
			const TransportWebsite*	_site;

		public:
			ObjectSiteLink(util::RegistryKeyType id = 0);

			util::RegistryKeyType			getObjectId()	const { return _objectId; }
			const TransportWebsite*	getSite()		const { return _site; }

			void setObjectId(util::RegistryKeyType value) { _objectId = value; }
			void setSite(const TransportWebsite* site) { _site = site; }
		};
	}
}

#endif // SYNTHESE_transportwebsite_ObjectSiteLink_h__
