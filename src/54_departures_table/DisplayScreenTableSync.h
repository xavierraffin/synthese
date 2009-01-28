////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenTableSync class header.
///	@file DisplayScreenTableSync.h
///	@author Hugues Romain
///	@date 2008-12-21 21:17
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_DisplayScreenTableSync_H__
#define SYNTHESE_DisplayScreenTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include "DisplayScreen.h"
#include "01_util/Constants.h"
#include "01_util/UId.h"
#include "SQLiteRegistryTableSyncTemplate.h"
#include "12_security/Types.h"

namespace synthese
{
	namespace messages
	{
		class SentAlarm;
	}

	namespace departurestable
	{
		/** DisplayScreen table synchronizer.
			@ingroup m54LS refLS
		*/
		class DisplayScreenTableSync : public db::SQLiteRegistryTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>
		{
		public:
			//! \name Columns
			//@{
				static const std::string COL_PLACE_ID;
				static const std::string COL_NAME;
				static const std::string COL_TYPE_ID;
				static const std::string COL_WIRING_CODE;
				static const std::string COL_TITLE;
				static const std::string COL_BLINKING_DELAY;
				static const std::string COL_TRACK_NUMBER_DISPLAY;
				static const std::string COL_SERVICE_NUMBER_DISPLAY;
				static const std::string COL_PHYSICAL_STOPS_IDS;	//!< List of physical stops uids, separated by comas
				static const std::string COL_ALL_PHYSICAL_DISPLAYED;
				static const std::string COL_FORBIDDEN_ARRIVAL_PLACES_IDS;	//!< List of forbidden connection places uids, separated by comas
				static const std::string COL_FORBIDDEN_LINES_IDS;	//!< List of forbidden lines uids, separated by comas
				static const std::string COL_DIRECTION;
				static const std::string COL_ORIGINS_ONLY;
				static const std::string COL_DISPLAYED_PLACES_IDS;	//!< List of displayed places uids, separated by comas
				static const std::string COL_MAX_DELAY;
				static const std::string COL_CLEARING_DELAY;
				static const std::string COL_FIRST_ROW;
				static const std::string COL_GENERATION_METHOD;
				static const std::string COL_FORCED_DESTINATIONS_IDS;	//!< List of forced destination uids in preselection, separated by comas
				static const std::string COL_DESTINATION_FORCE_DELAY;
				static const std::string COL_MAINTENANCE_CHECKS_PER_DAY;
				static const std::string COL_MAINTENANCE_IS_ONLINE;
				static const std::string COL_MAINTENANCE_MESSAGE;
				static const std::string COL_DISPLAY_TEAM;
				static const std::string COL_DISPLAY_CLOCK;
				static const std::string COL_COM_PORT;
				static const std::string COL_CPU_HOST_ID;
			//@}
			


			DisplayScreenTableSync();


			/** DisplayScreen search.
				(other search parameters)
				@param first First DisplayScreen object to answer
				@param number Number of DisplayScreen objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<DisplayScreen*> Founded DisplayScreen objects.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				const security::RightsOfSameClassMap& rights 
				, bool totalControl 
				, security::RightLevel neededLevel
				, uid duid = UNKNOWN_VALUE
				, uid localizationid = UNKNOWN_VALUE
				, uid lineid = UNKNOWN_VALUE
				, uid typeuid = UNKNOWN_VALUE
				, std::string cityName = std::string()
				, std::string stopName = std::string()
				, std::string name = std::string()
				, int state = UNKNOWN_VALUE
				, int message = UNKNOWN_VALUE
				, int first = 0
				, int number = 0
				, bool orderByUid = false
				, bool orderByCity = true
				, bool orderByStopName = false
				, bool orderByName = false
				, bool orderByType = false
				, bool orderByStatus = false
				, bool orderByMessage = false
				, bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			////////////////////////////////////////////////////////////////////
			///	Gets the message currently displayed on a screen.
			/// @param env Environment to populate
			///	@param screenId id of the screen
			/// @param number Number of results (<=0 : unlimited)
			///	@return pointer to the message
			///	@author Hugues Romain
			///	@date 2008
			/// @warning The message is returned even if the screen is deactivated.
			/// If there is no message, a null pointer is returned.
			////////////////////////////////////////////////////////////////////
			static std::vector<boost::shared_ptr<messages::SentAlarm> > GetCurrentDisplayedMessage(
				util::Env& env,
				util::RegistryKeyType screenId,
				int number = UNKNOWN_VALUE
			);


			
			////////////////////////////////////////////////////////////////////
			///	Tests if at least a line is displayed on the screen.
			///	@param screenId id of the screen
			///	@return bool true if at least a line is displayed on the screen
			///	@author Hugues Romain
			///	@date 2008
			/// @warning This function does not verify if the line has any
			/// active service
			/// @todo work only for departures table : rewrite inverted query
			////////////////////////////////////////////////////////////////////
			static bool GetIsAtLeastALineDisplayed(
				util::RegistryKeyType screenId
			);
		};
	}
}

#endif // SYNTHESE_DisplayScreenTableSync_H__
