
// ROAD MODULE 34

// Factories

synthese::road::RoadTableSync::integrate();
synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::road::CrossingTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::graph::Vertex>, synthese::road::CrossingTableSync>::integrate();
synthese::road::RoadChunkTableSync::integrate();
synthese::road::RoadPlaceTableSync::integrate();
synthese::road::RoadModule::integrate();
synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::road::PublicPlaceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::geography::NamedPlace>, synthese::road::PublicPlaceTableSync>::integrate();
synthese::road::RoadPlace::integrate();
synthese::road::PublicPlace::integrate();
synthese::road::NavstreetsFileFormat::integrate();

// Registries

synthese::util::Env::Integrate<synthese::road::Road>();
synthese::util::Env::Integrate<synthese::road::Crossing>();
synthese::util::Env::Integrate<synthese::road::RoadPlace>();
synthese::util::Env::Integrate<synthese::road::Address>();
synthese::util::Env::Integrate<synthese::road::RoadChunk>();
synthese::util::Env::Integrate<synthese::road::PublicPlace>();
