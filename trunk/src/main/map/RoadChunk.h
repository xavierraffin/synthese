#ifndef ROADCHUNK_H_
#define ROADCHUNK_H_

#include "Referrant.h"
#include "Address.h"
#include <vector>



namespace synmap
{

class Vertex;
class Location;
class Topography;
class Road;


class RoadChunk : public Referrant
{
public:
	
private:
	
	const std::vector<const Location*> _steps;
	const std::vector<const Vertex*> _passagePoints;

	const Address::AddressNumber _rightStartNumber;   // inclusive
	const Address::AddressNumber _rightEndNumber;     // inclusive
	
	const Address::AddressNumber _leftStartNumber;    // inclusive
	const Address::AddressNumber _leftEndNumber;      // inclusive

	const Road* _road;

	const double _length; // total length of the road chunk in meters.

public:

	RoadChunk(Topography* topography,
		  int key,
		  const std::vector<const Location*>& steps,
		  Address::AddressNumber rightStartNumber, 
		  Address::AddressNumber rightEndNumber, 
		  Address::AddressNumber leftStartNumber, 
		  Address::AddressNumber leftEndNumber);
	
	virtual ~RoadChunk();


	const Road* getRoad () const { return _road; }
	void setRoad (const Road* road);

	Address::AddressNumber getRightStartNumber () const { return _rightStartNumber; }
	Address::AddressNumber getRightEndNumber () const { return _rightEndNumber; }
	Address::AddressNumber getLeftStartNumber () const { return _leftStartNumber; }
	Address::AddressNumber getLeftEndNumber () const { return _leftEndNumber; }

	Address::AddressNumberSide getSideForNumber (Address::AddressNumber number) const;
	bool hasNumber (Address::AddressNumber number) const;

	const Location* getStep (int index) const { return _steps[index]; }

	double getLength () const { return _length; }

private:

	static double 
	  computeTotalLength (const std::vector<const Vertex*>& passagePoints);
	static std::vector<const Vertex*> 
	  filterPhysicalStops (const std::vector<const Location*>& steps);

};

}

#endif /*ROADCHUNK_H_*/
