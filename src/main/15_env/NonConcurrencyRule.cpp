
/** NonConcurrencyRule class implementation.
	@file NonConcurrencyRule.cpp

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

#include "NonConcurrencyRule.h"

namespace synthese
{
	namespace env
	{


		NonConcurrencyRule::NonConcurrencyRule( const CommercialLine* priorityLine /*= NULL */, const CommercialLine* hiddenLine /*= NULL */, int delay /*= UNKNOWN_VALUE  */ )
			: Registrable<uid, NonConcurrencyRule>()
			, _prorityLine(priorityLine)
			, _hiddenLine(hiddenLine)
			, _delay(delay)
		{
		}

		const CommercialLine* NonConcurrencyRule::getPriorityLine() const
		{
			return _prorityLine;
		}

		const CommercialLine* NonConcurrencyRule::getHiddenLine() const
		{
			return _hiddenLine;
		}

		int NonConcurrencyRule::getDelay() const
		{
			return _delay;
		}

		void NonConcurrencyRule::setPriorityLine( const CommercialLine* value )
		{
			_prorityLine = value;
		}

		void NonConcurrencyRule::setHiddenLine( const CommercialLine* value )
		{
			_hiddenLine = value;
		}

		void NonConcurrencyRule::setDelay( int value )
		{
			_delay = value;
		}
	}
}
