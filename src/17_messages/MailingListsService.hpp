
//////////////////////////////////////////////////////////////////////////////////////////
///	MailingListsService class header.
///	@file MailingListsService.hpp
///	@author hromain
///	@date 2013
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_MailingListsService_H__
#define SYNTHESE_MailingListsService_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace messages
	{
		class MailingList;

		//////////////////////////////////////////////////////////////////////////
		///	56.15 Function : MailingListsService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Mailing_lists
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m56Functions refFunctions
		///	@author hromain
		///	@date 2013
		/// @since 3.7.0
		class MailingListsService:
			public util::FactorableTemplate<server::Function,MailingListsService>
		{
		public:
			static const std::string PARAMETER_WITH_SUBSCRIPTIONS;
			static const std::string TAG_MAILING_LIST;

			
		protected:
			//! \name Page parameters
			//@{
				const MailingList* _mailingList;
				bool _withSubscriptions;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Mailing_lists#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author hromain
			/// @date 2013
			/// @since 3.7.0
			util::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Mailing_lists#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author hromain
			/// @date 2013
			/// @since 3.7.0
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);
			
			
		public:
			MailingListsService();

			//! @name Setters
			//@{
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author hromain
			/// @date 2013
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author hromain
			/// @date 2013
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author hromain
			/// @date 2013
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_MailingListsService_H__

