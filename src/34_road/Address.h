
/** Address class header.
	@file Address.h

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

#ifndef SYNTHESE_ENV_ADDRESS_H
#define SYNTHESE_ENV_ADDRESS_H

#include "Vertex.h"
#include "Registry.h"
#include "01_util/Constants.h"
#include "Importable.h"

#include <vector>
#include <set>

namespace synthese
{
	namespace road
	{
		class AddressablePlace;
		
		//////////////////////////////////////////////////////////////////////////
		/// Address.
		/// @ingroup m34
		/// @author Marc Jambert, Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// An address corresponds to a point on a side of a street.
		/// @image html uml_address.png
		///
		/// The address is defined by a RoadChunk and a metric offset.
		class Address
		:	public graph::Vertex,
			public impex::Importable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<Address> Registry;

		private:


		public:

			Address(
				util::RegistryKeyType id = 0,
				const AddressablePlace* place = NULL,
				double x = UNKNOWN_VALUE,
				double y = UNKNOWN_VALUE
			);

			~Address();

			using GeoPoint::operator=;

			//! @name Getters
			//@{
			//@}

			//! @name Setters
			//@{
			//@}

			//! @name Query methods
			//@{
				virtual graph::GraphIdType getGraphType() const;
			//@}
		};
	}
}

#endif
