
/** AlarmObjectLinkTableSync class implementation.
	@file AlarmObjectLinkTableSync.cpp

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

#include "AlarmObjectLinkTableSync.h"
#include "AlarmObjectLinkException.h"
#include "AlarmRecipient.h"
#include "AlarmObjectLink.h"
#include "SentAlarm.h"
#include "Registry.h"
#include "AlarmTableSync.h"
#include "LoadException.h"
#include "LinkException.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,AlarmObjectLinkTableSync>::FACTORY_KEY("99.00.01 Alarm links");
	}

	namespace messages
	{
		const string AlarmObjectLinkTableSync::COL_RECIPIENT_KEY("recipient_key");
		const string AlarmObjectLinkTableSync::COL_OBJECT_ID("object_id");
		const string AlarmObjectLinkTableSync::COL_ALARM_ID("alarm_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<AlarmObjectLinkTableSync>::TABLE(
			"t040_alarm_object_links"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<AlarmObjectLinkTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(AlarmObjectLinkTableSync::COL_RECIPIENT_KEY, SQL_TEXT),
			SQLiteTableSync::Field(AlarmObjectLinkTableSync::COL_OBJECT_ID, SQL_INTEGER),
			SQLiteTableSync::Field(AlarmObjectLinkTableSync::COL_ALARM_ID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<AlarmObjectLinkTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(AlarmObjectLinkTableSync::COL_OBJECT_ID.c_str(),	AlarmObjectLinkTableSync::COL_ALARM_ID.c_str(), ""),
			SQLiteTableSync::Index(AlarmObjectLinkTableSync::COL_ALARM_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<AlarmObjectLinkTableSync,AlarmObjectLink>::Load(
			AlarmObjectLink* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// It makes no sense to load such an object without the up links
			assert(linkLevel > FIELDS_ONLY_LOAD_LEVEL);

			object->setObjectId(rows->getLongLong ( AlarmObjectLinkTableSync::COL_OBJECT_ID));
			object->setRecipientKey(rows->getText ( AlarmObjectLinkTableSync::COL_RECIPIENT_KEY));

			try
			{
				shared_ptr<Alarm> alarm(AlarmTableSync::GetEditable(
						rows->getLongLong ( AlarmObjectLinkTableSync::COL_ALARM_ID),
						env,
						linkLevel
				)	);
				object->setAlarm(alarm.get());

				if(linkLevel >= RECURSIVE_LINKS_LOAD_LEVEL && dynamic_cast<SentAlarm*>(alarm.get()))
				{
					shared_ptr<AlarmRecipient> ar(Factory<AlarmRecipient>::create(object->getRecipientKey()));
					ar->addObject(static_cast<SentAlarm*>(alarm.get()), object->getObjectId());
				}
			}
			catch(FactoryException<AlarmRecipient> e)
			{
				throw LoadException<AlarmObjectLinkTableSync>(rows, AlarmObjectLinkTableSync::COL_RECIPIENT_KEY, "Unconsistent recipient type");
			}
			catch(ObjectNotFoundException<Alarm> e)
			{
				throw LinkException<AlarmObjectLinkTableSync>(rows, AlarmObjectLinkTableSync::COL_ALARM_ID, e);
			}
			catch (AlarmObjectLinkException e)
			{
				throw LoadException<AlarmObjectLinkTableSync>(rows, AlarmObjectLinkTableSync::COL_OBJECT_ID, e.getMessage());
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<AlarmObjectLinkTableSync,AlarmObjectLink>::Unlink(
			AlarmObjectLink* object
		){
			if(dynamic_cast<SentAlarm*>(object->getAlarm()))
			{
				try
				{
					shared_ptr<AlarmRecipient> ar(Factory<AlarmRecipient>::create(object->getRecipientKey()));
					ar->removeObject(static_cast<SentAlarm*>(object->getAlarm()), object->getObjectId());
				}
				catch(FactoryException<AlarmRecipient> e)
				{
					Log::GetInstance().error("Unhanded recipient type "+ object->getRecipientKey() +" in "+ TABLE.NAME +" object "+ lexical_cast<string>(object->getKey()), e);
				}
			}
		}


		template<> void SQLiteDirectTableSyncTemplate<AlarmObjectLinkTableSync,AlarmObjectLink>::Save(
			AlarmObjectLink* object,
			optional<SQLiteTransaction&> transaction
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
            query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getRecipientKey())
				<< "," << Conversion::ToString(object->getObjectId())
				<< "," << (object->getAlarm() ? object->getAlarm()->getKey() : RegistryKeyType(0))
				<< ")";
			sqlite->execUpdate(query.str());
		}

		template<> bool SQLiteConditionalRegistryTableSyncTemplate<AlarmObjectLinkTableSync,AlarmObjectLink>::IsLoaded( const SQLiteResultSPtr& row )
		{
			shared_ptr<Alarm> alarm(AlarmTableSync::GetEditable(
				row->getLongLong(AlarmObjectLinkTableSync::COL_ALARM_ID),
				Env::GetOfficialEnv(),
				FIELDS_ONLY_LOAD_LEVEL
			)	);
			return
				dynamic_cast<SentAlarm*>(alarm.get()) &&
				Factory<AlarmRecipient>::contains(row->getText(AlarmObjectLinkTableSync::COL_RECIPIENT_KEY))
			;
		}
	}

	namespace messages
	{
		void AlarmObjectLinkTableSync::Remove(
			RegistryKeyType alarmId,
			optional<RegistryKeyType> objectId
		){
			stringstream query;
			query
				<< "DELETE FROM " << TABLE.NAME
				<< " WHERE " 
				<< COL_ALARM_ID << "=" << Conversion::ToString(alarmId);
			if (objectId)
			{
				query << " AND " << COL_OBJECT_ID << "=" << *objectId;
			}

			DBModule::GetSQLite()->execUpdate(query.str());
		}

		
		
		void AlarmObjectLinkTableSync::CopyRecipients(
			const Alarm& sourceAlarm,
			Alarm& destAlarm
		){
			Env lenv;
			SearchResult links(
				Search(lenv, sourceAlarm.getKey())
			);
			BOOST_FOREACH(shared_ptr<AlarmObjectLink> aol, links)
			{
				AlarmObjectLink naol;
				naol.setAlarm(&destAlarm);
				naol.setObjectId(aol->getObjectId());
				naol.setRecipientKey(aol->getRecipientKey());
				Save(&naol);
			}
		}

		AlarmObjectLinkTableSync::SearchResult AlarmObjectLinkTableSync::Search(
			Env& env,
			util::RegistryKeyType alarmId,
			int first /*= 0*/,
			boost::optional<std::size_t> number, /*= 0*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " 
				<< AlarmObjectLinkTableSync::COL_ALARM_ID << "=" << util::Conversion::ToString(alarmId);
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
