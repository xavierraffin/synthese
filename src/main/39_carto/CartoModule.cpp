
#include "39_carto/CartoModule.h"
#include "39_carto/MapBackgroundManager.h"
#include "30_server/Server.h"



namespace synthese
{
	using namespace carto;

	namespace server
	{

		void CartoModule::initialize( const Server* server )
		{
			// Initialize map background manager
			MapBackgroundManager::SetBackgroundsDir (server->getConfig ().getDataDir () / "backgrounds");
			MapBackgroundManager::Initialize ();
		}
	}
}
