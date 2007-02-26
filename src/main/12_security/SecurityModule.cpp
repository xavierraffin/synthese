
/** SecurityModule class implementation.
	@file SecurityModule.cpp

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

#include <vector>

#include "01_util/Factory.h"
#include "01_util/Constants.h"

#include "12_security/ProfileTableSync.h"
#include "12_security/UserTableSync.h"
#include "12_security/User.h"
#include "12_security/Profile.h"
#include "12_security/SecurityModule.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace security
	{
		const std::string SecurityModule::ROOT_PROFILE = "root";
		const std::string SecurityModule::ROOT_RIGHTS = "*,*,100,100";
		const std::string SecurityModule::ROOT_USER = "root";

		Profile::Registry SecurityModule::_profiles;

		void SecurityModule::initialize()
		{
			Profile* rootProfile;
			vector<Profile*> profiles = ProfileTableSync::search(ROOT_PROFILE);
			if (profiles.size() == 0)
				rootProfile = new Profile;
			else
				rootProfile = profiles.front();
	
			rootProfile->setName(ROOT_PROFILE);
			rootProfile->setPublicRight(Right::DELETE);
			rootProfile->setPrivateRight(Right::DELETE);
			ProfileTableSync::save(rootProfile);

			vector<User*> users = UserTableSync::search(ROOT_USER, ROOT_USER, rootProfile->getKey());
			if (users.size() == 0)
			{
				User* rootUser = new User;
				rootUser->setName(ROOT_USER);
				rootUser->setLogin(ROOT_USER);
				rootUser->setPassword(ROOT_USER);
				rootUser->setProfile(rootProfile);
				rootUser->setConnectionAllowed(true);
				UserTableSync::save(rootUser);
			}
		}

		Profile::Registry& SecurityModule::getProfiles()
		{
			return _profiles;
		}

		map<string, string> SecurityModule::getRightsTemplates()
		{
			map<string, string> m;
			for (Factory<Right>::Iterator it = Factory<Right>::begin(); it != Factory<Right>::end(); ++it)
				m.insert(make_pair(it.getKey(), it.getKey()));
			return m;
		}

		std::map<uid, std::string> SecurityModule::getProfileLabels(bool withAll, int first/*=0*/, int last/*=-1*/ )
		{
			map<uid, string> m;
			if (withAll)
				m.insert(make_pair(UNKNOWN_VALUE, "(tous)"));
			for (Profile::Registry::const_iterator it = _profiles.begin(); it != _profiles.end(); ++it)
				m.insert(make_pair(it->first, it->second->getName()));
			return m;
		}

		std::map<uid, std::string> SecurityModule::getUserLabels(bool withAll, int first/*=0*/, int last/*=-1*/ )
		{
			map<uid, string> m;
			if (withAll)
				m.insert(make_pair(uid(UNKNOWN_VALUE), "(tous)"));
			vector<User*> users = UserTableSync::search("","");
			for (vector<User*>::iterator it = users.begin(); it != users.end(); ++it)
			{
				m.insert(make_pair((*it)->getKey(), (*it)->getSurname() + " " + (*it)->getName()));
				delete *it;
			}
			return m;
		}

		std::map<std::string, std::string> SecurityModule::getRightLabels( bool withAll/*=false*/ )
		{
			map<string, string> m;
			if (withAll)
				m.insert(make_pair("", "(toutes)"));
			for (Factory<Right>::Iterator it = Factory<Right>::begin(); it != Factory<Right>::end(); ++it)
				m.insert(make_pair(it.getKey(), it.getKey()));
			return m;
		}
	}
}
