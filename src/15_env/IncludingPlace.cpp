
/** IncludingPlace class implementation.
	@file IncludingPlace.cpp

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

#include "IncludingPlace.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace env
	{
		IncludingPlace::IncludingPlace(
			RegistryKeyType key,
			const string& name,
			const City* city
		):	Registrable(key),
			Place(name, city)			
		{
		}



		IncludingPlace::~IncludingPlace ()
		{
		}



		const IncludingPlace::IncludedPlaces& 
		IncludingPlace::getIncludedPlaces () const
		{
			return _includedPlaces;
		}
		    


		void 
		IncludingPlace::addIncludedPlace (const Place* place)
		{
			_isoBarycentreToUpdate = true;
			_includedPlaces.push_back (place);
		}



		void IncludingPlace::getImmediateVertices(
			VertexAccessMap& result
			, const AccessDirection& accessDirection
			, const AccessParameters& accessParameters
			, SearchAddresses returnAddresses
			, SearchPhysicalStops returnPhysicalStops
			, const Vertex* origin
		) const	{

			for(IncludedPlaces::const_iterator it(_includedPlaces.begin());
				it != _includedPlaces.end();
				++it
			){
				(*it)->getImmediateVertices(
					result, accessDirection, accessParameters
					, returnAddresses, returnPhysicalStops
					, origin
				);
			}
		}

		const geometry::Point2D& IncludingPlace::getPoint() const
		{
			if (_isoBarycentreToUpdate)
			{
				_isoBarycentre.clear();
				for (IncludedPlaces::const_iterator it(_includedPlaces.begin()); it != _includedPlaces.end(); ++it)
					_isoBarycentre.add((*it)->getPoint());
				_isoBarycentreToUpdate = false;
			}
			return _isoBarycentre;
		}

		bool IncludingPlace::includes( const Place* place ) const
		{
			if (place == this)
				return true;

			for (IncludedPlaces::const_iterator it(_includedPlaces.begin()); it != _includedPlaces.end(); ++it)
				if (*it == place)
					return true;
			return false;
		}
	}
}
