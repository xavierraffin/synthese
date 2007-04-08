
/** SecurityRight class implementation.
	@file SecurityRight.cpp

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

#include "01_util/Conversion.h"

#include "12_security/SecurityRight.h"
#include "12_security/SecurityModule.h"
#include "12_security/Constants.h"
#include "12_security/Profile.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace security
	{

		std::string SecurityRight::displayParameter() const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return "(tous profils)";

			return (SecurityModule::getProfiles().contains(Conversion::ToLongLong(_parameter)))
				? SecurityModule::getProfiles().get(Conversion::ToLongLong(_parameter))->getName()
				: "(invalide)";
		}

		Right::ParameterLabelsVector SecurityRight::getParametersLabels() const
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER, "(tous profils)"));
			_addSubProfilesLabel(m, NULL, string());
			return m;
		}

		bool SecurityRight::perimeterIncludes( const std::string& perimeter ) const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return true;
			if (perimeter == UNKNOWN_PERIMETER)
				return true;

			if (SecurityModule::getProfiles().contains(Conversion::ToLongLong(_parameter))
				&& SecurityModule::getProfiles().contains(Conversion::ToLongLong(perimeter)))
			{
				Profile* includedProfile = SecurityModule::getProfiles().get(Conversion::ToLongLong(perimeter));
				Profile* currentProfile = SecurityModule::getProfiles().get(Conversion::ToLongLong(_parameter));

				for (;includedProfile != NULL; includedProfile = SecurityModule::getProfiles().get(includedProfile->getParentId()))
					if (currentProfile == includedProfile)
						return true;
			}

			return false;
		}

		void SecurityRight::_addSubProfilesLabel( ParameterLabelsVector& plv, Profile* parent, std::string prefix)
		{
			vector<Profile*> p = SecurityModule::getSubProfiles(parent);

			for (vector<Profile*>::const_iterator it = p.begin(); it != p.end(); ++it)
			{
				plv.push_back(make_pair(Conversion::ToString((*it)->getKey()), prefix + (*it)->getName()));

				_addSubProfilesLabel(plv, *it, "&nbsp;&nbsp;" + prefix);
			}
		}
	}
}
