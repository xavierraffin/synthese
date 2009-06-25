
////////////////////////////////////////////////////////////////////////////////
/// Server Module class header.
///	@file ServerModule.h
///	@author Hugues Romain
///
/// @warning This header must always be included before all other ones to avoid
/// the windows bug "WinSock.h has already been included"
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_ServerModule_H__
#define SYNTHESE_ServerModule_H__

#include <boost/asio.hpp>
#include <boost/filesystem/path.hpp>

#include "ModuleClassTemplate.hpp"
#include "HTTPConnection.hpp"

namespace synthese
{
	/**	@defgroup m15Actions 15 Actions
		@ingroup m15

		@defgroup m18Functions 15 Functions
		@ingroup m15
	
		@defgroup m15 15 Server
		@ingroup m1
		@{
	*/

	/** 15 Server module namespace.
	*/
	namespace server
	{
		class Session;
		struct HTTPRequest;
		struct HTTPReply;

		/** 15 Server module class.
		*/
		class ServerModule:
			public ModuleClassTemplate<ServerModule>
		{
			friend class ModuleClassTemplate<ServerModule>;
		public:

		    //! DbModule parameters
		    static const std::string MODULE_PARAM_PORT;
		    static const std::string MODULE_PARAM_NB_THREADS;
		    static const std::string MODULE_PARAM_LOG_LEVEL;
		    static const std::string MODULE_PARAM_TMP_DIR;

		    typedef std::map<std::string, Session*> SessionMap;

		private:

			static SessionMap				_sessionMap;
			
			/// The io_service used to perform asynchronous operations.
			static boost::asio::io_service _io_service;
			
			/// Acceptor used to listen for incoming connections.
			static boost::asio::ip::tcp::acceptor _acceptor;
			
			/// The next connection to be accepted.
			static connection_ptr _new_connection;


		public:
			static SessionMap& getSessions();
			
			/** Called whenever a parameter registered by this module is changed
			 */
			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);


		public:
			/// Handle completion of an asynchronous accept operation.
			static void HandleAccept(
				const boost::system::error_code& e
			);
			
			/// Handle a request and produce a reply.
			/// @param req HTTP request to handle
			/// @param rep HTTP Reply to write the result on
			static void HandleRequest(
				const HTTPRequest& req,
				HTTPReply& rep
			);

			/// Perform URL-decoding on a string. 
			/// @return false if the encoding was invalid.
			static bool URLDecode(const std::string& in, std::string& out);
		};
	}
	/** @} */
}

#endif // SYNTHESE_ServerModule_H__
