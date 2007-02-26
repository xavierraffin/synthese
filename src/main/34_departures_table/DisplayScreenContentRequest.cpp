
/** DisplayScreenContentRequest class implementation.
	@file DisplayScreenContentRequest.cpp

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

#include <sstream>

#include "01_util/Conversion.h"
#include "01_util/Html.h"

#include "30_server/RequestException.h"

#include "34_departures_table/DisplayScreenContentRequest.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DeparturesTableModule.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace time;

	namespace departurestable
	{
		const std::string DisplayScreenContentRequest::PARAMETER_DATE = "date";
		const std::string DisplayScreenContentRequest::PARAMETER_TB = "tb";

		DisplayScreenContentRequest::DisplayScreenContentRequest()
			: Request()
			, _screen(NULL)
		{}

		Request::ParametersMap DisplayScreenContentRequest::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_OBJECT_ID, Conversion::ToString(_screen->getKey())));
			map.insert(make_pair(PARAMETER_DATE, _date.toInternalString()));
			return map;
		}

		void DisplayScreenContentRequest::setFromParametersMap(const Request::ParametersMap& map)
		{
			uid screenId = 0;
			try
			{
				Request::ParametersMap::const_iterator it;
				
				// Screen
				if (getObjectId())
					screenId = getObjectId();
				else
				{
					it = map.find(PARAMETER_TB);
					if (it == map.end())
						throw RequestException("Display screen not specified");
					screenId = Conversion::ToLongLong(it->second);
				}
				_screen = DeparturesTableModule::getDisplayScreens().get(screenId);

				// Date
				it = map.find(PARAMETER_DATE);
				if (it != map.end())
					_date = DateTime::FromInternalString(it->second);
			}
			catch (DisplayScreen::RegistryKeyException e)
			{
				throw RequestException("Display screen " + Conversion::ToString(screenId) + " not found");
			}
		}

		void DisplayScreenContentRequest::run( std::ostream& stream ) const
		{
			_screen->display(stream, _date);
		}

		DisplayScreenContentRequest::~DisplayScreenContentRequest()
		{
		}

		std::string DisplayScreenContentRequest::getHTMLFormHeader( const std::string& name ) const
		{
			stringstream s;
			s << Request::getHTMLFormHeader(name);
			if (getObjectId())
				s << Html::getHiddenInput(PARAMETER_OBJECT_ID, Conversion::ToString(getObjectId()));
			return s.str();
		}
	}
}
