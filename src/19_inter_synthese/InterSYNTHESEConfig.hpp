
/** InterSYNTHESEConfig class header.
	@file InterSYNTHESEConfig.hpp

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

#ifndef SYNTHESE_cms_InterSYNTHESEConfig_hpp__
#define SYNTHESE_cms_InterSYNTHESEConfig_hpp__

#include "Object.hpp"

#include "StandardFields.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace inter_synthese
	{
		class InterSYNTHESEConfigItem;

		FIELD_TYPE(LinkBreakMinutes, boost::posix_time::time_duration)
		FIELD_TYPE(MaxQueriesNumber, size_t)
		
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(LinkBreakMinutes),
			FIELD(MaxQueriesNumber)
		> InterSYNTHESEConfigRecord;



		//////////////////////////////////////////////////////////////////////////
		/// InterSYNTHESEConfig class.
		///	@ingroup m19
		/// @author Hugues Romain
		/// @since 3.5.0
		class InterSYNTHESEConfig:
			public Object<InterSYNTHESEConfig, InterSYNTHESEConfigRecord>
		{
		public:
		
			/// Chosen registry class.
			typedef util::Registry<InterSYNTHESEConfig>	Registry;

			typedef std::vector<InterSYNTHESEConfigItem*> Items;

		private:
			Items _items;
		
		public:
			InterSYNTHESEConfig(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif // SYNTHESE_cms_InterSYNTHESEConfig_hpp__