
/** TreeFolderDownNode class header.
	@file TreeFolderDownNode.hpp

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

#ifndef SYNTHESE_tree_TreeFolderDownNode_hpp__
#define SYNTHESE_tree_TreeFolderDownNode_hpp__

#include "TreeFolderDownNodeInterface.hpp"

namespace synthese
{
	namespace tree
	{
		/** TreeFolderDownNode class.
			@ingroup m20
		*/
		template<
			class RootType_
		>
		class TreeFolderDownNode:
			public TreeFolderDownNodeInterface
		{
		public:
			typedef RootType_ RootType;

			void setParent(RootType& value)
			{
				_setParent(value);
			}

			void setParent(TreeFolder& value)
			{
				_setParent(value);
			}

			RootType* getRoot() const
			{
				return static_cast<RootType*>(_getRoot());
			}
		};
}	}

#endif // SYNTHESE_tree_TreeFolderDownNode_hpp__

