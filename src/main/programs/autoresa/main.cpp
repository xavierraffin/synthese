#include "Functions.h"
#include "Login.h"
#include "Search.h"
#include "Reservation.h"
#include "Confirmation.h"
#include "Logout.h"


using namespace std;

int main(int argc, char *argv[])
{

	/*CAGI implementation*/
	AGI_TOOLS agi;
	AGI_CMD_RESULT res;
	AGITool_Init(&agi);
	int state=0;
	int stateRs=0;

	
	AGITool_answer(&agi, &res);
	//cout<<"main AutoResa";
	
	//int menuKey[]={1,2,3,4,5};
	//Functions::setLanguage(Functions::readKey(&agi,&res,menuKey,5,1,Functions::getMenu(0,0)));
	Functions::setLanguage(1);
	
	// menu bienvenu, time to creat the objects
	Functions::playbackText(&agi,&res,Functions::getMenu(1,1));
	
	// prepare objects during the language
	Login *login=new Login(&agi,&res);
	Search *search=new Search(&agi,&res);
	Reservation *reservation=new Reservation(&agi,&res);
	Confirmation *confirmation=new Confirmation(&agi,&res);
	Logout *logout=new Logout(&agi,&res);

	state=login->start(Functions::getFatalError());
	/*
	* state is to signe the next step,
		7: FeedbackCst
		8: FeedbackDrv
		9: Search
		-1: interruption, stop system
		0 to operator
	**/
	switch(state)
	{
		case 7: //cout<<"jump to FeedbackCst";
			confirmation->start(Functions::getFatalError(),login->getSession());
			break;
		case 8: //cout<<"jump to FeedbackDrv";
			confirmation->start(Functions::getFatalError(),login->getSession());
			break;
		case 9: //cout<<"jump to search";
			do
			{
				state=search->start(Functions::getFatalError(),login->getSession());
				switch(state)
				{
					case 1:
						stateRs=reservation->start(Functions::getFatalError(),login->getSession(),search->getChoicedFavorisTrajet(),search->getChoicedTimeOfChoicedTrip());

						switch(stateRs)
						{
							case 1:
								//cout<<"reservation successful";
								break;
							case -1:
								// do nothing, bcz fatalError faised
								break;
							case 0:
								// custumer doesnot confirm the reservation, research again
								// after break, will be new in this loop bcz stateRs==0
								break;
							default:
								Functions::passToManuel(&agi, &res, const_cast<char *>((login->getSession())->callerId.c_str()));
								break;
						}
						break;
					case -1:
						// do nothing, bcz fatalError raised
						break;
					default:  // like 0, bcz fatalError
						Functions::passToManuel(&agi, &res, const_cast<char *>((login->getSession())->callerId.c_str()));
						break;
				}
			}
			while(stateRs==0);
			break;
			
		case -1:
			//cout<<"interuption, system stopped";
			break;
			
		default:  //like 0
			//cout<<"jump to operator";
			Functions::passToManuel(&agi, &res, const_cast<char *>((login->getSession())->callerId.c_str()));
			break;
	}


	

	AGITool_Destroy(&agi);
	logout->start(Functions::getFatalError(),login->getSession());
	
	
	delete login;
	delete search;
	delete reservation;
	delete confirmation;
	delete logout;
	
	return 0;
}







