
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::interfaces::InterfaceModule>("11_interfaces");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::StaticValueInterfaceElement>("print");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::ParameterValueInterfaceElement>("param");

synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::StaticValueInterfaceElement>("print");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::ParameterValueInterfaceElement>("param");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::LineLabelInterfaceElement>("label");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::IncludePageInterfaceElement>("include");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::IfThenElseInterfaceElement>("if");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::GotoInterfaceElement>("goto");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::interfaces::InterfaceTableSync>("16.01");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::interfaces::InterfacePageTableSync>("16.02");