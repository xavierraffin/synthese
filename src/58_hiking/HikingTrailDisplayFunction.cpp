
/** HikingTrailDisplayFunction class implementation.
	@file HikingTrailDisplayFunction.cpp
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

#include "RequestException.h"
#include "Request.h"
#include "HikingRight.h"
#include "HikingTrailDisplayFunction.h"
#include "HikingTrail.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace transportwebsite;

	template<> const string util::FactorableTemplate<hiking::HikingTrailDisplayFunction::_FunctionWithSite,hiking::HikingTrailDisplayFunction>::FACTORY_KEY("HikingTrailDisplayFunction");
	
	namespace hiking
	{
		/// @todo Parameter names declarations
		//const string HikingTrailDisplayFunction::PARAMETER_PAGE("rub");
		
		ParametersMap HikingTrailDisplayFunction::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSiteBase::_getParametersMap());
			if(_hikingTrail.get())
			{
				map.insert(Request::PARAMETER_OBJECT_ID, _hikingTrail->getKey());
			}
			return map;
		}

		void HikingTrailDisplayFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_FunctionWithSite::_setFromParametersMap(map);
			try
			{
				_hikingTrail = Env::GetOfficialEnv().get<HikingTrail>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<HikingTrail>)
			{
				throw RequestException("No such hiking trail");
			}
		}

		void HikingTrailDisplayFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			/// @todo See the hiking trail through the interface
		}
		
		
		
		bool HikingTrailDisplayFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string HikingTrailDisplayFunction::getOutputMimeType() const
		{
			return "text/html";
		}
	}
}
