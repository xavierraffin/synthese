
/** RoadChunk class header.
	@file RoadChunk.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_ENV_ROADCHUNK_H
#define SYNTHESE_ENV_ROADCHUNK_H


#include "Edge.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"



namespace synthese
{
namespace env
{

    class Address;
    class Road;



/** Association class between road and address.
A road chunk is always delimited by two adresses, with no other
address in between.
These delimiting addresses can correspond to :
- a crossing address
- a physical stop address
- a public place address

 @ingroup m15
*/
class RoadChunk : 
    public synthese::util::Registrable<uid,RoadChunk>,
    public Edge 
{
private:

    const Address* _fromAddress;

public:

  RoadChunk (uid id = UNKNOWN_VALUE,
	     const Address* fromAddress = NULL,
	     int rankInRoad = UNKNOWN_VALUE,
	     bool isDeparture = false,
	     bool isArrival = false);

  
  virtual ~RoadChunk ();


  //! @name Getters/Setters
  //@{
  const Vertex* getFromVertex () const;
  double getMetricOffset () const;
	void setFromAddress(const Address* fromAddress);
  //@}
    

  //! @name Query methods.
  //@{
  //@}

};


}
}

#endif 

