#include "MapRequestHandler.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <assert.h>
#include <stdlib.h>

#include <boost/filesystem/operations.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "01_util/XmlParser.h"
#include "01_util/Conversion.h"

#include "70_server/Server.h"
#include "70_server/Request.h"
#include "70_server/RequestException.h"

#include "15_env/Environment.h"
#include "16_env_ls_xml/EnvironmentLS.h"

#include "39_carto/Map.h"
#include "39_carto/RenderingConfig.h"
#include "39_carto/PostscriptRenderer.h"
#include "39_carto/HtmlMapRenderer.h"
#include "40_carto_ls_xml/MapLS.h"

#include "01_util/Log.h"


using synthese::env::Environment;
using synthese::carto::Map;
using synthese::util::Conversion;
using synthese::util::Log;
using synthese::server::Server;
using namespace boost::posix_time;


namespace synthese
{
namespace carto
{

#ifdef WIN32
const std::string MapRequestHandler::GHOSTSCRIPT_BIN ("gswin32");
#else
const std::string MapRequestHandler::GHOSTSCRIPT_BIN ("gs");
#endif

const std::string MapRequestHandler::FUNCTION_CODE ("map");

const std::string MapRequestHandler::MODE_PARAMETER ("mode");
const std::string MapRequestHandler::ENVIRONMENT_PARAMETER ("env");
const std::string MapRequestHandler::MAP_PARAMETER ("map");

const int MapRequestHandler::REQUEST_MODE_SOCKET (1);
const int MapRequestHandler::REQUEST_MODE_HTTP (2);


MapRequestHandler::MapRequestHandler ()
    : synthese::server::RequestHandler (FUNCTION_CODE)
{
}



MapRequestHandler::MapRequestHandler (const MapRequestHandler& ref)
    : synthese::server::RequestHandler (ref)
{

}



MapRequestHandler::~MapRequestHandler ()
{

}



void 
MapRequestHandler::handleRequest (const synthese::server::Request& request,
				  std::ostream& stream) const
{
    int mode = Conversion::ToInt (request.getParameter (MODE_PARAMETER));
    if ((mode != REQUEST_MODE_SOCKET) && (mode != REQUEST_MODE_HTTP))
    {
        throw synthese::server::RequestException ("Invalid map request mode " + Conversion::ToString (mode));
    }

    XMLNode envNode = XMLNode::parseString (request.getParameter (ENVIRONMENT_PARAMETER).c_str (),
					    synthese::envlsxml::EnvironmentLS::ENVIRONMENT_TAG.c_str ());

    Environment* env = synthese::envlsxml::EnvironmentLS::Load (envNode);

    XMLNode mapNode = XMLNode::parseString (request.getParameter (MAP_PARAMETER).c_str (),
					    synthese::cartolsxml::MapLS::MAP_TAG.c_str ());
    Map* map = synthese::cartolsxml::MapLS::Load (mapNode, *env);

    // Create a temporary file name based on system time
    const boost::filesystem::path& tempDir = (mode == REQUEST_MODE_SOCKET) 
            ? Server::GetInstance ()->getTempDir ()
            : Server::GetInstance ()->getHttpTempDir ();

    // Generate an id for the map file based on current time
    ptime timems = boost::date_time::microsec_clock<ptime>::local_time ();

    const boost::filesystem::path tempPsFile (tempDir / ("map_" + Conversion::ToString ((unsigned long) &request) + ".ps"));
    const std::string tempPngFilename = "map_" + to_iso_string (timems) + ".png";
    const boost::filesystem::path tempPngFile (tempDir / tempPngFilename);

    // Create the postscript canvas for output
    std::ofstream of (tempPsFile.string ().c_str ());

    RenderingConfig conf;
    synthese::carto::PostscriptRenderer psRenderer (conf, of);
    psRenderer.render (*map);

    std::ofstream of2 ("c:/temp/testmap.html");
    synthese::carto::HtmlMapRenderer hmRenderer (conf, of2);
    hmRenderer.render (*map);

    of.close ();

    // Convert the ps file to png with ghostscript
    std::stringstream gscmd;
    gscmd << GHOSTSCRIPT_BIN << " -q -dSAFER -dBATCH -dNOPAUSE -sDEVICE=png16m -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -g" 
	  << map->getWidth () << "x" << map->getHeight () 
	  << " -sOutputFile=" << tempPngFile.string () << " " << tempPsFile.string ();
    
    Log::GetInstance ().debug (gscmd.str ());

    int ret = system (gscmd.str ().c_str ());
    
    if (ret != 0)
    {
	throw synthese::util::Exception ("Error executing GhostScript (gs executable in path ?)");
    }
    // boost::filesystem::remove (tempPsFile);

    // Now get size of the generated PNG file...
    long size = boost::filesystem::file_size (tempPngFile);
    
    std::ifstream ifpng (tempPngFile.string ().c_str (), std::ifstream::binary);
    char * buffer;
    buffer = new char [size];
    ifpng.read (buffer, size);
    ifpng.close();

    if (mode == REQUEST_MODE_SOCKET) 
    {
        // Send the content of the file through the socket.
        stream << size << ":";
        stream.write (buffer, size);
        stream << std::flush;

        // Remove the PNG file
        boost::filesystem::remove (tempPngFile);
    }
    else if (mode == REQUEST_MODE_HTTP)
    {
        // Send the URL to the the generated local PNG file.
        stream << Server::GetInstance ()->getHttpTempUrl () 
               << "/" << tempPngFilename << std::endl;
    }

    delete[] buffer;

    Log::GetInstance ().debug ("Sent PNG result (" + Conversion::ToString (size) + " bytes)");


    delete map;
    delete env;
}








}
}
