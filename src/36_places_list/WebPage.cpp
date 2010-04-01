
/** WebPage class implementation.
	@file WebPage.cpp

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

#include "WebPage.h"
#include "ParametersMap.h"
#include "DynamicRequest.h"
#include "FunctionWithSite.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	namespace util
	{
		template<> const string Registry<transportwebsite::WebPage>::KEY("WebPage");
	}

	namespace transportwebsite
	{
		WebPage::WebPage( util::RegistryKeyType id /*= UNKNOWN_VALUE*/ ):
			Registrable(id),
			_startDate(posix_time::not_a_date_time),
			_endDate(posix_time::not_a_date_time)
		{
		}



		bool WebPage::mustBeDisplayed( boost::posix_time::ptime now /*= boost::posix_time::second_clock::local_time()*/ ) const
		{
			return
				(_startDate.is_not_a_date_time() || _startDate <= now) &&
				(_endDate.is_not_a_date_time() || _endDate >= now)
			;
		}



		std::string::const_iterator WebPage::_parse(
			std::ostream& stream,
			std::string::const_iterator it,
			std::string::const_iterator end,
			const server::Request& request
		) const {
			while(it != end)
			{
				// Special characters
				if(*it == '\\' && it+1 != end)
				{
					++it;
					if(*it == 'n')
					{
						stream << endl;
					}
					else if(*it == '\\')
					{
						stream << '\\';
					}
					else if(*it == '<' && it+1 != end && *(it+1)=='?')
					{
						++it;
						stream << "<?";
					}
					else if(*it == '?' && it+1 != end && *(it+1)=='>')
					{
						++it;
						stream << "?>";
					}
					++it;
				} // Call to a public function
				else if(*it == '<' && it+1 != end && *(it+1)=='?' && it+2 != end)
				{
					stringstream query;
					query << Request::PARAMETER_FUNCTION << Request::PARAMETER_ASSIGNMENT;
					it = _parse(query, it+2, end, request);

					ParametersMap parametersMap(query.str());
					ParametersMap requestParametersMap(
						dynamic_cast<const DynamicRequest*>(&request) ?
						dynamic_cast<const DynamicRequest&>(request).getParametersMap() :
						ParametersMap()
					);
					requestParametersMap.remove(Request::PARAMETER_FUNCTION);
					parametersMap.merge(requestParametersMap);
					if(getRoot())
					{
						parametersMap.insert(FunctionWithSiteBase::PARAMETER_SITE, getRoot()->getKey());
					}
					string functionName(parametersMap.getDefault<string>(Request::PARAMETER_FUNCTION));
					if(!functionName.empty() && Factory<Function>::contains(functionName))
					{
						shared_ptr<Function> _function(Factory<Function>::create(functionName));
						if(_function.get())
						{
							try
							{
								_function->_setFromParametersMap(parametersMap);
								if (_function->isAuthorized(request.getSession()))
								{
									_function->run(stream, request);
								}
							}
							catch(...)
							{

							}
						}

					}
				} // Reached the end of a recursion level
				else if(*it == '?' && it+1 != end && *(it+1)=='>')
				{
					return it+2;
				}
				else
				{
					stream << *it;
					++it;
				}
			}
			return it;
		}



		void WebPage::display( std::ostream& stream, const server::Request& request ) const
		{
			_parse(stream, _content.begin(), _content.end(), request);
		}
	}
}
