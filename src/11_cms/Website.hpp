
/** Website class header.
	@file Website.hpp

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

#ifndef SYNTHESE_cms_Website_hpp__
#define SYNTHESE_cms_Website_hpp__

#include "Object.hpp"
#include "TreeRoot.hpp"
#include "TreeRankOrderingPolicy.hpp"
#include "SVNWorkingCopy.hpp"

#include "StandardFields.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/thread/mutex.hpp>

namespace synthese
{
	namespace cms
	{
		class Webpage;

		FIELD_TYPE(HostName, std::string)
		FIELD_TYPE(ClientURL, std::string)
		FIELD_TYPE(DefaultTemplate, boost::optional<Webpage&>)
		
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(StartDate),
			FIELD(EndDate),
			FIELD(HostName),
			FIELD(ClientURL),
			FIELD(DefaultTemplate),
			FIELD(db::svn::SVNWorkingCopy)
		> WebsiteRecord;

		//////////////////////////////////////////////////////////////////////////
		/// Website class.
		///	@ingroup m36
		/// @author Hugues Romain
		/// @since 3.2.0
		class Website:
			public tree::TreeRoot<Webpage, tree::TreeRankOrderingPolicy>,
			public Object<Website, WebsiteRecord>
		{
		public:
			typedef std::map<std::string, Webpage*> WebpagesBySmartURL;

			/// Chosen registry class.
			typedef util::Registry<Website>	Registry;

		private:
			WebpagesBySmartURL _webpagesBySmartURL;
			mutable boost::mutex _smartURLMutex; //!< For thread safety.

		public:
			Website(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				bool dateCheck() const;
				Webpage* getPageBySmartURL(const std::string& key) const;
				Webpage* getPageByIdOrSmartURL(const std::string& key) const;
				virtual std::string getName() const { return get<Name>(); }
				virtual SubObjects getSubObjects() const;

				typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > WebpagesList;

				WebpagesList getPagesList(
					const std::string& rootLabel
				) const;
			//@}

			//! @name Modifiers
			//@{
				void addPage(Webpage& page);
				void removePage(const std::string& page);
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif // SYNTHESE_cms_Website_hpp__