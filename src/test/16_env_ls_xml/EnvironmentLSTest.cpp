#include "EnvironmentLSTest.h"

#include "16_env_ls_xml/EnvironmentLS.h"

#include "15_env/Axis.h"
#include "15_env/Calendar.h"
#include "15_env/City.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Environment.h"
#include "15_env/Line.h"
#include "15_env/PlaceAlias.h"
#include "15_env/PublicPlace.h"
#include "15_env/PhysicalStop.h"
#include "15_env/TransportNetwork.h"

#include "04_time/Hour.h"
#include "04_time/Schedule.h"


#include "01_util/RGBColor.h"
#include "01_util/XmlToolkit.h"

#include <iostream>


using namespace synthese::env;
using namespace synthese::time;
using namespace synthese::util;


namespace synthese
{
namespace envlsxml
{


void 
EnvironmentLSTest::setUp () 
{

}


void 
EnvironmentLSTest::tearDown () 
{

} 





void 
EnvironmentLSTest::testLoadingEnvironment0 ()
{
    // Parse the environment xml file
    boost::filesystem::path xmlFile ("resources/environment0.xml", boost::filesystem::native);

    XMLNode environmentNode = 
	synthese::util::XmlToolkit::ParseFile (xmlFile, EnvironmentLS::ENVIRONMENT_TAG);

    Environment* env = EnvironmentLS::Load (environmentNode);
    
    // cities
    CPPUNIT_ASSERT_EQUAL (2, (int) env->getCities ().size ());
    CPPUNIT_ASSERT (env->getCities ().contains (6000));
    CPPUNIT_ASSERT (env->getCities ().contains (6001));
    
    const City* city6000 = env->getCities ().get (6000);
    CPPUNIT_ASSERT_EQUAL (std::string ("Toulouse"), city6000->getName ());
    CPPUNIT_ASSERT_EQUAL (10, (int) city6000->getConnectionPlacesMatcher ().size ());
    CPPUNIT_ASSERT_EQUAL (1, (int) city6000->getPublicPlacesMatcher ().size ());
    CPPUNIT_ASSERT_EQUAL (1, (int) city6000->getPlaceAliasesMatcher ().size ());
    CPPUNIT_ASSERT_EQUAL (2, (int) city6000->getRoadsMatcher ().size ());
    CPPUNIT_ASSERT_EQUAL (2, (int) city6000->getIncludedPlaces ().size ());



    // transport networks
    CPPUNIT_ASSERT_EQUAL (1, (int) env->getTransportNetworks ().size ());
    CPPUNIT_ASSERT (env->getTransportNetworks ().contains (2200));

    const TransportNetwork* transportNetwork2200 = env->getTransportNetworks ().get (2200);
    CPPUNIT_ASSERT_EQUAL (std::string ("tisseo"), transportNetwork2200->getName ());



    // axes
    CPPUNIT_ASSERT_EQUAL (2, (int) env->getAxes ().size ());
    CPPUNIT_ASSERT (env->getAxes ().contains (4000));
    CPPUNIT_ASSERT (env->getAxes ().contains (4001));
    
    const Axis* axis4000 = env->getAxes ().get (4000);
    CPPUNIT_ASSERT_EQUAL (std::string ("A1"), axis4000->getName ());
    CPPUNIT_ASSERT_EQUAL (true, axis4000->isFree ());
    CPPUNIT_ASSERT_EQUAL (false, axis4000->isAllowed ());
    


    // connection places
    CPPUNIT_ASSERT_EQUAL (11, (int) env->getConnectionPlaces ().size ());
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7000));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7001));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7002));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7003));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7004));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7005));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7006));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7007));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7008));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7009));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7030));
    
    const ConnectionPlace* connectionPlace7004 = env->getConnectionPlaces ().get (7004);
    CPPUNIT_ASSERT_EQUAL (std::string ("CP5"), connectionPlace7004->getName ());
    CPPUNIT_ASSERT_EQUAL (city6000, connectionPlace7004->getCity ());
    CPPUNIT_ASSERT_EQUAL (ConnectionPlace::CONNECTION_TYPE_LINELINE, 
			  connectionPlace7004->getConnectionType ());
    CPPUNIT_ASSERT_EQUAL (7, connectionPlace7004->getTransferDelay (
			      env->fetchVertex (12004), 
			      env->fetchVertex (12008)) );    



    // public places
    CPPUNIT_ASSERT_EQUAL (1, (int) env->getPublicPlaces ().size ());
    CPPUNIT_ASSERT (env->getPublicPlaces ().contains (13000));
    const PublicPlace* publicPlace13000 = env->getPublicPlaces ().get (13000);
    CPPUNIT_ASSERT_EQUAL (std::string ("PP1"), publicPlace13000->getName ());
    CPPUNIT_ASSERT_EQUAL (city6000, publicPlace13000->getCity ());


    
    // place aliases
    CPPUNIT_ASSERT_EQUAL (1, (int) env->getPlaceAliases ().size ());
    CPPUNIT_ASSERT (env->getPlaceAliases ().contains (11000));

    const PlaceAlias* placeAlias11000 = env->getPlaceAliases ().get (11000);
    CPPUNIT_ASSERT_EQUAL (std::string ("AP1"), placeAlias11000->getName ());
    CPPUNIT_ASSERT_EQUAL (city6000, placeAlias11000->getCity ());
    
    CPPUNIT_ASSERT (env->fetchPlace (7005) == placeAlias11000->getAliasedPlace ());
    

    
    // physical stops
    CPPUNIT_ASSERT_EQUAL (10, (int) env->getPhysicalStops ().size ());
    CPPUNIT_ASSERT (env->getPhysicalStops ().contains (12000));
    CPPUNIT_ASSERT (env->getPhysicalStops ().contains (12001));
    CPPUNIT_ASSERT (env->getPhysicalStops ().contains (12002));
    CPPUNIT_ASSERT (env->getPhysicalStops ().contains (12003));
    CPPUNIT_ASSERT (env->getPhysicalStops ().contains (12004));
    CPPUNIT_ASSERT (env->getPhysicalStops ().contains (12005));
    CPPUNIT_ASSERT (env->getPhysicalStops ().contains (12006));
    CPPUNIT_ASSERT (env->getPhysicalStops ().contains (12007));
    CPPUNIT_ASSERT (env->getPhysicalStops ().contains (12008));
    CPPUNIT_ASSERT (env->getPhysicalStops ().contains (12009));

    const PhysicalStop* physicalStop12000 = env->getPhysicalStops ().get (12000);
    CPPUNIT_ASSERT_EQUAL (std::string ("PS1"), physicalStop12000->getName ());
    CPPUNIT_ASSERT_EQUAL (220.0, physicalStop12000->getX ());
    CPPUNIT_ASSERT_EQUAL (123.0, physicalStop12000->getY ());
    CPPUNIT_ASSERT (env->fetchPlace (7000) == physicalStop12000->getPlace ());
    

    // lines
    CPPUNIT_ASSERT_EQUAL (2, (int) env->getLines ().size ());
    CPPUNIT_ASSERT (env->getLines ().contains (9000));
    CPPUNIT_ASSERT (env->getLines ().contains (9001));

    const Line* line9000 = env->getLines ().get (9000);
    const Line* line9001 = env->getLines ().get (9001);

    CPPUNIT_ASSERT_EQUAL (std::string ("L1"), line9000->getName ());
    CPPUNIT_ASSERT (RGBColor ("blue") == line9000->getColor ());
    CPPUNIT_ASSERT (transportNetwork2200 == line9000->getNetwork ());


    delete env;


}



}
}

