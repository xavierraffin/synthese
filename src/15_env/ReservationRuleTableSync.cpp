

/** ReservationRuleTableSync class implementation.
	@file ReservationRuleTableSync.cpp

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

#include <sstream>

#include "ReservationRuleTableSync.h"
#include "ReservationRule.h"

#include "Conversion.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	template<> const string util::FactorableTemplate<SQLiteTableSync,ReservationRuleTableSync>::FACTORY_KEY("15.10.06 Reservation rules");

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<ReservationRuleTableSync>::TABLE_NAME = "t021_reservation_rules";
		template<> const int SQLiteTableSyncTemplate<ReservationRuleTableSync>::TABLE_ID = 21;
		template<> const bool SQLiteTableSyncTemplate<ReservationRuleTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<ReservationRuleTableSync,ReservationRule>::Load(
			ReservationRule* rr,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
		    bool online (rows->getBool (ReservationRuleTableSync::COL_ONLINE));

		    bool originIsReference (rows->getBool (ReservationRuleTableSync::COL_ORIGINISREFERENCE));
		    
		    int minDelayMinutes = rows->getInt (ReservationRuleTableSync::COL_MINDELAYMINUTES);
		    int minDelayDays = rows->getInt (ReservationRuleTableSync::COL_MINDELAYDAYS);
		    int maxDelayDays = rows->getInt (ReservationRuleTableSync::COL_MAXDELAYDAYS);
		    
		    synthese::time::Hour hourDeadline = 
			synthese::time::Hour::FromSQLTime (rows->getText (ReservationRuleTableSync::COL_HOURDEADLINE));
		    
		    std::string phoneExchangeNumber (
			rows->getText (ReservationRuleTableSync::COL_PHONEEXCHANGENUMBER));

		    std::string phoneExchangeOpeningHours (
			rows->getText (ReservationRuleTableSync::COL_PHONEEXCHANGEOPENINGHOURS));

		    std::string description (
			rows->getText (ReservationRuleTableSync::COL_DESCRIPTION));

		    std::string webSiteUrl (
			rows->getText (ReservationRuleTableSync::COL_WEBSITEURL));

			ReservationRuleType ruleType(static_cast<ReservationRuleType>(rows->getInt(ReservationRuleTableSync::COL_TYPE)));

			rr->setType(ruleType);
		    rr->setOnline (online);
		    rr->setOriginIsReference (originIsReference);
		    rr->setMinDelayMinutes (minDelayMinutes);
		    rr->setMinDelayDays (minDelayDays);
		    rr->setMaxDelayDays (maxDelayDays);
		    rr->setHourDeadLine (hourDeadline);
		    rr->setPhoneExchangeNumber (phoneExchangeNumber);
		    rr->setPhoneExchangeOpeningHours (phoneExchangeOpeningHours);
		    rr->setDescription (description);
		    rr->setWebSiteUrl (webSiteUrl);
		}


		template<> void SQLiteDirectTableSyncTemplate<ReservationRuleTableSync,ReservationRule>::Save(ReservationRule* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() == UNKNOWN_VALUE)
				object->setKey(getId());
            query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey()) << ","
				<< static_cast<int>(object->getType()) << ","
				<< Conversion::ToString(object->getOnline()) << ","
				<< object->getMinDelayMinutes() << ","
				<< object->getMinDelayDays() << ","
				<< object->getMaxDelayDays() << ","
				<< object->getHourDeadLine().toSQLString() << ","
				<< Conversion::ToSQLiteString(object->getPhoneExchangeNumber()) << ","
				<< Conversion::ToSQLiteString(object->getPhoneExchangeOpeningHours()) << ","
				<< Conversion::ToSQLiteString(object->getDescription()) << ","
				<< Conversion::ToSQLiteString(object->getWebSiteUrl())
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<ReservationRuleTableSync,ReservationRule>::Unlink(
			ReservationRule* obj,
			Env* env
		){

		}

	}

	namespace env
	{
		const std::string ReservationRuleTableSync::COL_TYPE ("reservation_type");
		const std::string ReservationRuleTableSync::COL_ONLINE ("online");
		const std::string ReservationRuleTableSync::COL_ORIGINISREFERENCE ("origin_is_reference");
		const std::string ReservationRuleTableSync::COL_MINDELAYMINUTES ("min_delay_minutes");
		const std::string ReservationRuleTableSync::COL_MINDELAYDAYS ("min_delay_days");
		const std::string ReservationRuleTableSync::COL_MAXDELAYDAYS ("max_delay_days");
		const std::string ReservationRuleTableSync::COL_HOURDEADLINE ("hour_deadline");
		const std::string ReservationRuleTableSync::COL_PHONEEXCHANGENUMBER ("phone_exchange_number");
		const std::string ReservationRuleTableSync::COL_PHONEEXCHANGEOPENINGHOURS ("phone_exchange_opening_hours");
		const std::string ReservationRuleTableSync::COL_DESCRIPTION ("description");
		const std::string ReservationRuleTableSync::COL_WEBSITEURL ("web_site_url");

		ReservationRuleTableSync::ReservationRuleTableSync()
			: SQLiteRegistryTableSyncTemplate<ReservationRuleTableSync,ReservationRule>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_TYPE, "INTEGER", true);
			addTableColumn (COL_ONLINE, "BOOLEAN", true);
			addTableColumn (COL_ORIGINISREFERENCE, "BOOLEAN", true);
			addTableColumn (COL_MINDELAYMINUTES, "INTEGER", true);
			addTableColumn (COL_MINDELAYDAYS, "INTEGER", true);
			addTableColumn (COL_MAXDELAYDAYS, "INTEGER", true);
			addTableColumn (COL_HOURDEADLINE, "TIME", true);
			addTableColumn (COL_PHONEEXCHANGENUMBER, "TEXT", true);
			addTableColumn (COL_PHONEEXCHANGEOPENINGHOURS, "TEXT", true);
			addTableColumn (COL_DESCRIPTION, "TEXT", true);
			addTableColumn (COL_WEBSITEURL, "TEXT", true);
		}



		void ReservationRuleTableSync::Search(
			Env& env,
			int first, /*= 0*/
			int number, /*= 0*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
