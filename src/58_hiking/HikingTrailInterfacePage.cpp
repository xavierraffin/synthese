
/** HikingTrailInterfacePage class implementation.
	@file HikingTrailInterfacePage.cpp
	@author Hugues
	@date 2010

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

#include "HikingTrailInterfacePage.h"
#include "HikingTrail.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace hiking;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, hiking::HikingTrailInterfacePage>::FACTORY_KEY("hikingtrail");
	}

	namespace hiking
	{
		HikingTrailInterfacePage::HikingTrailInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, HikingTrailInterfacePage>(),
			Registrable(0)
		{
		}
		
		

		void HikingTrailInterfacePage::display(
			std::ostream& stream,
			const HikingTrail& object,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			
			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&object)
				, request
			);
		}
	}
}
