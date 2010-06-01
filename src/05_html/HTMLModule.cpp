
/** HTMLModule class implementation.
	@file HTMLModule.cpp

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

#include "HTMLModule.h"

#include <map>
#include <sstream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	namespace html
	{

		std::string HTMLModule::getLinkButton(
			const std::string& url,
			const std::string& caption,
			const string confirm,
			const string icon,
			bool useOnclick
		){
			stringstream s;
			s	<< "<a class=\"linkbutton\" "
				<< "onmouseover=\"this.className='activatedlinkbutton';\" "
				<< "onmouseout=\"this.className='linkbutton';\" "
				<< "onmousedown=\"this.className='clickedlinkbutton';\" "
				<< "onmouseup=\"this.className='activatedlinkbutton';\" ";
			if (confirm.empty())
			{
				if (useOnclick)
				{
					s << "href=\"#\" onclick=\"" << url << " return false;\"";
				}
				else
				{
					s << "href=\"" << url << "\"";
				}
			}
			else
			{
				string escapedConfirm(confirm);
				algorithm::replace_all(escapedConfirm, "'", "\\'");
				algorithm::replace_all(escapedConfirm, "\"", "&quot;");

				s << "href=\"#\" onclick=\"if (window.confirm('" << escapedConfirm << "')) ";
				if (useOnclick)
				{
					s << url;
				}
				else
				{
					s << "window.location='" << url << "';";
				}
				s << "return false;\"";
			}
			s	<< ">";
			if (!icon.empty())
				s << getHTMLImage(icon, caption) << "&nbsp;";
			s << caption << "</a>";
			return s.str();
		}

		std::string HTMLModule::getHTMLLink(const string& url, const std::string& content )
		{
			std::stringstream str;
			str << "<a href=\"" << url << "\">"
				<< content << "</a>";
			return str.str();
		}

		std::string HTMLModule::getHTMLImage( const std::string& url, const std::string& title )
		{
			stringstream s;
			s << "<img src=\"" << url << "\" alt=\"" << title << "\" title=\"" << title << "\" />";
			return s.str();
		}

		std::string HTMLModule::GetHTMLJavascriptOpen( std::string url/*=std::string()*/ )
		{
			stringstream s;
			s << "<script type=\"text/javascript\"";
			if (!url.empty())
				s << " src=\"" << url << "\"";
			s << ">";
			if (url.empty())
				s << "\r// <![CDATA[\r";
			else
				s << "</script>";
			return s.str();
		}

		std::string HTMLModule::GetHTMLJavascriptClose()
		{
			return string("\r// ]]>\r</script>");
		}



		std::string HTMLModule::HTMLEncode( const std::string& value )
		{
			static map<char,string> encodingTable;
			if(encodingTable.empty())
			{
				encodingTable['&'] = "&amp;";
				encodingTable['>'] = "&gt;";
				encodingTable['<'] = "&lt;";
				encodingTable['"'] = "&quot;";
			}
			stringstream s;
			BOOST_FOREACH(char c, value)
			{
				if(c >= 128)
				{
					s << "&#" << static_cast<int>(c) << ";";
					continue;
				}
				map<char,string>::const_iterator it(encodingTable.find(c));
				if(it != encodingTable.end())
				{
					s << it->second;
				}
				else
				{
					s << c;
				}
			}
			return s.str();
		}



		std::string HTMLModule::EscapeDoubleQuotes( const std::string value )
		{
			stringstream s;
			BOOST_FOREACH(char c, value)
			{
				if(c == '"')
				{
					s << "\\\"";
				}
				else
				{
					s << c;
				}
			}
			return s.str();
		}
	}
}
