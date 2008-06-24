
/** HTMLForm class header.
	@file HTMLForm.h

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

#ifndef SYNTHESE_HTMLForm_h__
#define SYNTHESE_HTMLForm_h__

#include <map>
#include <vector>
#include <utility>
#include <string>
#include <sstream>

#include "01_util/Constants.h"

#include "05_html/HTMLModule.h"

namespace synthese
{
	namespace time
	{
		class DateTime;
		class Date;
	}

	namespace html
	{

// Class definition ---------------------------------------------------------------------------------

		/** HTML form generator.
			@ingroup m05

			This class provides some useful ways of designing a HTML Form :
				- all HTML code is automatically generated according to the w3c standards
				- complex objects are provided (such calendars)
				- handles automatically the necessary hidden fields, that are displayed only if a visible fields does not exists
				- transform the form into a single properties display if the user has no sufficient rights
				- generates unique IDs to allow an easy object manipulation with JavaScript
				- generation of fake forms, based upon URLs that do the same action as a real FORM submit
				- some fields type can be generated without instantiate a HTMLForm element (static methods)

			Note for future developments : at the term of the HTMLForm development, it is wished to permit two usages of the HTMLForm class : statically without the benefit of all the automatic features as a simple code snippets library, or non statically as an advanced form handler

			To use the HTMLForm class statically, use directly all the static methods to generate fields code

			To use the HTMLForm class non statically, follow this example :
				-# Instantiate an HTMLForm object
					@code HTMLForm f("update","synthese"); @endcode
				-# Set the right of the current user to determinate if the form will be a real form or only a display of the actual values as text :
					@code f.setUpdateRight(true) @endcode
				-# Give to the form the hidden fields that must be generated if they do not exist as non hidden fields :
					@code f.addHiddenField("name","toto");
					      f.addHiddenField("surname","robert"); @endcode
				-# Display the form opening tag
					@code stream << f.open(); @endcode
				-# Display fields : WARNING : never use two form field generators in the same << operation
					@code stream << f.getTextInput("name","tutu"); @endcode
				-# Display a submit button
					@code stream << f.getSubmitButton("OK"); @endcode
				-# Close the form
					@code stream << f.close(); @endcode

			The HTML code generated by this example is :
@code
<form name="update" action="synthese" method="post">
<input type="text" name="name" value="tutu" id="update_name_AUTOGENERATEDFIELDID" />
<input type="submit" name="OK" />
<input type="hidden" name="surname" value="robert" />
</form>
@endcode

			Note that the "name" hidden input is replaced by the standard text field.
		*/
		class HTMLForm
		{
		public:
			typedef std::map<std::string, std::string> HiddenFieldsMap;

		protected:
			typedef std::map<std::string, int> IDCounterMap;
			
			const std::string	_name;				//!< Name of the form, must be unique within the page
			const std::string	_action;			//!< Action to be launched at the form submit
			IDCounterMap		_idCounter;			//!< ID map used to generate unique ID of the fields
			HiddenFieldsMap		_hiddenFields;		//!< Hidden fields that must be displayed at the form close
			HiddenFieldsMap		_initialFields;		//!< Stores all the initial values of the hidden fields even if they are replaced by non hidden fields
			bool				_updateRight;		//!< Right of the user



			/** Field unique ID generator.
				@param name Name of the field
				@param increment Indicates if the call must be counted in order to increment the id generated by the next call with the same name value
				@return std::string The generated unique ID
				@author Hugues Romain
				@date 2007
				
				The generated id is the concatenation of :
					- the form name,
					- the field name,
					- the keyword AUTOGENERATEDFORMID
					- an auto incremental ID if the field is already present and if the call is registered
				The keywords are separated by a _ characters.
			*/
			std::string _getFieldId(const std::string& name, bool increment=true);


			
		public:
			/** HTML Form constructor.
				@param name Name of the form in the HTML document
				@param action Action to run at the form submit
				@author Hugues Romain
				@date 2007				
			*/
			HTMLForm(const std::string& name, const std::string& action);



			//! @name Getters
			//@{
				/** Gets the name of the form.
					@return const std::string& the name of the form
					@author Hugues Romain
					@date 2008					
				*/
				const std::string& getName() const;


				
				
				/** Gets the update right status.
					@return bool the update right status
					@author Hugues Romain
					@date 2008
				*/
				bool getUpdateRight() const;
			//@}



			//! @name Setters
			//@{
				/** Update right setter.
					@param value true if a real form must be generated, false if only the value must be generated as simple text
					@author Hugues Romain
					@date 2007				
				*/
				void setUpdateRight(bool value);
			//@}



			//! @name Utilities
			//@{			
				/** Gets the unique id of a form field within the HTML page.
					@param name Name of the field
					@return std::string The unique id of the field
					@author Hugues Romain
					@date 2008
					@warning The result of getFieldId is not valid for radio elements (one id per field)
				*/
				std::string getFieldId(const std::string& name);



				/** Register of an hidden field.
					@param name Name of the field
					@param value Value of the field
					@author Hugues Romain
					@date 2007
					
					The corresponding code is generated at the form closing (see getHiddenFields method).
					If the field already exists, its value is replaced by the last one.
				*/
				void addHiddenField(const std::string& name, const std::string& value);



				/** Removes an hidden field if exists, to avoid to an interactive field to be in conflict with an hidden one.
					@param name Name of the hidden field to remove
					@param value Initial value of the field
					@author Hugues Romain
					@date 2007
				*/
				void removeHiddenFieldIfExists(const std::string& name, const std::string& value);



				/** URL Generator.
					@param overridingFields Fields value to output instead of the registered ones.
					@return std::string The URL launching the form (GET action)
					@author Hugues Romain
					@date 2007				
				*/
				std::string getURL(HiddenFieldsMap overridingFields = HiddenFieldsMap(), bool withInitialValues=true) const;
			//@}



			//! @name Code generators
			//@{
				/** Generator of the opening HTML tag.
					@return std::string The HTML generated code.
					@author Hugues Romain
					@date 2007
				*/
				std::string open(const std::string htmlComplement = std::string());



				/** Generator of the closing HTML tag.
					@return std::string The HTML generated code.
					@author Hugues Romain
					@date 2007
					
				*/
				std::string close();



				/** Generator of a script that gives the focus to a specific field at the page load.
					@param fieldName Name of the field to focus
					@return std::string The generated HTML code
					@author Hugues Romain
					@date 2007
					
				*/
				std::string setFocus(const std::string& fieldName) const;



				/** Generator of a script that gives the focus to a specific field at the page load, for input type=radio only.
					@param fieldName Name of the field to focus
					@param fieldRank Rank of the field element to focus
					@return std::string The generated HTML code
					@author Hugues Romain
					@date 2007
				*/
				std::string setFocus(const std::string& fieldName, int fieldRank) const;

				
				
				/** Gets all hidden fields to write on the page.
					@return std::string the generated HTML code
					@author Hugues Romain
					@date 2008
					This method erases the "hidden fields to be written" list after writing then into the page code.
					Note : the call of this method is useless if the standard close method is used to close the form on the page.

					@warning : Do not use this method until there is no more operations on the form, to avoid that a field be written twice for example
				*/
				std::string getHiddenFields();



				/** HTML Link button generator.
					@return std::string The generated HTML Code
					@author Hugues Romain
					@date 2007
					@note To override some field values, use HTMLModule::getLinkButon(getURL(HiddenFieldsMap), ...) instead.
				*/
				std::string getLinkButton(const std::string& caption, const std::string confirm="", const std::string icon="") const;

				
				
				/** Generates of link that submits the form content to a pop-up by POST method.
					@param content text of the link
					@param width width of the pop-pup
					@param height height of the pop-up
					@return std::string The generated HTML code
					@author Hugues Romain
					@date 2008					
				*/
				std::string getSubmitOnPopupLink(const std::string& content, int width, int height);
			//@}



			//! @name Fields generators
			//@{
				/** Externally sorted selection list HTML field (select)
					@param name Name of the field
					@param choices List of choices : a map sorted by elements of S (the sort value is not read), giving pairs "value => text"
					@param value Default selected value
					@return Selection list HTML input field
					@author Hugues Romain
					@date 2007					
				*/
				template<class K, class T>
				std::string getSelectInput(const std::string& name, const std::vector<std::pair<K, T> >& choices, const K& value);

				
				
				/** Radio input element generator (input type=radio).
					@param name name of the whole field (the same in each element)
					@param valueIfSelected value of the field element to transmit if it is checked
					@param valueToSelect checks the element at page load if equals to the valueIfSelected parameter
					@param label label of the input (default : nothing)
					@param disabled disabled status of the element (default : enabled)
					@return std::string the HTML generated code
					@author Hugues Romain
					@date 2008					
				*/
				template<class K>
				std::string getRadioInput(const std::string& name, const K& valueIfSelected, const K& valueToSelect, const std::string label="", bool disabled=false);

				
				
				/** Radio input collection generated upon a vector of values (input type=radio).
					@param name name of the field
					@param choices vector of pairs : first is the value of an element, second is the corresponding label
					@param value default value of the field that determinate which radio element is checked at the page load
					@return std::string the HTML generated code
					@author Hugues Romain
					@date 2008					
				*/
				template<class K>
				std::string getRadioInput(const std::string& name, const std::vector<std::pair<K, std::string> >& choices, const K& value);



				/** Simple text field HTML generator (input type=text).
					@param name Name of the field in the form
					@param value Default value present in the field at the page load
					@param displayTextBeforeTyping Text present in the field at the page load, that disapears when the field obtain the focus
					@return std::string The generated HTML code
					@author Hugues Romain
					@date 2007
				*/
				std::string getTextInput(const std::string& name, const std::string& value, std::string displayTextBeforeTyping="");



				/** Password field HTML generator (input type=password).
					@param name name of the field in the form
					@param value default value present in the field at the page load
					@return std::string the generated HTML code
					@author Hugues Romain
					@date 2008					
				*/
				std::string getPasswordInput(const std::string& name, const std::string& value);



				/** Interactive date-time selection calendar field generator.
					@param name Name of the field
					@param value Default value of the calendar
					@return std::string The generated HTML code
					@author Hugues Romain
					@date 2007

					In order to use the calendar field, you must include the following lines int the header of the page :
					@code
<style type="text/css">@import url(calendar-win2k-1.css);</style>
<script type="text/javascript" src="calendar.js"></script>
<script type="text/javascript" src="lang/calendar-en.js"></script>
<script type="text/javascript" src="calendar-setup.js"></script>
					@endcode
				*/
				std::string getCalendarInput(const std::string& name, const time::DateTime& value);



				/** Interactive date selection calendar field generator.
					@param name Name of the field
					@param value Default value of the calendar
					@return std::string The generated HTML code
					@author Hugues Romain
					@date 2007

					In order to use the calendar field, you must include the following lines int the header of the page :
					@code
<style type="text/css">@import url(calendar-win2k-1.css);</style>
<script type="text/javascript" src="calendar.js"></script>
<script type="text/javascript" src="lang/calendar-en.js"></script>
<script type="text/javascript" src="calendar-setup.js"></script>
					@endcode
				*/
				std::string getCalendarInput(const std::string& name, const time::Date& value);



				/** Text area field generator (textarea).
					@param name name of the field in the form
					@param value default text present in the text area at the page load
					@param rows number of rows to be displayed (vertical size)
					@param cols number of columns to be displayed (horizontal size)
					@return std::string the HTML generated code
					@author Hugues Romain
					@date 2008					
				*/
				std::string getTextAreaInput(const std::string& name, const std::string& value, int rows, int cols);



				/** Standard submit button generator (input type=submit).
					@param caption text displayed on the button
					@return std::string the HTML generated code
					@author Hugues Romain
					@date 2008					
				*/
				std::string getSubmitButton(const std::string& caption);



				/** Image submit button generator (input type=image).
					@param imageURL URL of the image
					@param alt Replacement text
					@param name Name of the input (optional)
					@return std::string HTML input button
					@author Hugues Romain
					@date 2008				
				*/
				std::string getImageSubmitButton(const std::string& imageURL, const std::string& alt, std::string name="im");



				/** Check box item generator (input type=checkbox).
					@param name name of the field in the form
					@param value value to transmit at the submit if the field is checked
					@param checked indicates if the field must be checked at the page load
					@return std::string the HTML generated code
					@author Hugues Romain
					@date 2008					
				*/
				std::string getCheckBox(const std::string& name, const std::string& value, bool checked);

				
				
				/** Options list containing numbers.
					@param name Name of the field
					@param mini Minimal number of the suite
					@param maxi Maximal number of the suite
					@param value Default value (optional / default = UNKNOWN_VALUE)
					@param step Step between each number (optional / default = 1). If step is a negative number, the choices are sorted descending.
					@param unknownValueText Additional choice for UNKNOWN_VALUE, placed at the top of the list :
						- if empty (default) : no UNKNOWN_VALUE choice
						- if non empty : UNKNOWN_VALUE choice, designed by the provided text in the list
					@return std::string Select HTML Tag containing all options elements as defined above
					@author Hugues Romain
					@date 2008				
				*/
				std::string getSelectNumberInput(
					const std::string& name
					, int mini
					, int maxi
					, int value=UNKNOWN_VALUE
					, int step=1
					, std::string unknownValueText = std::string()
				);



				/** Oui Non radio input.
					@param name Name of the field
					@param value Value of the field
					@return HTML input command
					@todo integrate language parameter to internationalize the labels
				*/
				std::string getOuiNonRadioInput(const std::string& name, bool value);
			//@}



			//! @name Static standard code generators
			//@{
				/** Static date selection calendar field.
					@param name name of the field in the form
					@param value default value of the field at the page load
					@param fieldId unique id of the field
					@param triggerId unique id of the link that launches the calendar display
					@param spanId unique id of the span that displays the chosen date when the calendar is hidden
					@return std::string the HTML generated code
					@author Hugues Romain
					@date 2008
				*/
				static std::string GetCalendarInput(
					const std::string& name
					, const time::Date& value
					, std::string fieldId = std::string()
					, std::string triggerId = std::string()
					, std::string spanId = std::string()
				);



				/** Static date-time selection calendar field.
					@param name name of the field in the form
					@param value default value of the field at the page load
					@param fieldId unique id of the field
					@param triggerId unique id of the link that launches the calendar display
					@param spanId unique id of the span that displays the chosen date when the calendar is hidden
					@return std::string the HTML generated code
					@author Hugues Romain
					@date 2008
				*/
				static std::string GetCalendarInput(
					const std::string& name
					, const time::DateTime& value
					, std::string fieldId = std::string()
					, std::string triggerId = std::string()
					, std::string spanId = std::string()
				);



				/** Standard text input field generator.
					@param name name of the field in the form
					@param value default value of the field
					@param displayTextBeforeTyping text to be displayed at the page load and erased at the first field focus
					@param fieldId unique ID of the field within the page
					@return std::string the HTML generated code
					@author Hugues Romain
					@date 2008					
				*/
				static std::string GetTextInput(
					const std::string& name
					, const std::string& value
					, std::string displayTextBeforeTyping = std::string()
					, std::string fieldId = std::string()
				);
			//@}
		};

// Method templates implementations ---------------------------------------------------------------



		template<class K, class T>
		std::string HTMLForm::getSelectInput( const std::string& name, const std::vector<std::pair<K, T> >& choices, const K& value )
		{
			std::stringstream s;

			if (!_updateRight)
			{
				for (typename std::vector<std::pair<K, T> >::const_iterator it = choices.begin(); it != choices.end(); ++it)
					if (it->first == value)
						s << it->second;
			}
			else
			{
				s << "<select name=\"" << name << "\" id=\"" << _getFieldId(name) << "\" >";
				for (typename std::vector<std::pair<K, T> >::const_iterator it = choices.begin(); it != choices.end(); ++it)
				{
					s << "<option value=\"" << it->first << "\"";
					if (it->first == value)
						s << " selected=\"selected\"";
					s << ">" << it->second << "</option>";
				}
				s << "</select>";
			}
			return s.str();
		}

		template<class K>
		std::string HTMLForm::getRadioInput(const std::string& name, const K& valueIfSelected, const K& valueToSelect, const std::string label, bool disabled)
		{
			if (!_updateRight)
			{
				return (valueIfSelected == valueToSelect)
					? label
					: std::string();
			}

			std::stringstream s;
			std::string id(_getFieldId(name));
			s << "<input name=\"" << name << "\" type=\"radio\" value=\"" << valueIfSelected << "\""
				<< " id=\"" << id << "\"";
			if (valueIfSelected == valueToSelect)
				s << " checked=\"checked\"";
			if (disabled)
				s << " disabled=\"disabled\"";
			s << " /><label for=\"" << id << "\">" << label << "</label>";
			std::stringstream vs;
			vs << valueToSelect;
			removeHiddenFieldIfExists(name, vs.str());
			return s.str();
		}

		template<class K>
		std::string HTMLForm::getRadioInput(const std::string& name, const std::vector<std::pair<K, std::string> >& choices, const K& value)
		{
			std::stringstream s;
			for (typename std::vector<std::pair<K, std::string> >::const_iterator it = choices.begin(); it != choices.end(); ++it)
			{
				s << getRadioInput(name, it->first, value, it->second);
			}
			return s.str();
		}

	}
}

#endif // SYNTHESE_HTMLForm_h__
