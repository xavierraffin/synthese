
/** City class implementation.
	@file City.cpp

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

#include "City.h"
#include "PlaceAlias.h"
#include "PublicPlace.h"
#include "Road.h"
#include "Registry.h"
#include "PublicTransportStopZoneConnectionPlace.h"

#include <assert.h>

using namespace std;

namespace synthese
{
	using namespace lexmatcher;
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<env::City>::KEY("City");
	}

	namespace env
	{

		City::City(
			RegistryKeyType key,
			std::string name,
			std::string code
		):	IncludingPlace(key, name, 0),  // Note this city's city is null ?
			_code (code),
			Registrable(key)
		{
		}



		City::~City ()
		{

		}




		LexicalMatcher<const PublicTransportStopZoneConnectionPlace*>& City::getConnectionPlacesMatcher(
		){
			return _connectionPlacesMatcher;
		}




		const LexicalMatcher<const PublicTransportStopZoneConnectionPlace*>& City::getConnectionPlacesMatcher(
		) const {
			return _connectionPlacesMatcher;
		}




		synthese::lexmatcher::LexicalMatcher<const PublicPlace*>& 
		City::getPublicPlacesMatcher ()
		{
			return _publicPlacesMatcher;
		}




		const synthese::lexmatcher::LexicalMatcher<const PublicPlace*>& 
		City::getPublicPlacesMatcher () const
		{
			return _publicPlacesMatcher;
		}






		synthese::lexmatcher::LexicalMatcher<const Road*>& 
		City::getRoadsMatcher ()
		{
			return _roadsMatcher;
		}




		const synthese::lexmatcher::LexicalMatcher<const Road*>& 
		City::getRoadsMatcher () const
		{
			return _roadsMatcher;
		}




		synthese::lexmatcher::LexicalMatcher<const PlaceAlias*>& 
		City::getPlaceAliasesMatcher ()
		{
			return _placeAliasesMatcher;
		}




		const synthese::lexmatcher::LexicalMatcher<const PlaceAlias*>& 
		City::getPlaceAliasesMatcher () const
		{
			return _placeAliasesMatcher;
		}



		std::vector<const Road*> 
		City::searchRoad (const std::string& fuzzyName, int nbMatches) const
		{
			std::vector<const Road*> result;
			LexicalMatcher<const Road*>::MatchResult matches =  _roadsMatcher.bestMatches (fuzzyName, nbMatches);
			for (LexicalMatcher<const Road*>::MatchResult::iterator it = matches.begin ();
			 it != matches.end (); ++it)
			{
			result.push_back (it->value);
			}
			return result;
		}



		void City::getImmediateVertices(
			VertexAccessMap& result
			, const AccessDirection& accessDirection
			, const AccessParameters& accessParameters
			, SearchAddresses returnAddresses
			, SearchPhysicalStops returnPhysicalStops
			, const Vertex* origin
		) const {

			if (_includedPlaces.empty ())
			{
				if (_connectionPlacesMatcher.size () > 0)
				{
					_connectionPlacesMatcher.entries().begin()->second->getImmediateVertices (
						result, accessDirection, accessParameters, 
						returnAddresses, returnPhysicalStops
						, origin
					);
				}
				else if (_placeAliasesMatcher.size () > 0)
				{
					_placeAliasesMatcher.entries().begin()->second->getImmediateVertices(
						result, accessDirection, accessParameters
						, returnAddresses, returnPhysicalStops
						, origin
					);
				}
				else if (_publicPlacesMatcher.size () > 0)
				{
					_publicPlacesMatcher.entries().begin()->second->getImmediateVertices(
						result, accessDirection, accessParameters
						, returnAddresses, returnPhysicalStops
						, origin
					);
				}
				else if (_roadsMatcher.size () > 0)
				{
					_roadsMatcher.entries().begin()->second->getImmediateVertices(
						result, accessDirection, accessParameters
						, returnAddresses, returnPhysicalStops
						, origin
					);
				}
			}
			else
			{
				IncludingPlace::getImmediateVertices(
					result, accessDirection, accessParameters, 
					returnAddresses, returnPhysicalStops
					, origin
				);
			}
		}



		lexmatcher::LexicalMatcher<const Place*>& City::getAllPlacesMatcher()
		{
			return _allPlacesMatcher;
		}

		const lexmatcher::LexicalMatcher<const Place*>& City::getAllPlacesMatcher() const
		{
			return _allPlacesMatcher;
		}
	}
}
