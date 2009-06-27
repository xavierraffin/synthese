
/** CancelReservationAction class header.
	@file CancelReservationAction.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_CancelReservationAction_H__
#define SYNTHESE_CancelReservationAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace resa
	{
		class ReservationTransaction;

		/** CancelReservationAction action class.
			@ingroup m31Actions refActions
		*/
		class CancelReservationAction
			: public util::FactorableTemplate<server::Action, CancelReservationAction>
		{
		public:
			static const std::string PARAMETER_RESERVATION_TRANSACTION_ID;
			static const std::string PARAMETER_PASSWORD;

		private:
			boost::shared_ptr<ReservationTransaction>	_transaction;
			
		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run();
			
			CancelReservationAction();

			void setTransaction(boost::shared_ptr<ReservationTransaction> transaction);

			virtual bool _isAuthorized() const;
		};
	}
}

#endif // SYNTHESE_CancelReservationAction_H__
