#include "Axis.h"

namespace synthese
{
namespace env
{

Axis::Axis (const uid& id,
	    const std::string& name,
	    bool free, bool allowed)
    : synthese::util::Registrable<uid,Axis> (id)
      , _name (name)
      , _free (free)
      , _allowed (allowed)
{

}


Axis::~Axis ()
{

}



const std::string& 
Axis::getName () const
{
    return _name;
}


bool 
Axis::isFree () const
{
    return _free;
}


bool 
Axis::isAllowed () const
{
    return _allowed;
}


}
}
