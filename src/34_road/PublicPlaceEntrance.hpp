
/** PublicPlaceEntrance class header.
	@file PublicPlaceEntrance.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_road_PublicPlaceEntrance_hpp__
#define SYNTHESE_road_PublicPlaceEntrance_hpp__

#include "Address.h"
#include "AddressField.hpp"
#include "DataSourceLinksField.hpp"
#include "NumericField.hpp"
#include "Object.hpp"
#include "PublicPlace.h"
#include "StringField.hpp"

#include "ImportableTemplate.hpp"
#include "Registrable.h"

#include "Registry.h"

namespace synthese
{
	namespace road
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(PublicPlace),
			FIELD(Name),
			FIELD(AddressField),
			FIELD(impex::DataSourceLinks)
		> PublicPlaceEntranceSchema;



		/** Public place entrance.
			@ingroup m34
		*/
		class PublicPlaceEntrance:
			public Object<PublicPlaceEntrance, PublicPlaceEntranceSchema>,
			public Address,
			public impex::ImportableTemplate<PublicPlaceEntrance>
		{
		public:
			typedef util::Registry<PublicPlaceEntrance> Registry;

			PublicPlaceEntrance(
				util::RegistryKeyType id = 0
			);

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			virtual void unlink();
		};
}	}

#endif // SYNTHESE_road_PublicPlaceEntrance_hpp__
