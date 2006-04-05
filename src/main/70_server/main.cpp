#include "Server.h"

#include "01_util/Conversion.h"
#include "01_util/Exception.h"
#include "01_util/Log.h"


#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

using synthese::util::Log;
using synthese::util::Conversion;


namespace po = boost::program_options;


int main( int argc, char **argv )
{
    //  int port = 3591;
    // int threads = 10;

//  boost program_options causes crashes with GCC 4.0
// VIRER boost program options !!!!! bugg� !!!!
    int loglevel;
    int port;
    int threads;
    std::string datadir;
    std::string tempdir;

    std::string httptempdir;
    std::string httptempurl;

    po::options_description desc("Allowed options");
    desc.add_options()
	("help", "produce this help message")
	("datadir", po::value<std::string>(&datadir)->default_value ("./data"), 
                "data files directory")

	("tempdir", po::value<std::string>(&tempdir)->default_value ("."), 
                "temporary files directory")

	("loglevel", po::value<int>(&loglevel)->default_value (1), 
                "log level (0:debug ; 1:info ; 2:warn; 3:error; 4:fatal; 5:none)")

	("port", po::value<int>(&port)->default_value (3591), 
                "TCP service port")

	("threads", po::value<int>(&threads)->default_value (10), 
                "number of parallel threads")

	("httptempdir", po::value<std::string>(&httptempdir)->default_value ("."), 
                "temporary files directory accessible through an HTTP server")

    ("httptempurl", po::value<std::string>(&httptempurl)->default_value ("http://localhost/tmp"), 
                "URL to access temporary files publicly available through an HTTP server")
	;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    
    if (vm.count("help")) {
	std::cout << desc << std::endl;
	return 1;
    }

    const boost::filesystem::path& workingDir = boost::filesystem::initial_path();
    Log::GetInstance ().info ("Working dir  = " + workingDir.string ());


    // Configure default log (default output is cout).
    synthese::util::Log::GetInstance ().setLevel (
	(synthese::util::Log::Level) loglevel);
    
    Log::GetInstance ().info ("");
    Log::GetInstance ().info ("Param datadir  = " + datadir);
    Log::GetInstance ().info ("Param tempdir  = " + tempdir);
    Log::GetInstance ().info ("Param loglevel = " + Conversion::ToString (loglevel));
    Log::GetInstance ().info ("Param port     = " + Conversion::ToString (port));
    Log::GetInstance ().info ("Param threads  = " + Conversion::ToString (threads));
    Log::GetInstance ().info ("Param httptempdir  = " + httptempdir);
    Log::GetInstance ().info ("Param httptempurl  = " + httptempurl);
    Log::GetInstance ().info ("");

    synthese::server::Server server (port, threads, datadir, tempdir, httptempdir, httptempurl);
    synthese::server::Server::SetInstance (&server);
    try
    {
	synthese::server::Server::GetInstance ()->run ();
    }
    catch (synthese::util::Exception& ex)
    {
	Log::GetInstance ().fatal ("Exit!", ex);
    }
}

