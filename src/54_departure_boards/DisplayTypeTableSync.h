
/** DisplayTypeTableSync class header.
	@file DisplayTypeTableSync.h

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

#ifndef SYNTHESE_DisplayTypeTableSync_H__
#define SYNTHESE_DisplayTypeTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "DisplayType.h"

#include "DBDirectTableSyncTemplate.hpp"
#include "OldLoadSavePolicy.hpp"

namespace synthese
{
	namespace departure_boards
	{
		class DisplayType;

		//////////////////////////////////////////////////////////////////////////
		/// 54.10 Table : Display types.
		///	@ingroup m54LS refLS
		/// @author Hugues Romain
		class DisplayTypeTableSync:
			public db::DBDirectTableSyncTemplate<
				DisplayTypeTableSync,
				DisplayType,
				db::FullSynchronizationPolicy,
				db::OldLoadSavePolicy
			>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_DISPLAY_INTERFACE_ID;
			static const std::string COL_AUDIO_INTERFACE_ID;
			static const std::string COL_MONITORING_INTERFACE_ID;
			static const std::string COL_ROWS_NUMBER;
			static const std::string COL_MAX_STOPS_NUMBER;
			static const std::string COL_TIME_BETWEEN_CHECKS;
			static const std::string COL_DISPLAY_MAIN_PAGE_ID;
			static const std::string COL_DISPLAY_ROW_PAGE_ID;
			static const std::string COL_DISPLAY_DESTINATION_PAGE_ID;
			static const std::string COL_DISPLAY_TRANSFER_DESTINATION_PAGE_ID;
			static const std::string COL_MONITORING_PARSER_PAGE_ID;
			static const std::string COL_IS_DISPLAYED_MESSAGE_PAGE_ID;
			static const std::string COL_MESSAGE_TYPE;


			////////////////////////////////////////////////////////////////////
			/// DisplayType search.
			///	@param env Environment to populate
			/// @param nameLike SQL LIKE mask to filter on display type name (do
			///		not forget the % command) (default/% = deactivate the filter)
			/// @param interfaceId id of display interface to use as filter
			///		(default = deactivate the filter)
			/// @param first First user to answer
			///	@param number Number of users to answer (default = all)
			///		The size of the result registry is less or equal to number, then all
			///		users were returned despite of the number limit. If the size
			///		is greater than number (actually equal to number + 1) then
			///		there is others accounts to show. Test it to know if the
			///		situation needs a "click for more" button.
			/// @param orderByName Order the results alphabetically by the name
			/// @param orderByInterfaceName Order the results alphabetically by
			///		the name of the interface, then by the name of the type
			/// @param orderByRows Order the result by the rows number
			///	@param raisingOrder Ascending or descending order
			/// @param linkLevel Level of link of the objects stored in the result
			///		registry
			/// @throws Exception if the load of objects failed
			///	@author Hugues Romain
			///	@date 2006
			////////////////////////////////////////////////////////////////////
			static SearchResult Search(
				util::Env& env,
				boost::optional<std::string> nameLike = boost::optional<std::string>(),
				boost::optional<util::RegistryKeyType> interfaceId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByName = true,
				bool orderByInterfaceName = false,
				bool orderByRows = false,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_DisplayTypeTableSync_H__
