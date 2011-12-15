
//////////////////////////////////////////////////////////////////////////
///	FreeDRTAreaTableSync class header.
///	@file FreeDRTAreaTableSync.hpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_FreeDRTAreaTableSync_hpp__
#define SYNTHESE_FreeDRTAreaTableSync_hpp__

#include "FreeDRTArea.hpp"
#include "DBRegistryTableSyncTemplate.hpp"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		///	FreeDRTArea table synchronizer.
		///	@ingroup m35LS refLS
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.3.1
		class FreeDRTAreaTableSync:
			public db::DBRegistryTableSyncTemplate<FreeDRTAreaTableSync,FreeDRTArea>
		{
		public:
			//! @name Field names
			//@{
				static const std::string COL_COMMERCIAL_LINE_ID;
				static const std::string COL_TRANSPORT_MODE_ID;
				static const std::string COL_NAME;
				static const std::string COL_CITIES;
				static const std::string COL_USE_RULES;
			//@}
			

			//! @name Services
			//@{		
				//////////////////////////////////////////////////////////////////////////
				///	FreeDRTArea search.
				///	@param env Environment to populate
				///	@param lineId optional ID of the line
				///	@param first First  object to answer
				///	@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				///	@param orderByName The result records must be ordered by their name
				///	@param raisingOrder The result records must be sorted ascendantly
				///	@param linkLevel Level of links to build when reading foreign keys
				///	@return Found objects.
				///	@author Hugues Romain
				///	@date 2011
				/// @since 3.3.1
				static SearchResult Search(
					util::Env& env,
					boost::optional<util::RegistryKeyType> lineId = boost::optional<util::RegistryKeyType>(),
					std::size_t first = 0,
					boost::optional<std::size_t> number = boost::optional<std::size_t>(),
					bool orderByName = true,
					bool raisingOrder = true,
					util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
				);


				static FreeDRTArea::Cities UnserializeCities(
					const std::string& value,
					util::Env& env,
					util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
				);
				static std::string SerializeCities(
					const FreeDRTArea::Cities& value
				);
			//@}
		};
	}
}

#endif // SYNTHESE_FreeDRTAreaTableSync_hpp__

