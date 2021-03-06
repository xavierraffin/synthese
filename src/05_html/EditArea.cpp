
/** EditArea class implementation.
	@file EditArea.cpp

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

#include "EditArea.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"

using namespace std;

namespace synthese
{
	namespace html
	{
		EditArea::EditArea(
			std::ostream& stream
		){
			stream << HTMLModule::GetHTMLJavascriptOpen("/lib/edit_area/edit_area_full.js");
		}



		void EditArea::getTextArea(
			std::ostream& stream,
			HTMLForm& form,
			const std::string& parameterName,
			const std::string& value,
			std::size_t rows,
			std::size_t cols
		){
			_getScript(stream, form.getFieldId(parameterName));
			stream << form.getTextAreaInput(parameterName, value, rows, cols, false);
		}



		void EditArea::getAjaxForm(
			std::ostream& stream,
			const std::string& ajaxSaveURL,
			const std::string& parameterName,
			const std::string& value,
			std::size_t rows,
			std::size_t cols
		){
			// Declarations
			const string id(parameterName + "_AUTOGENERATEDFIELDID");
			const string saveCallBackName(parameterName + "_save_callback");

			// Save callback code
			stream <<
				HTMLModule::GetHTMLJavascriptOpen() <<
				"function " << saveCallBackName << "(id, value)" <<
				"{" <<
					"var xhr;" <<
					"try {  xhr = new ActiveXObject('Msxml2.XMLHTTP');   }" <<
					"catch (e) " <<
					"{" <<
						"try {   xhr = new ActiveXObject('Microsoft.XMLHTTP');    }" <<
						"catch (e2) " <<
						"{" <<
							"try {  xhr = new XMLHttpRequest();     }" <<
							"catch (e3) {  xhr = false;   }" <<
						"}" <<
					"}" <<

					"xhr.onreadystatechange  = function()" <<
					"{ " <<
						"if(xhr.readyState  == 4)" <<
						"{" <<
							"if(xhr.status != 200) " <<
								"alert('Error during save action');" <<
						"}" <<
					"}; " <<

					"xhr.open('POST','" << ajaxSaveURL << "');" <<
					"xhr.setRequestHeader('Content-Type','application/x-www-form-urlencoded');" <<
					"xhr.send(document.getElementById(id).name +'='+ encodeURIComponent(value));" <<
				"}" <<
				HTMLModule::GetHTMLJavascriptClose()
			;
			_getScript(stream, id, saveCallBackName);
			stream << HTMLForm::GetTextAreaInput(parameterName, value, rows, cols, false, id);
		}



		void EditArea::_getScript(
			std::ostream& stream,
			const std::string& id,
			boost::optional<const std::string&> saveCallback
		) const	{
			stream <<
				HTMLModule::GetHTMLJavascriptOpen() <<
				"editAreaLoader.init({" <<
				"id : \"" << id << "\"" <<
				",syntax: \"html\"" <<
				",browsers: \"all\"" <<
				",start_highlight: true" <<
				",toolbar:\"";
			if(saveCallback)
			{
				stream << "save, |, ";
			}
			stream <<
				"search, go_to_line, fullscreen, |, undo, redo, |, select_font,|, change_smooth_selection, highlight, reset_highlight, word_wrap, |, help\""
			;
			if(saveCallback)
			{
				stream << ",save_callback:\"" << *saveCallback << "\"";
			}
			stream <<
				"});" <<
				HTMLModule::GetHTMLJavascriptClose();
		}
}	}
