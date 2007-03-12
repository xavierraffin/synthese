
/** Html class header.
	@file Html.h

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

#ifndef SYNTHESE_Html_H__
#define SYNTHESE_Html_H__

#include <string>
#include <sstream>
#include <map>
#include <utility>

#include "01_util/Constants.h"

namespace synthese
{
	namespace util
	{
		class Html
		{
		public:
			//! \name Basis Input fields
			//@{
				
				
				/** External sorted selection list HTML input field (template)
					@param name Name of the field
					@param choices List of choices : a map sorted by elements of S (the sort value is not read), giving pairs "value => text"
					@param value Default selected value
					@return Selection list HTML input field
					@author Hugues Romain
					@date 2007					
				*/
				template<class S, class K, class T>
					static std::string getSortedSelectInput(const std::string& name, const std::map<S, std::pair<K, T> >& choices, const K& value);

				template<class K, class T>
				static std::string getSelectInput(const std::string& name, const std::map<K, T>& choices, const K& value);

				template<class K, class T>
				static std::string getRadioInput(const std::string& name, const std::map<K, T>& choices, const K& value);

				static std::string getTextInput(const std::string& name, const std::string& value, std::string displayTextBeforeTyping="");

				static std::string getPasswordInput(const std::string& name, const std::string& value);

				static std::string getHiddenInput(const std::string& name, const std::string& value);

				static std::string getTextAreaInput(const std::string& name, const std::string& value, int rows, int cols);

				static std::string getSubmitButton(const std::string& caption);

				static std::string getLinkButton(const std::string& url, const std::string& caption);

				static std::string getCheckBox(const std::string& name, const std::string& value, bool checked);
			//@}

			//! \name Advanced Input fields
			//@{
				static std::string getSelectNumberInput(const std::string& name, int mini, int maxi, int value=UNKNOWN_VALUE, int step=1);

				/** Oui Non radio input.
					@param name Name of the field
					@param value Value of the field
					@return HTML input command
					@todo integrate language parameter to internationalize the labels
				*/
				static std::string getOuiNonRadioInput(const std::string& name, bool value);
			//@}

			static std::string setFocus(const std::string& formName, const std::string& fieldName);

		};

		template<class S, class K, class T>
		std::string Html::getSortedSelectInput( const std::string& name, const std::map<S, std::pair<K, T> >& choices, const K& value )
		{
			std::stringstream s;
			s << "<select name=\"" << name << "\">";
			for (typename std::map<S, std::pair<K, T> >::const_iterator it = choices.begin(); it != choices.end(); ++it)
			{
				s << "<option value=\"" << it->second.first << "\"";
				if (it->second.first == value)
					s << " selected=\"1\"";
				s << ">" << it->second.second << "</option>";
			}
			s << "</select>";
			return s.str();
		}

		template<class K, class T>
		std::string Html::getSelectInput(const std::string& name, const std::map<K, T>& choices, const K& value)
		{
			std::stringstream s;
			s << "<select name=\"" << name << "\">";
			for (typename std::map<K, T>::const_iterator it = choices.begin(); it != choices.end(); ++it)
			{
				s << "<option value=\"" << it->first << "\"";
				if (it->first == value)
					s << " selected=\"1\"";
				s << ">" << it->second << "</option>";
			}
			s << "</select>";
			return s.str();
		}

		template<class K, class T>
		std::string Html::getRadioInput(const std::string& name, const std::map<K, T>& choices, const K& value)
		{
			std::stringstream s;
			for (typename std::map<K, T>::const_iterator it = choices.begin(); it != choices.end(); ++it)
			{
				s << "<input name=\"" << name << "\" type=\"radio\" value=\"" << it->first << "\"";
				if (it->first == value)
					s << " checked=\"1\"";
				s << " />" << it->second;
			}
			return s.str();
		}

	}
}

#endif // SYNTHESE_Html_H__
