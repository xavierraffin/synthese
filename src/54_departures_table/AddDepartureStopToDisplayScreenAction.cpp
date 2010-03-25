
/** AddDepartureStopToDisplayScreenAction class implementation.
	@file AddDepartureStopToDisplayScreenAction.cpp

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

#include "AddDepartureStopToDisplayScreenAction.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "ArrivalDepartureTableRight.h"
#include "DisplayScreenTableSync.h"
#include "PhysicalStopTableSync.h"
#include "Conversion.h"
#include "ArrivalDepartureTableLog.h"
#include "PhysicalStop.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace env;
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<Action, departurestable::AddDepartureStopToDisplayScreenAction>::FACTORY_KEY("adstdsa");
	}
	
	namespace departurestable
	{
		const string AddDepartureStopToDisplayScreenAction::PARAMETER_SCREEN_ID = Action_PARAMETER_PREFIX + "sc";
		const string AddDepartureStopToDisplayScreenAction::PARAMETER_STOP = Action_PARAMETER_PREFIX + "sto";


		ParametersMap AddDepartureStopToDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			if(_stop.get())
			{
				map.insert(PARAMETER_STOP, _stop->getKey());
			}
			if(_screen.get()) map.insert(PARAMETER_SCREEN_ID, _screen->getKey());
			return map;
		}

		void AddDepartureStopToDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_SCREEN_ID),
					*_env
				);
				setStopId(map.get<RegistryKeyType>(PARAMETER_STOP));
			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Display screen not found");
			}
		}



		void AddDepartureStopToDisplayScreenAction::run(Request& request)
		{
			// Preparation
			_screen->addPhysicalStop(_stop.get());
			
			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				"Ajout de l'arr�t de d�part "+ _stop->getCodeBySource() +"/"+ _stop->getName(),
				*request.getUser()
			);
			
			// Action
			DisplayScreenTableSync::Save(_screen.get());
		}



		bool AddDepartureStopToDisplayScreenAction::isAuthorized(const Session* session
		) const {
			assert(_screen.get() != NULL);
			if (_screen->getLocalization() != NULL)
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_screen->getLocalization()->getKey()));
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE);
			}
		}
		
		void AddDepartureStopToDisplayScreenAction::setStopId(
			RegistryKeyType id
		){
			try
			{
				_stop = PhysicalStopTableSync::Get(id, *_env, UP_LINKS_LOAD_LEVEL);
			}
			catch (ObjectNotFoundException<PhysicalStop>& e)
			{
				throw ActionException("Departure physical stop", e, *this);
			}
		}
		
		void AddDepartureStopToDisplayScreenAction::setScreen(
			boost::shared_ptr<DisplayScreen> screen
		){
			_screen = screen;
		}
	
		void AddDepartureStopToDisplayScreenAction::setScreen(
			boost::shared_ptr<const DisplayScreen> screen
		){
			_screen = const_pointer_cast<DisplayScreen>(screen);
		}
	}
}
