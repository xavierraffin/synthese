
/** DriverAllocationTemplate class header.
	@file DriverAllocationTemplate.hpp

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

#ifndef SYNTHESE_37_DriverAllocationTemplate_hpp__
#define SYNTHESE_37_DriverAllocationTemplate_hpp__

#include "Object.hpp"

#include "NumericField.hpp"
#include "DataSourceLinksField.hpp"
#include "DateField.hpp"
#include "DriverService.hpp"
#include "ImportableTemplate.hpp"
#include "MinutesField.hpp"
#include "NumericField.hpp"

#include <vector>
#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace pt_operation
	{
		FIELD_DOUBLE(Amount)
		FIELD_DOUBLE(MaxBoniAmount)
		FIELD_MINUTES(MaxBoniTime)
		FIELD_MINUTES(WorkRange)
		FIELD_MINUTES(WorkDuration)
		FIELD_BOOL(WithTicketSales)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(DriverService::Vector),
			FIELD(Date),
			FIELD(Amount),
			FIELD(MaxBoniAmount),
			FIELD(MaxBoniTime),
			FIELD(impex::DataSourceLinks),
			FIELD(WorkRange),
			FIELD(WorkDuration),
			FIELD(WithTicketSales)
		> DriverAllocationTemplateSchema;

		/** DriverAllocation class.
			@ingroup m37
		*/
		class DriverAllocationTemplate:
			public Object<DriverAllocationTemplate, DriverAllocationTemplateSchema>,
			public impex::ImportableTemplate<DriverAllocationTemplate>
		{
		public:
			typedef util::Registry<DriverAllocationTemplate> Registry;

			DriverAllocationTemplate(util::RegistryKeyType id = 0);

			boost::posix_time::time_duration getWorkRange() const;
			boost::posix_time::time_duration getWorkDuration() const;
			boost::posix_time::time_duration getServiceBeginning() const;
			boost::posix_time::time_duration getServiceEnd() const;
		};
	}
}

#endif // SYNTHESE_37_DriverAllocationTemplate_hpp__
