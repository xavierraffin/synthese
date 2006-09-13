#ifndef SYNTHESE_ENV_ADDRESS_H
#define SYNTHESE_ENV_ADDRESS_H


#include <vector>
#include <set>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "Vertex.h"



namespace synthese
{
namespace env
{


class LogicalPlace;
class Road;



/** Adress (road + metric offset)
 An address is a position on a road given a metric offset from the start of the road.

An address may be associated with a connection place in the following cases :
  - The address corresponds to a crossing between two roads
  - The address belongs to a logical stop

*/
 class Address : 
     public synthese::util::Registrable<uid,Address>,
     public Vertex
{
private:

    const Road* _road;    //!< Address road.
    double _metricOffset;  //!< Metric offset (meters)

public:

    Address (const uid& id,
	     const AddressablePlace* place,
	     const Road* road, 
	     double metricOffset,
	     double x = UNKNOWN_VALUE,
	     double y = UNKNOWN_VALUE);

    ~Address();


    //! @name Getters/Setters
    //@{
    const uid& getId () const;

    const Road* getRoad() const;
    double getMetricOffset () const;
    //@}



    //! @name Query methods
    //@{
    bool isAddress () const;
    bool isPhysicalStop () const;
    //@}



};

}
}

#endif
