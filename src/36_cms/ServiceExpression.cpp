
/** ServiceExpression class implementation.
	@file ServiceExpression.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "ServiceExpression.hpp"

#include "FunctionWithSiteBase.hpp"
#include "ParametersMap.h"
#include "Request.h"
#include "RequestException.h"

#include <sstream>
#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;

	namespace cms
	{
		const string ServiceExpression::PARAMETER_VAR = "VAR";
		const string ServiceExpression::PARAMETER_TEMPLATE = "template";



		std::string ServiceExpression::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{
			stringstream s;
			display(s, request, additionalParametersMap, page, variables);
			return s.str();
		}



		void ServiceExpression::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			if(!_functionCreator)
			{
				return;
			}

			// Service parameters evaluation
			ParametersMap serviceParametersMap;
			BOOST_FOREACH(const Parameters::value_type& param, _serviceParameters)
			{
				serviceParametersMap.insert(
					param.first,
					param.second.eval(request, additionalParametersMap, page, variables)
				);
			}
			if(!_inlineTemplate.empty())
			{
				serviceParametersMap.insert(Function::PARAMETER_OUTPUT_FORMAT, string());
			}

			// Template parameters evaluation
			ParametersMap templateParametersMap(request.getFunction()->getTemplateParameters());
			BOOST_FOREACH(const Parameters::value_type& param, _templateParameters)
			{
				templateParametersMap.insert(
					param.first,
					param.second.eval(request, additionalParametersMap, page, variables)
				);
			}

			// Function
			shared_ptr<Function> function(_functionCreator->create());
			try
			{
				if(dynamic_cast<FunctionWithSiteBase*>(function.get()))
				{
					static_cast<FunctionWithSiteBase*>(function.get())->setSite(page.getRoot());
				}
				function->setTemplateParameters(templateParametersMap);
				function->_setFromParametersMap(serviceParametersMap);
				if (function->isAuthorized(request.getSession()))
				{
					// Run of the service
					ParametersMap result(
						function->run(
							stream,
							request
					)	);

					// Display of the result if inline template defined
					if(!_inlineTemplate.empty())
					{
						// Merge page parameters in result map
						result.merge(additionalParametersMap);

						// Display of each inline defined node
						_inlineTemplate.display(stream, request, result, page, variables);
					}
				}
			}
			catch(RequestException&)
			{

			}
			catch(Request::RedirectException& e)
			{
				throw e;
			}
			catch(...)
			{

			}
		}



		ServiceExpression::ServiceExpression(
			std::string::const_iterator& it,
			std::string::const_iterator end
		):	_functionCreator(NULL)
		{
			// function name
			string functionName;
			for(;it != end && *it != '&' && *it != '?'; ++it)
			{
				functionName.push_back(*it);
			}
			try
			{
				_functionCreator = Factory<Function>::GetCreator(ParametersMap::Trim(functionName));

				// parameters
				if(it != end && *it == '?')
				{
					it += 2;
				}
				else
				{
					set<string> functionTermination;
					functionTermination.insert("&");
					functionTermination.insert("?>");
					while(it != end && *it == '&')
					{
						stringstream parameterName;
						++it;
						ParseText(parameterName, it, end, "=");

						if(it != end)
						{
							// Parsing of the nodes
							WebpageContent parameterNodes(it, end, functionTermination);

							// Special template parameter
							if(parameterName.str() == PARAMETER_TEMPLATE)
							{
								_inlineTemplate = parameterNodes;
							}
							else
							{
								// Storage in template parameters if begins with VAR else in service parameters
								string parameterNameStr(ParametersMap::Trim(parameterName.str()));
								if(parameterNameStr.size() < PARAMETER_VAR.size() || parameterNameStr.substr(0, PARAMETER_VAR.size()) != PARAMETER_VAR)
								{
									_serviceParameters.push_back(make_pair(parameterNameStr, parameterNodes));
								}
								else
								{
									_templateParameters.push_back(make_pair(parameterNameStr.substr(PARAMETER_VAR.size()), parameterNodes));
								}
							}

							if(*(it-1) != '&')
							{
								break;
							}
							--it;
						}
				}	}
			}
			catch(FactoryException<Function>&)
			{
				for(; it != end && it+1 != end && *it != '?' && *(it+1) != '>' ; ++it) ;
				if(it != end && *it == '?')
				{
					++it;
					if(it != end && *it == '>')
					{
						++it;
				}	}
			}
		}
}	}