
//////////////////////////////////////////////////////////////////////////
/// WebPageUpdateAction class header.
///	@file WebPageUpdateAction.h
///	@author Hugues
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_WebPageUpdateAction_H__
#define SYNTHESE_WebPageUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace transportwebsite
	{
		class WebPage;

		//////////////////////////////////////////////////////////////////////////
		/// WebPageUpdateAction action class.
		/// @ingroup m56Actions refActions
		class WebPageUpdateAction:
			public util::FactorableTemplate<server::Action, WebPageUpdateAction>
		{
		public:
			static const std::string PARAMETER_WEB_PAGE_ID;
			static const std::string PARAMETER_TITLE;
			static const std::string PARAMETER_CONTENT1;
			static const std::string PARAMETER_INCLUDE1;
			static const std::string PARAMETER_CONTENT2;
			static const std::string PARAMETER_INCLUDE2;
			static const std::string PARAMETER_CONTENT3;

		private:
			boost::shared_ptr<WebPage> _page;
			std::string _content1;
			std::string _include1;
			std::string _content2;
			std::string _include2;
			std::string _content3;
			std::string _title;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);
			


			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			void setWebPage(boost::shared_ptr<WebPage> value);
		};
	}
}

#endif // SYNTHESE_WebPageUpdateAction_H__
