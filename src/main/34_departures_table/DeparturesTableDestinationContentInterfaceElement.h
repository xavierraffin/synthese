
/** DeparturesTableDestinationContentInterfaceElement class header.
	@file DeparturesTableDestinationContentInterfaceElement.h

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

#ifndef SYNTHESE_DeparturesTableDestinationContentInterfaceElement_H__
#define SYNTHESE_DeparturesTableDestinationContentInterfaceElement_H__


#include "11_interfaces/LibraryInterfaceElement.h"

#include <string>

namespace synthese
{
	namespace departurestable
	{
		/** Departure Table Destination Content Interface Element Class.
			@ingroup m34Library refLibrary

			Parameters :
				- 0 : destinationsToDisplay
				- 1 : displayTerminus
				- 2 : Name to display :
					- char(13) : 13 chars name if exists, connection place name truncated else
					- char(26) : 26 chars name if exists, connection place name truncated else
					- else : connection place name
				- 3 : stopsSeparator
				- 4 : Display city name
					- station_city : adds the city name before the connection place name
					- station_city_if_new : adds the city name before the connection place name only at city changes
					- else : do not add the city name before the connection place name
				- 5 : beforeCity
				- 6 : afterCity
			
			Object : ArrivalDepartureRow object

		*/
		class DeparturesTableDestinationContentInterfaceElement : public interfaces::LibraryInterfaceElement
		{
		public:
			static const std::string DESTINATIONS_TO_DISPLAY_ALL;
			static const std::string DESTINATIONS_TO_DISPLAY_TERMINUS;
			static const std::string TYPE_STATION_CITY;
			static const std::string TYPE_STATION_CITY_IF_NEW;
			static const std::string TYPE_STATION;
			static const std::string TYPE_CHAR_13;
			static const std::string TYPE_CHAR_26;
			

		private:
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _destinationsToDisplayVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayTerminusVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayTypeVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _stopsSeparatorVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayCityVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _beforeCityVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _afterCityVIE;

			void displayDestination(std::ostream& stream, bool displayCity) const;

		public:
			void storeParameters(interfaces::ValueElementList& vel);
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL, const server::Request* request = NULL) const;
			const std::string getLabel(const interfaces::ParametersVector& parameters) const;
		};

	}
}

#endif // SYNTHESE_DeparturesTableDestinationContentInterfaceElement_H__
