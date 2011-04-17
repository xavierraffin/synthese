
/** PTUseRulesTableSync class implementation.
	@file PTUseRulesTableSync.cpp

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

#include "PTUseRuleTableSync.h"
#include "PTUseRule.h"
#include "FareTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "TransportNetworkRight.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync,PTUseRuleTableSync>::FACTORY_KEY("35.10.06 Public transportation use rules");

	namespace pt
	{
		const string PTUseRuleTableSync::COL_RESERVATION_TYPE("reservation_type");
		const string PTUseRuleTableSync::COL_CAPACITY("capacity");
		const string PTUseRuleTableSync::COL_ORIGINISREFERENCE ("origin_is_reference");
		const string PTUseRuleTableSync::COL_MINDELAYMINUTES ("min_delay_minutes");
		const string PTUseRuleTableSync::COL_MINDELAYDAYS ("min_delay_days");
		const string PTUseRuleTableSync::COL_MAXDELAYDAYS ("max_delay_days");
		const string PTUseRuleTableSync::COL_HOURDEADLINE ("hour_deadline");
		const string PTUseRuleTableSync::COL_NAME("name");
		const string PTUseRuleTableSync::COL_DEFAULT_FARE("default_fare_id");
		const string PTUseRuleTableSync::COL_FORBIDDEN_IN_DEPARTURE_BOARDS("forbidden_in_departure_boards");
		const string PTUseRuleTableSync::COL_FORBIDDEN_IN_TIMETABLES("forbidden_in_timetables");
		const string PTUseRuleTableSync::COL_FORBIDDEN_IN_JOURNEY_PLANNING("forbidden_in_journey_planning");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<PTUseRuleTableSync>::TABLE(
			"t061_pt_use_rules"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<PTUseRuleTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(PTUseRuleTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(PTUseRuleTableSync::COL_CAPACITY, SQL_TEXT),
			DBTableSync::Field(PTUseRuleTableSync::COL_RESERVATION_TYPE, SQL_INTEGER),
			DBTableSync::Field(PTUseRuleTableSync::COL_ORIGINISREFERENCE, SQL_BOOLEAN),
			DBTableSync::Field(PTUseRuleTableSync::COL_MINDELAYMINUTES, SQL_INTEGER),
			DBTableSync::Field(PTUseRuleTableSync::COL_MINDELAYDAYS, SQL_INTEGER),
			DBTableSync::Field(PTUseRuleTableSync::COL_MAXDELAYDAYS, SQL_INTEGER),
			DBTableSync::Field(PTUseRuleTableSync::COL_HOURDEADLINE, SQL_TIME),
			DBTableSync::Field(PTUseRuleTableSync::COL_DEFAULT_FARE, SQL_INTEGER),
			DBTableSync::Field(PTUseRuleTableSync::COL_FORBIDDEN_IN_DEPARTURE_BOARDS, SQL_BOOLEAN),
			DBTableSync::Field(PTUseRuleTableSync::COL_FORBIDDEN_IN_TIMETABLES, SQL_BOOLEAN),
			DBTableSync::Field(PTUseRuleTableSync::COL_FORBIDDEN_IN_JOURNEY_PLANNING, SQL_BOOLEAN),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<PTUseRuleTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};

		template<> void DBDirectTableSyncTemplate<PTUseRuleTableSync,PTUseRule>::Load(
			PTUseRule* rr,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			bool originIsReference (rows->getBool (PTUseRuleTableSync::COL_ORIGINISREFERENCE));

			time_duration minDelayMinutes = minutes(rows->getInt (PTUseRuleTableSync::COL_MINDELAYMINUTES));
			date_duration minDelayDays = days(rows->getInt (PTUseRuleTableSync::COL_MINDELAYDAYS));
			date_duration maxDelayDays = days(rows->getInt (PTUseRuleTableSync::COL_MAXDELAYDAYS));


			time_duration hourDeadline(rows->getHour(PTUseRuleTableSync::COL_HOURDEADLINE));

			PTUseRule::ReservationRuleType ruleType(static_cast<PTUseRule::ReservationRuleType>(rows->getInt(PTUseRuleTableSync::COL_RESERVATION_TYPE)));

			rr->setReservationType(ruleType);
			rr->setOriginIsReference (originIsReference);
			rr->setMinDelayMinutes (minDelayMinutes);
			rr->setMinDelayDays (minDelayDays);
			rr->setMaxDelayDays(maxDelayDays.days() > 0 ? maxDelayDays : optional<date_duration>());
			rr->setHourDeadLine (hourDeadline);
			rr->setName(rows->getText(PTUseRuleTableSync::COL_NAME));
			rr->setAccessCapacity(rows->getOptionalUnsignedInt(PTUseRuleTableSync::COL_CAPACITY));
			rr->setForbiddenInDepartureBoards(rows->getBool(PTUseRuleTableSync::COL_FORBIDDEN_IN_DEPARTURE_BOARDS));
			rr->setForbiddenInTimetables(rows->getBool(PTUseRuleTableSync::COL_FORBIDDEN_IN_TIMETABLES));
			rr->setForbiddenInJourneyPlanning(rows->getBool(PTUseRuleTableSync::COL_FORBIDDEN_IN_JOURNEY_PLANNING));

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType id(rows->getLongLong(PTUseRuleTableSync::COL_DEFAULT_FARE));
				if(id > 0)
				{
					try
					{
						rr->setDefaultFare(FareTableSync::Get(id, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<Fare> e)
					{
						Log::GetInstance().warn("Fare "+ lexical_cast<string>(id) +" not found in PT Use Rule "+ lexical_cast<string>(rr->getKey()));
					}
				}
			}
		}

		template<> void DBDirectTableSyncTemplate<PTUseRuleTableSync,PTUseRule>::Save(
			PTUseRule* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<PTUseRuleTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getAccessCapacity() ? lexical_cast<string>(*object->getAccessCapacity()) : string());
			query.addField(static_cast<int>(object->getReservationType()));
			query.addField(object->getOriginIsReference());
			query.addField(object->getMinDelayMinutes().total_seconds() / 60);
			query.addField(static_cast<int>(object->getMinDelayDays().days()));
			query.addField(object->getMaxDelayDays() ? lexical_cast<string>(object->getMaxDelayDays()->days()) : string());
			query.addField(object->getHourDeadLine().is_not_a_date_time() ? string() : to_simple_string(object->getHourDeadLine()));
			query.addField(object->getDefaultFare() ? object->getDefaultFare()->getKey() : RegistryKeyType(0));
			query.addField(object->getForbiddenInDepartureBoards());
			query.addField(object->getForbiddenInTimetables());
			query.addField(object->getForbiddenInJourneyPlanning());
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<PTUseRuleTableSync,PTUseRule>::Unlink(
			PTUseRule* obj
		){

		}



		template<> bool DBTableSyncTemplate<PTUseRuleTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<PTUseRuleTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PTUseRuleTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PTUseRuleTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace pt
	{
		PTUseRuleTableSync::SearchResult PTUseRuleTableSync::Search(
			util::Env& env,
			boost::optional<std::string> name /*= boost::optional<std::string>()*/,
			int first /*= 0*/,
			boost::optional<std::size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName /*= true*/,
			bool raisingOrder /*= true*/,
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			SelectQuery<PTUseRuleTableSync> query;
			if(name)
			{
				query.addWhereField(COL_NAME, *name, ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
			}
			if(number)
			{
				query.setNumber(*number + 1);
				if(first > 0)
				{
					query.setFirst(first);
				}
			}
			return LoadFromQuery(query, env, linkLevel);
		}
}	}
