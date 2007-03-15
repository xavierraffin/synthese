
/** SearchFormHTMLTable class implementation.
	@file SearchFormHTMLTable.cpp

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

#include "01_util/Html.h"

#include "SearchFormHTMLTable.h"
#include "AdminRequest.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace admin
	{

		SearchFormHTMLTable::SearchFormHTMLTable( const AdminRequest* request )
			: HtmlTable(6, "searchform")
			, _request(request)
		{

		}

		std::string SearchFormHTMLTable::open()
		{
			stringstream s;
			s	<< _request->getHTMLFormHeader("search")
				<< HtmlTable::open();
			return s.str();
		}

		std::string SearchFormHTMLTable::close()
		{
			stringstream s;
			s	<< goCol(4, 2, "submitcell")
				<< Html::getSubmitButton("Rechercher")
				<< HtmlTable::close()
				<< "</form>";
			return s.str();
		}

		std::string SearchFormHTMLTable::cell( const std::string& label, const std::string& content, std::string id/*=""*/ )
		{
			stringstream s;
			s	<< col();
			if (!id.empty())
				s << "<label for=\"" << id << "\">";
			s	<< label;
			if (!id.empty())
				s << "</label>";
			s	<< col()
				<< content;
			return s.str();
		}
	}
}
