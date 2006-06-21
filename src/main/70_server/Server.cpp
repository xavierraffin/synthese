#include "Server.h"

#include "CleanerThreadExec.h"
#include "ServerThread.h"


#include "RequestException.h"
#include "Request.h"
#include "RequestDispatcher.h"

#include "01_util/Thread.h"
#include "01_util/Log.h"
#include "01_util/Exception.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/thread/thread.hpp>

#ifdef MODULE_39
#include "39_carto/MapRequestHandler.h"
#include "39_carto/MapBackgroundManager.h"
#endif

using synthese::util::Log;
using synthese::util::Thread;

using namespace boost::posix_time;


namespace synthese
{
namespace server
{


Server* Server::_instance = 0;


Server::Server (int port, 
		int nbThreads,
		const std::string& dataDir,
		const std::string& tempDir,
        const std::string& httpTempDir, 
        const std::string& httpTempUrl)
    : _port (port)
    , _nbThreads (nbThreads)
	, _dataDir (dataDir, boost::filesystem::native)
    , _tempDir (tempDir, boost::filesystem::native)
    , _httpTempDir (httpTempDir, boost::filesystem::native)
    , _httpTempUrl (httpTempUrl)
{
}



Server::~Server ()
{
}



Server* 
Server::GetInstance ()
{
    if (_instance == 0)
    {
	    throw new synthese::util::Exception ("Server instance has never been set!");
    }
    return _instance;
}



void 
Server::SetInstance (Server* instance)
{
    _instance = instance;
}

    


void 
Server::initialize ()
{
    _dataDir.normalize ();
    if (boost::filesystem::exists (_dataDir) == false)
    {
	throw synthese::util::Exception ("Cannot find data directory '" + _dataDir.string () + "'");
    }
    _tempDir.normalize ();
    if (boost::filesystem::exists (_tempDir) == false)
    {
	throw synthese::util::Exception ("Cannot find temp directory '" + _tempDir.string () + "'");
    }

#ifdef MODULE_39
    
    // Initialize map background manager
    synthese::carto::MapBackgroundManager::SetBackgroundsDir (_dataDir / "backgrounds");
    synthese::carto::MapBackgroundManager::Initialize ();

    synthese::server::RequestDispatcher::getInstance ()->
	registerHandler (new synthese::carto::MapRequestHandler ());
#endif
    

}




void 
Server::run () 
{
    Log::GetInstance ().info ("Starting server...");

    try 
    {
	initialize ();

	synthese::tcp::TcpService* service = 
	    synthese::tcp::TcpService::openService (_port);
	

	ServerThread serverThread (service);
	CleanerThreadExec cleanerExec;

	// Every 4 hours, old files of http temp dir are cleant 
	time_duration checkPeriod = hours(4); 
	cleanerExec.addTempDirectory (_httpTempDir, checkPeriod);

	
	if (_nbThreads == 1) 
	{
	    // Monothread execution ; easier for debugging
	    Log::GetInstance ().info ("Server ready.");
	    serverThread ();
	}
	{
	    // Create the thread group.
	    boost::thread_group threads;
	    
	    // Creates all server threads.
	    for (int i=0; i< _nbThreads; ++i) 
	    {
		threads.create_thread (serverThread);
	    }

	    // Create the cleaner thread (check every 5s)
	    Thread cleanerThread (cleanerExec, "*cleaner*", 5000);
	    cleanerThread.start ();

	    Log::GetInstance ().info ("Server ready.");
	    threads.join_all();
	}
	
    }
    catch (std::exception& ex)
    {
	Log::GetInstance ().fatal ("", ex);
    } 


    synthese::tcp::TcpService::closeService (_port);
}




int 
Server::getPort () const
{
    return _port;
}



int 
Server::getNbThreads () const
{
    return _nbThreads;
}




const boost::filesystem::path& 
Server::getDataDir () const
{
    return _dataDir;
}



const boost::filesystem::path& 
Server::getTempDir () const
{
    return _tempDir;
}


const boost::filesystem::path& 
Server::getHttpTempDir () const
{
    return _httpTempDir;
}


const std::string& 
Server::getHttpTempUrl () const
{
    return _httpTempUrl;
}

    




}
}




