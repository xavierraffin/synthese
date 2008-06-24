
/** TextTemplateTableSync class header.
	@file TextTemplateTableSync.h

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

#ifndef SYNTHESE_TextTemplateTableSync_H__
#define SYNTHESE_TextTemplateTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteNoSyncTableSyncTemplate.h"

#include "17_messages/Types.h"

#include "TextTemplate.h"

namespace synthese
{
	namespace messages
	{
		/** TextTemplate table synchronizer.
			@ingroup m17LS refLS
		*/
		class TextTemplateTableSync : public db::SQLiteNoSyncTableSyncTemplate<TextTemplateTableSync,TextTemplate>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_SHORT_TEXT;
			static const std::string COL_LONG_TEXT;
			static const std::string COL_LEVEL;
			static const std::string COL_IS_FOLDER;
			static const std::string COL_PARENT_ID;

			TextTemplateTableSync();


			/** TextTemplate search.
				(other search parameters)
				@param first First TextTemplate object to answer
				@param number Number of TextTemplate objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<TextTemplate*> Founded TextTemplate objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<TextTemplate> > Search(
				AlarmLevel level = ALARM_LEVEL_UNKNOWN
				, uid parentId = static_cast<uid>(UNKNOWN_VALUE)
				, bool isFolder = false
				, std::string name = std::string()
				, const TextTemplate* templateToBeDifferentWith = NULL
				, int first = 0
				, int number = -1
				, bool orderByName = true
				, bool orderByShortText = false
				, bool orderByLongText = false
				, bool raisingOrder = true
				);
		};
	}
}

#endif // SYNTHESE_TextTemplateTableSync_H__
