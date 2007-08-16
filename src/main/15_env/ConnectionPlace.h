
/** ConnectionPlace class header.
	@file ConnectionPlace.h

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

#ifndef SYNTHESE_ENV_CONNECTIONPLACE_H
#define SYNTHESE_ENV_CONNECTIONPLACE_H


#include <map>
#include <set>
#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"
#include "01_util/Constants.h"

#include "15_env/AddressablePlace.h"
#include "15_env/Types.h"

#include "06_geometry/IsoBarycentre.h"

namespace synthese
{
	namespace time
	{
		class DateTime;
	}

	namespace geometry
	{
		class SquareDistance; 
	}

	namespace env
	{
		class Address;
		class Edge;
		class Path;

		/** A connection place indicates that there are possible
			connections between different network vertices.

			Each connection is associated with a type (authorized, 
			forbidden, recommended...) and a transfer delay.

			@ingroup m15
		*/
		class ConnectionPlace : 
			public util::Registrable<uid,ConnectionPlace>, 
			public AddressablePlace
		{
		public:

			static const int UNKNOWN_TRANSFER_DELAY;
			static const int FORBIDDEN_TRANSFER_DELAY;
			static const int SQUAREDISTANCE_SHORT_LONG;

		private:
			mutable bool _isoBarycentreToUpdateC;
			mutable geometry::IsoBarycentre _isoBarycentreC;

			PhysicalStops _physicalStops; 

			std::map< std::pair<uid, uid>, int > _transferDelays; //!< Transfer delays between vertices (in minutes)
			int _defaultTransferDelay;
			int _minTransferDelay;
			int _maxTransferDelay;

			ConnectionType _connectionType;

			mutable int _score;

		public:

			ConnectionPlace (
				uid id = UNKNOWN_VALUE
				, std::string name = std::string()
				, const City* city = NULL
				, ConnectionType connectionType = CONNECTION_TYPE_FORBIDDEN
				, int defaultTransferDelay = FORBIDDEN_TRANSFER_DELAY
				);

			virtual ~ConnectionPlace ();


			//! @name Getters/Setters
			//@{
				int							getDefaultTransferDelay () const;
				void						setDefaultTransferDelay (int defaultTransferDelay);

				int							getMinTransferDelay () const;
				int							getMaxTransferDelay () const;

				const PhysicalStops&		getPhysicalStops () const;

				const ConnectionType		getConnectionType () const;
				void						setConnectionType (const ConnectionType& connectionType);

			//@}


			//! @name Query methods.
			//@{
				/** Score getter.
					@return int the score of the place
					@author Hugues Romain
					@date 2007

					The vertex score is calculated by the following way :
						- each commercial line gives some points, depending of the number of services which belongs to the line :
							- 1 to 10 services lines gives 2 point
							- 10 to 50 services lines gives 3 points
							- 50 to 100 services lines gives 4 points
							- much than 100 services lines gives 5 points
						- if the score is bigger than 100 points, then the score is 100
				*/
				int getScore() const;

				bool isConnectionAllowed (const Vertex* fromVertex, 
							const Vertex* toVertex) const;

				ConnectionType getRecommendedConnectionType (const SquareDistance& squareDistance) const;


				int getTransferDelay (const Vertex* fromVertex, 
						const Vertex* toVertex) const;


				VertexAccess getVertexAccess (const AccessDirection& accessDirection,
							const AccessParameters& accessParameters,
							const Vertex* destination,
							const Vertex* origin = 0) const;
			    
				virtual void getImmediateVertices(
					VertexAccessMap& result
					, const AccessDirection& accessDirection
					, const AccessParameters& accessParameters
					, SearchAddresses returnAddresses
					, SearchPhysicalStops returnPhysicalStops
					, const Vertex* origin = NULL
				) const;

				virtual const geometry::Point2D& getPoint() const;

				virtual uid getId() const;

				std::vector<std::pair<uid, std::string> >	getPhysicalStopLabels(bool withAll = false) const;

				/** Labels list for select field containing physical stops, with exclusion list.
					@param noDisplay Physical stops to exclude
					@return Labels list for select field containing physical stops
					@author Hugues Romain
					@date 2007
				*/
				std::vector<std::pair<uid, std::string> >	getPhysicalStopLabels(const PhysicalStops& noDisplay) const;
			//@}


			//! @name Update methods.
			//@{
				void addPhysicalStop (const PhysicalStop* physicalStop);
				virtual void addAddress (const Address* address);
				void addTransferDelay (uid departureId, uid arrivalId, int transferDelay);
				void clearTransferDelays ();
			//@}

		};
	}
}

#endif 	    
