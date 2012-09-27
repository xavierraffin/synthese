
/** VDVClient class header.
	@file VDVClient.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_data_exchange_VDVClient_hpp__
#define SYNTHESE_data_exchange_VDVClient_hpp__

#include "Object.hpp"

#include "DataSource.h"
#include "StandardFields.hpp"

#include "VDVServer.hpp" // For DataSourcePointer (remove after refactoring)

#include <map>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace data_exchange
	{
		class VDVClientSubscription;

		FIELD_TYPE(ReplyAddress, std::string)
		FIELD_TYPE(ReplyPort, std::string)
		FIELD_TYPE(Active, bool)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(ReplyAddress),
			FIELD(ReplyPort),
			FIELD(ClientControlCentreCode),
			FIELD(ServerControlCentreCode),
			FIELD(ServiceCode),
			FIELD(DataSourcePointer),
			FIELD(Active)
		> VDVClientRecord;

		/** VDV client.
			@ingroup m36
		*/
		class VDVClient:
			public Object<VDVClient, VDVClientRecord>
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<VDVClient>	Registry;

			typedef std::map<std::string, boost::shared_ptr<VDVClientSubscription> > Subscriptions;
			
		private:
			Subscriptions _subscriptions;

			std::string _getURL(const std::string& request) const;

		public:
			VDVClient(util::RegistryKeyType id = 0);

			void addSubscription(boost::shared_ptr<VDVClientSubscription> subscription);
			const Subscriptions& getSubscriptions() const { return _subscriptions; }
			void removeSubscription(const std::string& key);
			void cleanSubscriptions();

			bool checkUpdate() const;

			void sendUpdateSignal() const;

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif // SYNTHESE_data_exchange_VDVClient_hpp__
