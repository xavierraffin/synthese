#ifndef SYNTHESE_CARTOSERVICE_MAPREQUESTHANDLER_H
#define SYNTHESE_CARTOSERVICE_MAPREQUESTHANDLER_H




#include "11_interfaces/Request.h"

#include <boost/filesystem/path.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#include <assert.h>
#include <stdlib.h>

#include <boost/filesystem/operations.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include "01_util/XmlParser.h"


namespace synthese
{


	namespace env
	{
		class Environment;
	}

	namespace carto
	{
		class Map;
		class RenderingConfig;
	}

	namespace interfaces
	{

		/** Map request handler.

		Generates a map given all the information to be drawn.

		Two modes are available for requesting :

		* Mode 1 : "Socket mode"
		Request format : mode=1&output=png&env={XML_ENV}&map={XML_MAP}
			where XML_ENV is the XML definition of the local
			environment used for map generation; 
			where XML_MAP is the XML definition of the map 
			drawing parameters.
		Response type : PNG file sent through socket
		Response format : |{PNGFILESIZE}|:|{PNG FILE DATA}
		  
		* Mode 2 : "HTTP mode"
		Request format : mode=2&output=ps&env={XML_ENV}&map={XML_MAP}
			where XML_ENV is the XML definition of the local
			environment used for map generation; 
			where XML_MAP is the XML definition of the map 
			drawing parameters.
		Response type : JPEG file generated locally on server side.
		Response format : |{JPEG_URL}|
			where PNG_URL is the url of the generated PNG file.

		The different output types are :
		- ps  : PostScript
		- jpeg : JPEG bitmap
		- html : JPEG + HTML map (only available in http mode)
		- mapinfo : MID/MIF files returned in a zip
		  
		@ingroup m80
		*/
		class MapRequestHandler : public Request
		{
		private:
			static const std::string _factory_key;

			int _mode;
			std::string _output;
			XMLNode _envNode;
			XMLNode _mapNode;

		public:

			
			static const std::string MODE_PARAMETER;
			static const std::string OUTPUT_PARAMETER;
			static const std::string ENVIRONMENT_PARAMETER;
			static const std::string MAP_PARAMETER;

			static const int REQUEST_MODE_SOCKET;
			static const int REQUEST_MODE_HTTP;

		protected:
			ParametersMap getParametersMap() const;
			void setFromParametersMap(const ParametersMap& map);

		public:
			void run(std::ostream& stream) const;

		};
	}
}



#endif
