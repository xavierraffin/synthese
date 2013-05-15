
/** HastusCSVFileFormat class header.
	@file HastusCSVFileFormat.hpp

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

#ifndef SYNTHESE_HastusCSVFileFormat_H__
#define SYNTHESE_HastusCSVFileFormat_H__

#include "FileFormatTemplate.h"
#include "Calendar.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "ScheduledService.h"
#include "JourneyPattern.hpp"
#include "RollingStock.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}

	namespace pt
	{
		class PTUseRule;
		class TransportNetwork;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Hastus CSV file format.
		/// This format was created by Tisséo in order to be read by SYNTHESE.
		/// The files are generated by an OIG script.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m61File refFile
		class HastusCSVFileFormat:
			public impex::FileFormatTemplate<HastusCSVFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<HastusCSVFileFormat>,
				public PTDataCleanerFileFormat
			{
			public:
				static const std::string FILE_ARRETS;
				static const std::string FILE_ITINERAIRES;
				static const std::string FILE_VOYAGES;

				static const std::string PARAMETER_NETWORK_ID;
				static const std::string PARAMETER_ROLLING_STOCK_ID_A;
				static const std::string PARAMETER_ROLLING_STOCK_ID_T;
				static const std::string PARAMETER_ROLLING_STOCK_ID_DEFAULT;
				static const std::string PARAMETER_IMPORT_STOP_AREA;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION;
				static const std::string PARAMETER_DISPLAY_LINKED_STOPS;
				static const std::string PARAMETER_HANDICAPPED_ALLOWED_USE_RULE;
				static const std::string PARAMETER_HANDICAPPED_FORBIDDEN_USE_RULE;

			private:
				typedef std::map<std::string, boost::shared_ptr<pt::RollingStock> > RollingStockMap;
				static const std::string SEP;

				boost::shared_ptr<const pt::TransportNetwork> _network;
				RollingStockMap _rollingStocks;
				boost::shared_ptr<pt::RollingStock> _defaultRollingStock;
				bool _importStopArea;
				bool _interactive;
				bool _displayLinkedStops;
				boost::posix_time::time_duration _stopAreaDefaultTransferDuration;
				boost::shared_ptr<pt::PTUseRule> _handicappedAllowedUseRule;
				boost::shared_ptr<pt::PTUseRule> _handicappedForbiddenUseRule;

				mutable std::vector<std::string> _line;
				std::string _getValue(std::size_t rank) const;
				void _loadLine(const std::string& line) const;

				//////////////////////////////////////////////////////////////////////////
				/// Temporary storage of a trip.
				/// @author Hugues Romain
				struct TripIndex
				{
					std::string lineCode;
					std::string routeCode;
					std::string code;
					std::string team;
					bool handicapped;

					bool operator <(const TripIndex& other) const;
				};

				struct TripValues
				{
					std::string routeName;
					bool wayBack;
					pt::ScheduledService::Schedules schedules;
					pt::JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
					calendar::Calendar calendar;
					pt::RollingStock* rollingStock;
				};
				typedef std::map<TripIndex, TripValues> Trips;
				mutable Trips _trips;
				typedef std::map<std::string, std::set<TripIndex> > TripsByCode;
				mutable TripsByCode _tripsByCode;

				mutable impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync> _stopPoints;

			protected:

				virtual bool _checkPathsMap() const;

				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					const std::string& key,
					boost::optional<const server::Request&> adminRequest
				) const;


			public:
				Importer_(
					util::Env& env,
					const impex::DataSource& dataSource
				);

				//////////////////////////////////////////////////////////////////////////
				/// Import screen to include in the administration console.
				/// @param os stream to write the result on
				/// @param request request for display of the administration console
				/// @since 3.2.0
				/// @date 2010
				virtual void displayAdmin(
					std::ostream& os,
					const server::Request& request
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.2.1
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.2.1
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<HastusCSVFileFormat> Exporter_;
		};
	}
}

#endif
