
synthese::pt_website::TransportSiteAdmin::integrate();

synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::pt_website::TransportWebsiteTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::cms::Website>, synthese::pt_website::TransportWebsiteTableSync>::integrate();

synthese::pt_website::ObjectSiteLinkTableSync::integrate();
synthese::pt_website::RollingStockFilterTableSync::integrate();
synthese::pt_website::UserFavoriteJourneyTableSync::integrate();

synthese::pt_website::TransportWebsiteModule::integrate();

synthese::pt_website::PlacesListRequestFunction::integrate();
synthese::pt_website::UserFavoriteListInterfaceElement::integrate();

synthese::pt_website::UserFavoriteInterfacePage::integrate();

synthese::pt_website::CityListRequestFunction::integrate();
synthese::pt_website::CityListFunction::integrate();
synthese::pt_website::PlacesListFunction::integrate();

synthese::pt_website::SiteUpdateAction::integrate();
synthese::pt_website::AddUserFavoriteJourneyAction::integrate();

synthese::pt_website::TransportWebsiteRight::integrate();

// Registries
synthese::util::Env::Integrate<synthese::pt_website::ObjectSiteLink>();
synthese::util::Env::Integrate<synthese::pt_website::TransportWebsite>();
synthese::util::Env::Integrate<synthese::pt_website::SiteCommercialLineLink>();
synthese::util::Env::Integrate<synthese::pt_website::RollingStockFilter>();
synthese::util::Env::Integrate<synthese::pt_website::UserFavoriteJourney>();

