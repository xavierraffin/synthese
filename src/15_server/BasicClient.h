#ifndef SYNTHESE_SERVER_BASIC_CLIENT_h__
#define SYNTHESE_SERVER_BASIC_CLIENT_h__

#include <string>
#include <map>


namespace synthese
{

	namespace server
	{

		//////////////////////////////////////////////////////////////////////////
		/// Simple HTTP client.
		/// @param m15
		class BasicClient
		{
		private:

		    const std::string _serverHost;      //!< Server host.
			const std::string _serverPort;              //!< Server port.
		    const int _timeOut;                 //!< TCP time out in milliseconds. 0 means no timeout.
			const bool _outputHTTPHeaders;
			const bool _acceptGzip;


			std::string _send(
				const std::string& url,
				const std::string& postData,
				const std::string& contentType
			) const;

		public:

		    BasicClient (
				const std::string& serverHost,
				const std::string serverPort = "8080",
				int timeOut = 0,
				bool outputHTTPHeaders = false,
				bool acceptGzip = true
			);

		    /**
		     * Sends a request to a server and writes received answer on an output stream.
		     * @param out : The output stream to write on.
		     * @param request : Request string to send to server (ex : fonction=rp&si=1&da=A)
		     * @param clientIp : Client IP (only relevant in a CGI context)
		     * @param clientURL : Client URL for link generation (only relevant in a CGI context)



			 // Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
			 //
			 // Distributed under the Boost Software License, Version 1.0. (See accompanying
			 // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
			 //



		     */
			std::string get(
				const std::string& url
			) const;


			std::string post(
				const std::string& url,
				const std::string& data,
				std::string contentType = std::string()
			) const;


			//////////////////////////////////////////////////////////////////////////
			/// Simple HTTP GET request helper.
			/// @warning Only HTTP is supported, not HTTPS
			/// @param url the url to get (format http://host:port/path)
			static std::string Get(
				const std::string url
			);
		};
}	}

#endif // SYNTHESE_SERVER_BASIC_CLIENT_h__
