
#include "Address.h"
#include "RoadChunk.h"
#include "Vertex.h"
#include "PhysicalStop.h"
#include "Topography.h"
#include <assert.h>


namespace synmap
{


/** Constructeur.
	@param __road Route sur laquelle se trouve l'adresse
	@param __address_number Num�ro d'adresse (rien = num�ro inconnu)
	
	@todo G�rer ici imm�diatement le PM : soit il est fourni, soit il est extrapol� d'apr�s le num�ro, soit on prend le milieu
*/
Address::Address(Road* __road, RoadChunk::AddressNumber __address_number)
	: _road(__road)
	, _address_number(__address_number)
{
}


/** Destructeur.
*/
Address::~Address()
{
}


/** Recherche des arr�ts physiques situ�s � proximit� de l'adresse.

	@param distance Distance maximale � parcourir
	
	L'algorithme consiste en un parcours int�gral du graphe des segments de route, born� par une distance maximale parcourue.

	@todo Remplacer findmostplausiblechunk par un findChunkByPM Voir le positionnement de cette question.

	@author Marc Jambert
*/
std::set< Address::PathToPhysicalStop >
Address::findPathsToPhysicalStops (double distance) const {
  const RoadChunk* chunk = _road->findMostPlausibleChunkForNumber (_address_number);
  // TODO : add an algorithm to find more precisely the vertex to
  // start from ?
  const Vertex* start = chunk->getStep (0)->getVertex ();
  std::set< std::vector<const Vertex*> > paths = 
    start->findPathsToCloseNeighbors (distance);

  std::set< PathToPhysicalStop > result;

  Road::RoadChunkVector tmpChunks;
  std::vector<const PhysicalStop*> tmpPhysicalStops;

  for (std::set< std::vector<const Vertex*> >::iterator path = paths.begin ();
       path != paths.end ();
       ++path) {
    
    for (int i=0; i<path->size (); ++i) {
      const Vertex* v = path->at (i);
      
      // Is there any physical stop located at this vertex ?
      tmpPhysicalStops.clear ();
      _road->getTopography()->findPhysicalStops (v, tmpPhysicalStops);

      for (std::vector<const PhysicalStop*>::const_iterator 
	     itPstop = tmpPhysicalStops.begin ();
	   itPstop != tmpPhysicalStops.end ();
	     ++itPstop) {
	const PhysicalStop* pstop = *itPstop;

	// Create an entry in result which is a path from start
	// to the found physical stop
	Road::RoadChunkVector pathChunks;
	
	for (int j=0; j+1<=i; ++j) {
	  const Edge* edge = _road->getTopography()->getEdge (path->at (j), path->at (j+1));
	  
	  // Normally, there can be only one road chunk associated
	  // with an edge.
	  tmpChunks.clear ();
	  _road->getTopography ()->findRoadChunks (edge, tmpChunks);

	  assert (tmpChunks.size () == 1);

	  // If the last chunk inserted is the same than the one 
	  // we just found, do nothing.
	  if ((pathChunks.size () > 0) &&
	      (tmpChunks.at(0) == pathChunks[pathChunks.size ()-1])) continue;

	  pathChunks.push_back (tmpChunks.at (0));

	}
	
	result.insert (PathToPhysicalStop (pathChunks, pstop));
	  
	
      }

    }
  }
  

  return result;

} 



}
