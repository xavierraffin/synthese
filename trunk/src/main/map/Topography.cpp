#include "Topography.h"
#include "Vertex.h"
#include "Location.h"
#include "Edge.h"
#include "Road.h"
#include "RoadChunk.h"
#include "Itinerary.h"
#include "PhysicalStop.h"
#include "Zone.h"

#include <assert.h>

namespace synmap
{

Topography::Topography(const cEnvironnement* _environment)
: _environment (environment)
{
}

Topography::~Topography()
{
	for (unsigned int i=0; i<_roads.size (); ++i) {
		delete _roads[i];	
	}
	for (unsigned int i=0; i<_roadChunks.size (); ++i) {
		delete _roadChunks[i];	
	}
	for (unsigned int i=0; i<_itineraries.size (); ++i) {
		delete _itineraries[i];	
	}
	for (unsigned int i=0; i<_physicalStops.size (); ++i) {
		delete _physicalStops[i];	
	}
	for (unsigned int i=0; i<_zones.size (); ++i) {
		delete _zones[i];	
	}

	for (VertexMap2D::iterator vit2 = _vertices.begin (); vit2 != _vertices.end (); ++vit2) {
		VertexMap1D* vm1 = vit2->second;		
		for (VertexMap1D::iterator vit1 = vm1->begin (); vit1 != vm1->end (); ++vit1) {
			delete vit1->second; // delete the vertex.
		}
	}
		
	for (EdgeMap2D::iterator eit2 = _edges.begin (); eit2 != _edges.end (); ++eit2) {
		EdgeMap1D* em1 = eit2->second;		
		for (EdgeMap1D::iterator eit1 = em1->begin (); eit1 != em1->end (); ++eit1) {
			delete eit1->second; // delete the edge.
		}
	}
	for (unsigned int i=0; i<_locations.size (); ++i) {
		delete _locations[i];	
	}

		
	
}


Vertex* 
Topography::newVertexPtr (double x, double y)
{
	VertexMap1D* xmap = _vertices[x];
	if (xmap == 0) {
		xmap = new VertexMap1D ();
		_vertices[x] = xmap;
	} 
	
	Vertex* vertex = (*xmap)[y];
	if (vertex == 0) {
		vertex = new Vertex (x, y);
		(*xmap)[y] = vertex;
	}
	
	return vertex; 	
}



Edge* 
Topography::newEdgePtr (const Vertex* from, const Vertex* to)
{
	EdgeMap1D* frommap = _edges[from];
	if (frommap == 0) {
		frommap = new EdgeMap1D ();
		_edges[from] = frommap;
	} 
	
	Edge* edge = (*frommap)[to];
	if (edge == 0) {
		edge = new Edge (from, to);
		((Vertex*) from)->addEdge (edge);
		((Vertex*) to)->addEdge (edge);
		(*frommap)[to] = edge;
	}
	
	return edge; 	
}




const Vertex* 
Topography::newVertex (double x, double y) 
{
	return newVertexPtr (x, y);  
}



const Location* 
Topography::newLocation (double x, double y) {
  return newLocation (newVertex (x, y));
}


const Location* 
Topography::newLocation (const Vertex* vertex) {
  const Location* location = new Location (vertex);
  _locations.push_back (location);
  return location;
}


const Edge* 
Topography::newEdge (const Vertex* from, const Vertex* to) 
{
	return newEdgePtr (from, to);  
}



const Vertex* 
Topography::getVertex (double x, double y) 
{
	return (*_vertices[x])[y];
}



bool
Topography::hasEdge (const Vertex* from, const Vertex* to) 
{
	return (_edges[from] != 0) && (*_edges[from])[to] != 0;
}



const Edge* 
Topography::getEdge (const Vertex* from, const Vertex* to) const
{
  return (_edges.find (from)->second->find (to)->second);
}



const Road* 
Topography::newRoad (int key,
		     const std::string& name,
		     const Road::RoadType& type,
		     const std::string& discriminant,
		     const cCommune* town,
		     const std::vector<RoadChunk*>& chunks)
{
  Road* road = new Road (this, 
			 key, 
			 name, 
			 type,
			 discriminant,
			 town,
			 chunks);
  
  _roads.insert (make_pair (key, road));
  return road;

}



RoadChunk* 
Topography::newRoadChunk (int key,
			  const std::vector<const Location*>& steps,
			  const Address::AddressNumber& rightStartNumber, 
			  const Address::AddressNumber& rightEndNumber, 
			  const Address::AddressNumber& leftStartNumber, 
			  const Address::AddressNumber& leftEndNumber)
{
  RoadChunk* chunk = new RoadChunk (this, key, steps,
				    rightStartNumber,
				    rightEndNumber,
				    leftStartNumber,
				    leftEndNumber);	
  
  _roadChunks.insert (make_pair (key, chunk));
  for (std::vector<const Location*>::const_iterator iter = steps.begin ();
       iter != steps.end (); ++iter) 
    {
      addVertexReferrant ((*iter)->getVertex (), chunk);
    }
  
  
  // We consider for now that the road chunk is always double side.
  // pourquoi marche pas ????????????
  std::vector<const Edge*> edges;
  for (unsigned int i=0; i+1<steps.size (); ++i) {
    const Edge* edge = newEdge (steps[i]->getVertex(), steps[i+1]->getVertex());
    addEdgeReferrant (edge, chunk);
    edge = newEdge (steps[i+1]->getVertex(), steps[i]->getVertex());
    addEdgeReferrant (edge, chunk);
    }
  
  
  return chunk;	
}




const PhysicalStop* 
Topography::newPhysicalStop (
			   int logicalPlaceId,
			   int rank,
			   const Vertex* vertex)
{
  assert (vertex != 0);
  PhysicalStop* stop = new PhysicalStop (this, logicalPlaceId, rank, vertex);
  
  _physicalStops.insert (make_pair (stop->getKey (), stop));
  
  addVertexReferrant (vertex, stop);
  
  return stop;
	
}



const Itinerary* 
Topography::newItinerary (int key,
                          const std::vector<const Location*>& steps)

{
	assert (steps.size () > 1);
	Itinerary* itinerary = new Itinerary (this, key, steps);
	
	_itineraries.insert (make_pair (key, itinerary));
	
	for (unsigned int i=0; i<steps.size(); ++i) {
	  addVertexReferrant (steps[i]->getVertex(), itinerary);
	}

	std::vector<const Edge*> edges = itinerary->getEdges ();
	for (unsigned int i=0; i<edges.size (); ++i) {
		addEdgeReferrant (edges[i], itinerary);
	}
	
	return itinerary;
	
}



const Zone* 
Topography::newZone (int key, 
                     const std::vector<const Vertex*>& vertices)
{
	Zone* zone = new Zone (this, key, vertices);
	
	_zones.insert (make_pair (key, zone));

	for (unsigned int i=0; i<vertices.size(); ++i) {
		addVertexReferrant (vertices[i], zone);
	}
	
	return zone;
	
}


const Itinerary* 
Topography::getItinerary (int key) const 
{
    if (_itineraries.find (key) == _itineraries.end ()) return 0;
    return _itineraries.find (key)->second;  
}



const Road* 
Topography::getRoad (int key) const 
{
  if (_roads.find (key) == _roads.end ()) return 0;
  return _roads.find (key)->second;  
}


void 
Topography::removeItinerary (int key) 
{
    if (_itineraries.find (key) == _itineraries.end ()) return;
    _itineraries.erase (_itineraries.find (key));  
}



void 
Topography::addVertexReferrant (const Vertex* vertex, const Referrant* ref)
{
	Referrants* refs = _vertexReferrants[vertex];
	if (refs == 0) {
		refs = new Referrants ();
		_vertexReferrants[vertex] = refs;
	}
	
	if (const RoadChunk* roadChunk = dynamic_cast<const RoadChunk*> (ref)) 
	{
		refs->roadChunks.insert (roadChunk);
	}
	else if (const PhysicalStop* pstop = dynamic_cast<const PhysicalStop*> (ref)) 
	{
		refs->physicalStops.insert (pstop);
	}
	else if (const Itinerary* iti = dynamic_cast<const Itinerary*> (ref)) 
	{
		refs->itineraries.insert (iti);
	}
    else if (const Zone* zone = dynamic_cast<const Zone*> (ref)) 
    {
        refs->zones.insert (zone);
    }
    
}




void 
Topography::addEdgeReferrant (const Edge* edge, const Referrant* ref)
{
	Referrants* refs = _edgeReferrants[edge];
	if (refs == 0) {
		refs = new Referrants ();
		_edgeReferrants[edge] = refs;
	}
	
	if (const RoadChunk* roadChunk = dynamic_cast<const RoadChunk*> (ref)) 
	{
		refs->roadChunks.insert (roadChunk);
	}
	else if (const PhysicalStop* pstop = dynamic_cast<const PhysicalStop*> (ref)) 
	{
		refs->physicalStops.insert (pstop);
	}
	else if (const Itinerary* iti = dynamic_cast<const Itinerary*> (ref)) 
	{
		refs->itineraries.insert (iti);
	}
}



void 
Topography::findRoadChunks (const Vertex* vertex, 
							std::vector<const RoadChunk*>& dst, 
							const RoadChunkFilter filter) const
{
	VertexRefMap::const_iterator vit = _vertexReferrants.find (vertex);
	if (vit == _vertexReferrants.end ()) return;
	const Referrants* refs = vit->second;
	
	for (set<const RoadChunk*>::const_iterator cit = refs->roadChunks.begin ();
		 cit != refs->roadChunks.end (); ++cit) 
	{
		if ((filter == 0) || (filter (*cit)))
		{
			dst.push_back (*cit);
		}
	}
}


								
					



void Topography::findPhysicalStops (const Vertex* vertex, 
									std::vector<const PhysicalStop*>& dst,
									const PhysicalStopFilter filter) const
{
	VertexRefMap::const_iterator vit = _vertexReferrants.find (vertex);
	if (vit == _vertexReferrants.end ()) return;
	const Referrants* refs = vit->second;
	
	for (set<const PhysicalStop*>::const_iterator cit = refs->physicalStops.begin ();
		 cit != refs->physicalStops.end (); ++cit) 
	{
		if ((filter == 0) || (filter (*cit)))
		{
			dst.push_back (*cit);
		}
	}
	
}


void Topography::findItineraries (const Vertex* vertex, 
				  std::vector<const Itinerary*>& dst,
				  const ItineraryFilter filter) const
{
	VertexRefMap::const_iterator vit = _vertexReferrants.find (vertex);
	if (vit == _vertexReferrants.end ()) return;
	const Referrants* refs = vit->second;
	
	for (set<const Itinerary*>::const_iterator cit = refs->itineraries.begin ();
		 cit != refs->itineraries.end (); ++cit) 
	{
		if ((filter == 0) || (filter (*cit)))
		{
			dst.push_back (*cit);
		}
	}
}




void 
Topography::findZones (const Vertex* vertex, 
					   std::vector<const Zone*>& dst,
				       const ZoneFilter filter) const
{
	VertexRefMap::const_iterator vit = _vertexReferrants.find (vertex);
	if (vit == _vertexReferrants.end ()) return;
	const Referrants* refs = vit->second;
	
	for (set<const Zone*>::const_iterator cit = refs->zones.begin ();
		 cit != refs->zones.end (); ++cit) 
	{
		if ((filter == 0) || (filter (*cit)))
		{
			dst.push_back (*cit);
		}
	}
}


void 
Topography::findRoadChunks (const Edge* edge, 
							std::vector<const RoadChunk*>& dst, 
							const RoadChunkFilter filter) const
{
	EdgeRefMap::const_iterator eit = _edgeReferrants.find (edge);
	if (eit == _edgeReferrants.end ()) return;
	const Referrants* refs = eit->second;
	
	for (set<const RoadChunk*>::const_iterator cit = refs->roadChunks.begin ();
		 cit != refs->roadChunks.end (); ++cit) 
	{
		if ((filter == 0) || (filter (*cit)))
		{
			dst.push_back (*cit);
		}
	}
		
	
}				
				




void Topography::findItineraries (const Edge* edge, 
								  std::vector<const Itinerary*>& dst,
								  const ItineraryFilter filter) const
{
	EdgeRefMap::const_iterator eit = _edgeReferrants.find (edge);
	if (eit == _edgeReferrants.end ()) return;
	const Referrants* refs = eit->second;
	
	for (set<const Itinerary*>::const_iterator cit = refs->itineraries.begin ();
		 cit != refs->itineraries.end (); ++cit) 
	{
		if ((filter == 0) || (filter (*cit)))
		{
			dst.push_back (*cit);
		}
	}
	
}






}
