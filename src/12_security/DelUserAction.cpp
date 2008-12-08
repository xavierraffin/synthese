
/** DelUserAction class implementation.
	@file DelUserAction.cpp

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

#include "DelUserAction.h"

#include "12_security/UserTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, security::DelUserAction>::FACTORY_KEY("sdu");
	
	namespace security
	{
		ParametersMap DelUserAction::getParametersMap() const
		{
			return ParametersMap();
		}

		void DelUserAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_user = UserTableSync::Get(_request->getObjectId());
			}
			catch (ObjectNotFoundException<User>& e)
			{
				throw ActionException("Specified user not found" + e.getMessage());
			}

		}

		void DelUserAction::run()
		{
			UserTableSync::Remove(_user->getKey());
		}
	}
}
