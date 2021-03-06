
/** JourneyPatternTableSync class implementation.
	@file JourneyPatternTableSync.cpp

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

#include "JourneyPatternTableSync.hpp"

#include "DataSourceLinksField.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "User.h"
#include "GraphConstants.h"
#include "CommercialLineTableSync.h"
#include "FareTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "DataSourceTableSync.h"
#include "PTUseRuleTableSync.h"
#include "PTUseRule.h"
#include "ImportableTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "LineStopTableSync.h"
#include "TransportNetworkRight.h"
#include "DestinationTableSync.hpp"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace fare;
	using namespace util;
	using namespace impex;
	using namespace graph;
	using namespace pt;
	using namespace security;
	using namespace vehicle;

	template<> const string util::FactorableTemplate<DBTableSync,JourneyPatternTableSync>::FACTORY_KEY(
		"35.30.01 Journey patterns"
	);

	namespace pt
	{
		const string JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID = "commercial_line_id";
		const string JourneyPatternTableSync::COL_NAME ("name");
		const string JourneyPatternTableSync::COL_TIMETABLENAME ("timetable_name");
		const string JourneyPatternTableSync::COL_DIRECTION ("direction");
		const string JourneyPatternTableSync::COL_DIRECTION_ID("direction_id");
		const string JourneyPatternTableSync::COL_ISWALKINGLINE ("is_walking_line");
		const string JourneyPatternTableSync::COL_ROLLINGSTOCKID ("rolling_stock_id");
		const string JourneyPatternTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const string JourneyPatternTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const string JourneyPatternTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
		const string JourneyPatternTableSync::COL_WAYBACK("wayback");
		const string JourneyPatternTableSync::COL_DATASOURCE_ID("data_source");
		const string JourneyPatternTableSync::COL_MAIN("main");
		const string JourneyPatternTableSync::COL_PLANNED_LENGTH = "planned_length";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<JourneyPatternTableSync>::TABLE(
			"t009_lines"
		);
		template<> const Field DBTableSyncTemplate<JourneyPatternTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID, SQL_INTEGER),
			Field(JourneyPatternTableSync::COL_NAME, SQL_TEXT),
			Field(JourneyPatternTableSync::COL_TIMETABLENAME, SQL_TEXT),
			Field(JourneyPatternTableSync::COL_DIRECTION, SQL_TEXT),
			Field(JourneyPatternTableSync::COL_DIRECTION_ID, SQL_INTEGER),
			Field(JourneyPatternTableSync::COL_ISWALKINGLINE, SQL_BOOLEAN),
			Field(JourneyPatternTableSync::COL_ROLLINGSTOCKID, SQL_INTEGER),
			Field(JourneyPatternTableSync::COL_BIKECOMPLIANCEID, SQL_INTEGER),
			Field(JourneyPatternTableSync::COL_HANDICAPPEDCOMPLIANCEID, SQL_INTEGER),
			Field(JourneyPatternTableSync::COL_PEDESTRIANCOMPLIANCEID, SQL_INTEGER),
			Field(JourneyPatternTableSync::COL_WAYBACK, SQL_INTEGER),
			Field(JourneyPatternTableSync::COL_DATASOURCE_ID, SQL_TEXT),
			Field(JourneyPatternTableSync::COL_MAIN, SQL_BOOLEAN),
			Field(JourneyPatternTableSync::COL_PLANNED_LENGTH, SQL_DOUBLE),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<JourneyPatternTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID.c_str(), ""));
			r.push_back(DBTableSync::Index(JourneyPatternTableSync::COL_DATASOURCE_ID.c_str(), ""));
			return r;
		}


		template<>
		void OldLoadSavePolicy<JourneyPatternTableSync,JourneyPattern>::Load(
			JourneyPattern* line,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			DBModule::LoadObjects(line->getLinkedObjectsIds(*rows), env, linkLevel);
			line->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				line->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<>
		void OldLoadSavePolicy<JourneyPatternTableSync,JourneyPattern>::Save(
			JourneyPattern* object,
			optional<DBTransaction&> transaction
		){
			if(!object->getCommercialLine()) throw Exception("JourneyPattern save error. Missing commercial line");
			ReplaceQuery<JourneyPatternTableSync> query(*object);
			query.addField(object->getCommercialLine()->getKey());
			query.addField(object->getName());
			query.addField(object->getTimetableName());
			query.addField(object->getDirection());
			query.addField(object->getDirectionObj() ? object->getDirectionObj()->getKey() : RegistryKeyType(0));
			query.addField(object->getWalkingLine());
			query.addField(object->getRollingStock() ? object->getRollingStock()->getKey() : RegistryKeyType(0));
			query.addField(
				object->getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(object->getRule(USER_BIKE)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_BIKE))->getKey() : RegistryKeyType(0)
			);
			query.addField(
				object->getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED))->getKey() : RegistryKeyType(0)
			);
			query.addField(
				object->getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN))->getKey() : RegistryKeyType(0)
			);
			query.addField(object->getWayBack());
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);
			query.addField(object->getMain());
			query.addField(object->getPlannedLength());
			query.execute(transaction);
		}



		template<>
		void OldLoadSavePolicy<JourneyPatternTableSync,JourneyPattern>::Unlink(JourneyPattern* obj)
		{
			if(obj->getCommercialLine())
			{
				const_cast<CommercialLine*>(obj->getCommercialLine())->removePath(obj);
			}
		}



		template<> bool DBTableSyncTemplate<JourneyPatternTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the user has sufficient right level for this journey pattern
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, string());
		}



		template<> void DBTableSyncTemplate<JourneyPatternTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			// Services deletion
			Env env;
			ScheduledServiceTableSync::SearchResult sservices(ScheduledServiceTableSync::Search(env, id));
			BOOST_FOREACH(const ScheduledServiceTableSync::SearchResult::value_type& sservice, sservices)
			{
				ScheduledServiceTableSync::Remove(NULL, sservice->getKey(), transaction, false);
			}

			ContinuousServiceTableSync::SearchResult cservices(ContinuousServiceTableSync::Search(env, id));
			BOOST_FOREACH(const ContinuousServiceTableSync::SearchResult::value_type& cservice, cservices)
			{
				ContinuousServiceTableSync::Remove(NULL, cservice->getKey(), transaction, false);
			}

			// LineStops deletion
			LineStopTableSync::SearchResult edges(LineStopTableSync::Search(env, id));
			BOOST_FOREACH(const LineStopTableSync::SearchResult::value_type& edge, edges)
			{
				DBModule::GetDB()->deleteStmt(edge->getKey(), transaction);
			}
		}



		template<> void DBTableSyncTemplate<JourneyPatternTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<JourneyPatternTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		JourneyPatternTableSync::SearchResult JourneyPatternTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> commercialLineId,
			int first, /*= 0*/
			boost::optional<std::size_t> number,
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel,
			boost::optional<bool> wayback
		){
			SelectQuery<JourneyPatternTableSync> query;
			if (commercialLineId)
			{
				query.addWhereField(COL_COMMERCIAL_LINE_ID, *commercialLineId);
			}
			if(wayback)
			{
				query.addWhereField(COL_WAYBACK, *wayback);
			}
			if (orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}
			return LoadFromQuery(query, env, linkLevel);
		}
}	}
