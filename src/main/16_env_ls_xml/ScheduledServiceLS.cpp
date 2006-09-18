#include "ScheduledServiceLS.h"

#include <assert.h>

#include <boost/tokenizer.hpp>

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/XmlToolkit.h"

#include "04_time/Schedule.h"

#include "15_env/Environment.h"
#include "15_env/Path.h"
#include "15_env/ScheduledService.h"


using namespace synthese::util::XmlToolkit;

using synthese::env::Path;
using synthese::time::Schedule;
using synthese::env::ScheduledService;



namespace synthese
{
namespace envlsxml
{

const std::string ScheduledServiceLS::SCHEDULEDSERVICE_TAG ("scheduledService");
const std::string ScheduledServiceLS::SCHEDULEDSERVICE_ID_ATTR ("id");
const std::string ScheduledServiceLS::SCHEDULEDSERVICE_SERVICENUMBER_ATTR ("serviceNumber");
const std::string ScheduledServiceLS::SCHEDULEDSERVICE_PATHID_ATTR ("pathId");
const std::string ScheduledServiceLS::SCHEDULEDSERVICE_BIKECOMPLIANCEID_ATTR ("bikeComplianceId");
const std::string ScheduledServiceLS::SCHEDULEDSERVICE_HANDICAPPEDCOMPLIANCEID_ATTR ("handicappedComplianceId");
const std::string ScheduledServiceLS::SCHEDULEDSERVICE_PEDESTRIANCOMPLIANCEID_ATTR ("pedestrianComplianceId");
const std::string ScheduledServiceLS::SCHEDULEDSERVICE_RESERVATIONRULEID_ATTR ("reservationRuleId");
const std::string ScheduledServiceLS::SCHEDULEDSERVICE_SCHEDULES_ATTR ("schedules");




void 
ScheduledServiceLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    // assert (SCHEDULEDSERVICE_TAG == node.getName ());
    uid id (GetLongLongAttr (node, SCHEDULEDSERVICE_ID_ATTR));

    if (environment.getScheduledServices ().contains (id)) return;

    uid pathId (GetLongLongAttr (node, SCHEDULEDSERVICE_PATHID_ATTR));

    int serviceNumber (GetIntAttr (node, SCHEDULEDSERVICE_SERVICENUMBER_ATTR));

    Path* path = environment.fetchPath (pathId);
    
    uid bikeComplianceId (GetLongLongAttr (node, SCHEDULEDSERVICE_BIKECOMPLIANCEID_ATTR));
    uid handicappedComplianceId (GetLongLongAttr (node, SCHEDULEDSERVICE_HANDICAPPEDCOMPLIANCEID_ATTR));
    uid pedestrianComplianceId (GetLongLongAttr (node, SCHEDULEDSERVICE_PEDESTRIANCOMPLIANCEID_ATTR));
    uid reservationRuleId (GetLongLongAttr (node, SCHEDULEDSERVICE_RESERVATIONRULEID_ATTR));

    std::string schedules (GetStringAttr (node, SCHEDULEDSERVICE_SCHEDULES_ATTR));
    
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    // Parse all schedules arrival#departure,arrival#departure...
    boost::char_separator<char> sep1 (",");
    boost::char_separator<char> sep2 ("#");
    tokenizer schedulesTokens (schedules, sep1);
    
    std::vector<synthese::time::Schedule> departureSchedules;
    std::vector<synthese::time::Schedule> arrivalSchedules;

    for (tokenizer::iterator schedulesIter = schedulesTokens.begin();
	 schedulesIter != schedulesTokens.end (); ++schedulesIter)
    {
	tokenizer schedulesTokens2 (*schedulesIter, sep2);
	tokenizer::iterator schedulesIter2 = schedulesTokens2.begin();
	
	departureSchedules.push_back (Schedule::FromString (*schedulesIter2));
	arrivalSchedules.push_back (Schedule::FromString (*(++schedulesIter2)));
    }
    
    assert (departureSchedules.size () > 0);
    assert (arrivalSchedules.size () > 0);

    
    ScheduledService* ss = new synthese::env::ScheduledService (id, serviceNumber, 
								path, 
								departureSchedules.at (0));

    ss->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    ss->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    ss->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));
    ss->setReservationRule (environment.getReservationRules ().get (reservationRuleId)); 

    path->addService (ss, departureSchedules, arrivalSchedules);
    environment.getScheduledServices ().add (ss);
}




XMLNode* 
ScheduledServiceLS::Save (const synthese::env::ScheduledService* scheduledService)
{
    // ...
    return 0;
}






}
}

