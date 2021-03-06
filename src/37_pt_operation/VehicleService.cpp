
/** VehicleService service class implementation.
	@file VehicleService.cpp

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

#include "VehicleService.hpp"

#include "ImportableTableSync.hpp"
#include "NumericField.hpp"
#include "ScheduledService.h"
#include "StringField.hpp"
#include "VehicleServiceTableSync.hpp"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace impex;
	using namespace pt;
	using namespace util;

	namespace util
	{
		template<>
		const std::string Registry<pt_operation::VehicleService>::KEY("VehicleService");
	}

	namespace pt_operation
	{
		const string VehicleService::TAG_SERVICE = "service";
		const string VehicleService::ATTR_CLASS = "class";
		const string VehicleService::VALUE_COMMERCIAL = "commercial";
		const string VehicleService::VALUE_DEAD_RUN = "deadRun";

		VehicleService::VehicleService(RegistryKeyType id):
			Registrable(id)
		{}



		SchedulesBasedService* VehicleService::getService(std::size_t rank) const
		{
			if(rank >= _services.size())
			{
				return NULL;
			}
			return _services[rank];
		}



		void VehicleService::insert(pt::SchedulesBasedService& value)
		{
			Services::iterator it(_services.begin());
			for(; it != _services.end(); ++it)
			{
				if(*it == &value)
				{
					return;
				}
				if((*it)->getDepartureSchedule(false, 0) > value.getDepartureSchedule(false,0))
				{
					break;
				}
			}
			if(it == _services.end())
			{
				_services.push_back(&value);
			}
			else
			{
				_services.insert(it, &value);
			}
		}



		void VehicleService::clearServices()
		{
			_services.clear();
		}



		void VehicleService::addDriverServiceChunk(
			const DriverService::Chunk& value
		){
			_driverServiceChunks.insert(&value);
		}



		void VehicleService::removeDriverServiceChunk(
			const DriverService::Chunk& value
		){
			_driverServiceChunks.erase(&value);
		}



		void VehicleService::toParametersMap(
			util::ParametersMap& map,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles /*= boost::logic::indeterminate*/,
			std::string prefix /*= std::string() */
		) const	{
			map.insert(Key::FIELD.name, getKey());
			map.insert(Name::FIELD.name, getName());

			// Services
			BOOST_FOREACH(const Services::value_type& service, _services)
			{
				boost::shared_ptr<ParametersMap> serviceMap(new ParametersMap);

				serviceMap->insert(ATTR_CLASS, dynamic_cast<ScheduledService*>(service) ? VALUE_COMMERCIAL : VALUE_DEAD_RUN);
				service->toParametersMap(*serviceMap, false);

				map.insert(TAG_SERVICE, serviceMap);
			}
		}



		bool VehicleService::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			// Name
			if(record.isDefined(VehicleServiceTableSync::COL_NAME))
			{
				string value(record.get<string>(VehicleServiceTableSync::COL_NAME));
				if(value != getName())
				{
					result = true;
					setName(value);
				}
			}

//			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				// Services
				if(record.isDefined(VehicleServiceTableSync::COL_SERVICES))
				{
					VehicleService::Services value(
						VehicleServiceTableSync::UnserializeServices(
							record.get<string>(VehicleServiceTableSync::COL_SERVICES),
							env
					)	);
					if(value != getServices())
					{
						result = true;
						setServices(value);
					}
				}
			}

			// Dates
			if(record.isDefined(VehicleServiceTableSync::COL_DATES))
			{
				Calendar value;
				value.setFromSerializedString(
					record.get<string>(VehicleServiceTableSync::COL_DATES)
				);
				if(value != *this)
				{
					result = true;
					setFromSerializedString(record.get<string>(VehicleServiceTableSync::COL_DATES));
				}
			}

			// Data source links (at the end of the load to avoid registration of objects which are removed later by an exception)
			if(record.isDefined(VehicleServiceTableSync::COL_DATASOURCE_LINKS))
			{
				Importable::DataSourceLinks value(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						record.get<string>(VehicleServiceTableSync::COL_DATASOURCE_LINKS),
						env
				)	);
				if(value != getDataSourceLinks())
				{
					result = true;
					setDataSourceLinksWithRegistration(value);
				}
			}

			return result;
		}



		void VehicleService::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{

		}



		synthese::LinkedObjectsIds VehicleService::getLinkedObjectsIds( const Record& record ) const
		{
			LinkedObjectsIds result;
			if(record.isDefined(VehicleServiceTableSync::COL_DATASOURCE_LINKS))
			{
				Env env;
				Importable::DataSourceLinks value(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						record.get<string>(VehicleServiceTableSync::COL_DATASOURCE_LINKS),
						env
				)	);
				BOOST_FOREACH(const Importable::DataSourceLinks::value_type& item, value)
				{
					result.push_back(item.first->getKey());
				}
			}
			return result;
		}



		bool VehicleService::DriverServiceChunkCompare::operator()(
			const DriverService::Chunk* ds1,
			const DriverService::Chunk* ds2
		) const	{
			if(ds1 == ds2)
			{
				return false;
			}
			if(ds1->elements.empty())
			{
				return false;
			}
			if(ds2->elements.empty())
			{
				return true;
			}

			const SchedulesBasedService& s1(*ds1->elements.begin()->service);
			const SchedulesBasedService& s2(*ds2->elements.begin()->service);

			const time_duration& t1(s1.getDepartureSchedule(false, 0));
			const time_duration& t2(s2.getDepartureSchedule(false, 0));

			if(t1 != t2)
			{
				return t1 < t2;
			}

			const time_duration& at1(s1.getLastArrivalSchedule(false));
			const time_duration& at2(s2.getLastArrivalSchedule(false));

			if(at1 != at2)
			{
				return at1 < at2;
			}

			return ds1 < ds2;
		}
}	}
