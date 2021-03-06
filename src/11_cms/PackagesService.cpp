
//////////////////////////////////////////////////////////////////////////////////////////
///	PackagesService class implementation.
///	@file PackagesService.cpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PackagesService.hpp"

#include "BasicClient.h"
#include "CMSInstallRight.hpp"
#include "HTMLForm.h"
#include "HTMLPage.hpp"
#include "HTMLTable.h"
#include "HTMLModule.h"
#include "ImportTableSync.hpp"
#include "InterSYNTHESEPackage.hpp"
#include "InterSYNTHESEPackageFileFormat.hpp"
#include "InterSYNTHESEPackagesService.hpp"
#include "LoginAction.h"
#include "Profile.h"
#include "RequestException.h"
#include "Request.h"
#include "Session.h"
#include "StaticActionFunctionRequest.h"
#include "StaticFunctionRequest.h"
#include "SVNCheckoutAction.hpp"
#include "SVNUpdateAction.hpp"
#include "URI.hpp"
#include "User.h"
#include "Webpage.h"
#include "Website.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost;
using namespace boost::algorithm;
using namespace boost::property_tree;
using namespace std;

namespace synthese
{
	using namespace html;
	using namespace impex;
	using namespace inter_synthese;
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace db::svn;

	template<>
	const string FactorableTemplate<Function,cms::PackagesService>::FACTORY_KEY = "packages";
	
	namespace cms
	{
		const string PackagesService::PARAMETER_REPO_URL = "repo_url";
		const string PackagesService::PARAMETER_USER = "user";
		const string PackagesService::PARAMETER_PASSWORD = "password";
		const string PackagesService::PARAMETER_REPO_TYPE = "repo_type";
		const string PackagesService::PARAMETER_PACKAGE_TO_INSTALL = "package_to_install";
		


		ParametersMap PackagesService::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_REPO_URL, (_repoType == SVN_REPO) ? _svnRepo.getURL() : _interSYNTHESERepoURL);
			map.insert(PARAMETER_REPO_TYPE, static_cast<size_t>(_repoType));
			map.insert(PARAMETER_USER, _user);
			map.insert(PARAMETER_PASSWORD, _password);
			if(_packageToInstall)
			{
				map.insert(PARAMETER_PACKAGE_TO_INSTALL, *_packageToInstall);
			}
			return map;
		}



		void PackagesService::_setFromParametersMap(const ParametersMap& map)
		{
			// Repo type
			_repoType = static_cast<RepositoryType>(
				map.getDefault<size_t>(
					PARAMETER_REPO_TYPE,
					static_cast<size_t>(INTER_SYNTHESE_REPO)
			)	);
			if(_repoType == SVN_REPO)
			{
				_svnRepo = SVNRepository(
					map.getDefault<string>(
						PARAMETER_REPO_URL,
						"https://extranet.rcsmobility.com/svn/synthese3/trunk/sites"
				)	);
			}
			else
			{
				_interSYNTHESERepoURL = map.getDefault<string>(
					PARAMETER_REPO_URL,
					"www.rcsmobility.com:8088"
				);
			}
			_user = map.getDefault<string>(PARAMETER_USER);
			_password = map.getDefault<string>(PARAMETER_PASSWORD);

			_packageToInstall = map.getOptional<string>(PARAMETER_PACKAGE_TO_INSTALL);
		}



		ParametersMap PackagesService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			HTMLPage p;
			p.setDefaultInlineCSS();
			
			if(request.getSession() &&
				request.getSession()->hasProfile() &&
				request.getSession()->getUser()->getProfile()->isAuthorized<CMSInstallRight>(READ)
			){
				// User right
				bool installRight(request.getSession()->getUser()->getProfile()->isAuthorized<CMSInstallRight>(WRITE));

				if(	installRight &&
					_packageToInstall
				){
					InterSYNTHESEPackage::PackageAddress address(*_packageToInstall);
					// Search for existing import based on this package
					shared_ptr<Import> import;
					Env env;
					BOOST_FOREACH(const Import::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<Import>())
					{
						// Jump over non inter synthese imports
						if(it.second->get<FileFormatKey>() != InterSYNTHESEPackageFileFormat::FACTORY_KEY)
						{
							continue;
						}
						shared_ptr<InterSYNTHESEPackageFileFormat::Importer_> importer(
							dynamic_pointer_cast<InterSYNTHESEPackageFileFormat::Importer_, Importer>(
								it.second->getImporter(env, IMPORT_LOG_NOLOG, string(), optional<ostream&>(), it.second->get<Parameters>())
						)	);
						if(!importer.get())
						{
							continue;
						}
						try
						{
							importer->setFromParametersMap(it.second->get<Parameters>(), true);
						}
						catch(...)
						{
							continue;
						}
						if(	importer->getSmartURL() == address.smartURL &&
							importer->getUser() == _user &&
							importer->getPassword() == _password
						){
							import = it.second;
							break;
						}
					}

					// Create import if not exist
					if(!import.get())
					{
						import.reset(new Import);
						import->set<Name>(*_packageToInstall);
						import->set<FileFormatKey>(InterSYNTHESEPackageFileFormat::FACTORY_KEY);
						ParametersMap pm;
						pm.insert(InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_URL, *_packageToInstall);
						pm.insert(InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_USER, _user);
						pm.insert(InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_PASSWORD, _password);
						import->set<Parameters>(pm);
						ImportTableSync::Save(import.get());
					}

					shared_ptr<Importer> importer(
						import->getImporter(env, IMPORT_LOG_NOLOG, string(), optional<ostream&>(), import->get<Parameters>())
					);
					bool success(true);
					importer->setFromParametersMap(import->get<Parameters>(), true);
					success &= importer->beforeParsing();
					success &= importer->parseFiles();
					success &= importer->afterParsing();
					if(success)
					{
						importer->save().run();
					}
				}

				// SVN URL change form
				vector<pair<optional<RepositoryType>, string> > repoTypes;
				repoTypes.push_back(make_pair(INTER_SYNTHESE_REPO, "Inter-SYNTHESE"));
				repoTypes.push_back(make_pair(SVN_REPO, "Subversion"));
				StaticFunctionRequest<PackagesService> svnRequest(request, true);
				optional<string> undefinedPackage;
				svnRequest.getFunction()->setPackageToInstall(undefinedPackage);
				HTMLForm f(svnRequest.getHTMLForm());
				p << f.open();
				p << "Type : " << f.getSelectInput(PARAMETER_REPO_TYPE, repoTypes, optional<RepositoryType>(_repoType));
				p << "URL : " << f.getTextInput(PARAMETER_REPO_URL, (_repoType == SVN_REPO) ? _svnRepo.getURL() : _interSYNTHESERepoURL);
				p << " Utilisateur : " << f.getTextInput(PARAMETER_USER, _user);
				p << " Mot de passe : " << f.getPasswordInput(PARAMETER_PASSWORD, _password);
				p << " " << f.getSubmitButton("Changer");
				p << f.close();

				// Get installable packages
				if(_repoType == SVN_REPO)
				{
					SVNCommands::LsResult packages(_svnRepo.ls(_user, _password));
					if(packages.empty())
					{
						p << "<p>Pas de package à installer à cette URL.</p>";
					}
					else
					{
						// Installation request
						StaticActionFunctionRequest<SVNCheckoutAction, PackagesService> checkoutRequest(request, true);
						checkoutRequest.getAction()->setUser(_user);
						checkoutRequest.getAction()->setPassword(_password);

						// Update request
						StaticActionFunctionRequest<SVNUpdateAction, PackagesService> updateRequest(request, true);
						updateRequest.getAction()->setUser(_user);
						updateRequest.getAction()->setPassword(_password);

						// Draw the table
						HTMLTable::ColsVector c;
						c.push_back("Package");
						c.push_back("Installé");
						if(installRight)
						{
							c.push_back("Actions");
						}
						HTMLTable t(c);
						p << t.open();
						BOOST_FOREACH(const string& package, packages)
						{
							// Jump over empty strings
							if(trim_copy(package).empty())
							{
								continue;
							}

							// Check of the id of the site
							SVNCommands::LsResult files(
								_svnRepo.ls("/"+ package, _user, _password)
							);
							RegistryKeyType siteId(0);
							BOOST_FOREACH(const string& file, files)
							{
								if(	file.size() > 5 &&
									file.substr(file.size() - 5, 5) == ".dump"
								){
									try
									{
										siteId = lexical_cast<RegistryKeyType>(
											file.substr(0, file.size() - 5)
										);
									}
									catch(bad_lexical_cast&)
									{
									}
								}
							}

							// No id was found : corrupted package
							if(!siteId)
							{
								continue;
							}

							// Is the package currently installed ?
							bool isInstalled(
								Env::GetOfficialEnv().getRegistry<Website>().contains(siteId)
							);

							// HTML output
							p << t.row();

							// Package name
							p << t.col() << package;

							// Package is installed =
							p << t.col() <<
								(isInstalled ? "OUI" : "NON");

							// Action cell
							if(installRight)
							{
								p << t.col();
								if(isInstalled)
								{
									boost::shared_ptr<ObjectBase> site(
										Env::GetOfficialEnv().getCastEditable<ObjectBase, Website>(siteId)
									);
									updateRequest.getAction()->setObject(site);
									p << HTMLModule::getLinkButton(
										updateRequest.getURL(),
										"Mettre à jour",
										"Etes-vous sûr de vouloir mettre à jour le package "+ package +" ?"
									);
								}
								else
								{
									SVNRepository packageRepo(_svnRepo.getURL() +"/"+ package);
									checkoutRequest.getAction()->setRepo(packageRepo);
									p << HTMLModule::getLinkButton(
										checkoutRequest.getURL(),
										"Installer",
										"Etes-vous sûr de vouloir installer le package "+ package +" ?"
									);
								}
							}
						}
						p << t.close();
					}
				}
				else
				{ // Inter-SYNTHESE packages

					// Getting the packages list
					string port("80");
					string host(_interSYNTHESERepoURL);
					vector<string> parts;
					split(parts, _interSYNTHESERepoURL, is_any_of(":"));
					if(parts.size() > 1)
					{
						host = parts[0];
						port = parts[1];
					}
					StaticFunctionRequest<InterSYNTHESEPackagesService> r;
					r.getFunction()->setOutputFormat(MimeTypes::JSON);
					BasicClient c(
						host,
						port
					);
					string contentStr(
						c.post(
							host,
							r.getURI(),
							"application/x-www-form-urlencoded"
					)	);
					if(contentStr.empty())
					{
						p << "<p>Pas de package à installer à cette URL.</p>";
					}
					else
					{
						ptree packages;
						istringstream ss(contentStr);
						read_json(ss, packages);
					
						if(packages.count(InterSYNTHESEPackagesService::TAG_PACKAGE))
						{
							HTMLTable::ColsVector c;
							c.push_back("Package");
							c.push_back("Installé");
							if(installRight)
							{
								c.push_back("Actions");
							}
							HTMLTable t(c);
							p << t.open();

							StaticFunctionRequest<PackagesService> installRequest(request, true);

							BOOST_FOREACH(const ptree::value_type& packageNode, packages.get_child(InterSYNTHESEPackagesService::TAG_PACKAGE))
							{
								RegistryKeyType packageId(
									packageNode.second.get(Key::FIELD.name, RegistryKeyType(0))
								);
								string packageName(
									packageNode.second.get(Name::FIELD.name, string())
								);


								p << t.row();

								p << t.col() << packageName;

								// Installed
								bool installed(
									Env::GetOfficialEnv().getRegistry<InterSYNTHESEPackage>().contains(
										packageId
								)	);
								p << t.col() << (installed ? "OUI" : "NON");

								// Action
								optional<string> packageURL("http://" + host +":" + port + "/"+ packageNode.second.get(Code::FIELD.name, string()));
								installRequest.getFunction()->setPackageToInstall(packageURL);;
								p << t.col() << HTMLModule::getHTMLLink(installRequest.getURL(), "Installer", "Etes-vous sûr de vouloir installer le paquet "+ packageName);
							}	

							p << t.close();
						}
						else
						{
							p << "<p>Pas de package à installer à cette URL.</p>";
						}
					}
				}
			}
			else
			{
				p << "<p>Veuillez vous connecter pour utiliser cette fonction</p>";

				StaticActionFunctionRequest<LoginAction, PackagesService> loginRequest(request, true);
				HTMLForm f(loginRequest.getHTMLForm());
				p << f.open();
				p << "Utilisateur : " << f.getTextInput(LoginAction::PARAMETER_LOGIN, string());
				p << " Mot de passe : " << f.getPasswordInput(LoginAction::PARAMETER_PASSWORD, string());
				p << " " << f.getSubmitButton("Login");
				p << f.close();
			}

			p.output(stream);

			return map;
		}
		
		
		
		bool PackagesService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PackagesService::getOutputMimeType() const
		{
			return MimeTypes::HTML;
		}
}	}
