
/** UpdateDisplayTypeAction class header.
	@file UpdateDisplayTypeAction.h

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

#ifndef SYNTHESE_UpdateDisplayTypeAction_H__
#define SYNTHESE_UpdateDisplayTypeAction_H__

#include "FactorableTemplate.h"
#include "Action.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace interfaces
	{
		class Interface;
	}

	namespace departure_boards
	{
		class DisplayType;

		/** UpdateDisplayTypeAction action class.
			@ingroup m54Actions refActions
		*/
		class UpdateDisplayTypeAction : public util::FactorableTemplate<server::Action, UpdateDisplayTypeAction>
		{
		public:
			static const std::string PARAMETER_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_INTERFACE_ID;
			static const std::string PARAMETER_AUDIO_INTERFACE_ID;
			static const std::string PARAMETER_MONITORING_INTERFACE_ID;
			static const std::string PARAMETER_ROWS_NUMBER;
			static const std::string PARAMETER_MAX_STOPS_NUMBER;
			static const std::string PARAMETER_TIME_BETWEEN_CHECKS;
			static const std::string PARAMETER_DISPLAY_MAIN_PAGE_ID;
			static const std::string PARAMETER_DISPLAY_ROW_PAGE_ID;
			static const std::string PARAMETER_DISPLAY_DESTINATION_PAGE_ID;
			static const std::string PARAMETER_DISPLAY_TRANSFER_DESTINATION_PAGE_ID;

		private:
			boost::shared_ptr<DisplayType> _dt;
			std::string _name;
			boost::shared_ptr<const interfaces::Interface>	_interface;
			boost::shared_ptr<const interfaces::Interface>	_monitoringInterface;
			boost::shared_ptr<const interfaces::Interface>	_audioInterface;
			int	_rows_number;
			int	_max_stops_number;
			boost::posix_time::time_duration	_timeBetweenChecks;
			boost::shared_ptr<const cms::Webpage> _displayMainPage;
			boost::shared_ptr<const cms::Webpage> _displayRowPage;
			boost::shared_ptr<const cms::Webpage> _displayDestinationPage;
			boost::shared_ptr<const cms::Webpage> _displayTransferDestinationPage;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);


			void setTypeId(util::RegistryKeyType id);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_UpdateDisplayTypeAction_H__
