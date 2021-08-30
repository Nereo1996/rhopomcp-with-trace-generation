#include "tiger.h"
#include "simulator.h"
#include "utils.h"
#include <random>

using namespace UTILS;
using namespace std;

const double Tiger::NOISE = 0.15;




TigerState::TigerState() :
tiger_position(0) {
}

TigerState::TigerState(int position) : tiger_position(position) {
}

//std::string TigerState::text() const{
//	return tiger_position == Tiger::LEFT ? "LEFT" : "RIGHT";
//}

int Tiger::NumStates() const {
	return 2;
}

Tiger::Tiger(){
	NumActions = 3;
	NumObservations = 3;
	RewardRange = 100;
 };


Tiger::~Tiger(){ };

bool Tiger::Step(STATE& s, int action, int& observation, double& reward) const 
{
	TigerState& state = static_cast<TigerState&>(s);
	reward = 0;
	bool terminal;

	if (action == ACT_LEFT){
		reward = state.tiger_position != action ? 10 : -100; 
		observation = OBS_NONE;
		terminal = true;

	} else if (action == ACT_RIGHT ){
		reward = state.tiger_position != action ? 10 : -100;
		observation = OBS_NONE;
		terminal = true;
	} else{
		reward = -1;
		if(Tiger::GetObservation(state) == OBS_RIGHT){
			observation = (state.tiger_position == POS_RIGHT?  OBS_RIGHT : OBS_LEFT);		
		}else{
			observation = (state.tiger_position == POS_LEFT? OBS_LEFT : OBS_RIGHT);	
		}
		terminal = false;
	}
	return terminal;
}

STATE* Tiger::Copy(const STATE& state) const{
	
	const TigerState& tigerstate = safe_cast<const TigerState&>(state);
	TigerState* newstate = MemoryPool.Allocate();
	*newstate = tigerstate;

	return newstate;

}



int Tiger::GetObservation(const TigerState& tigerstate) const
{
	if (Bernoulli(1-Tiger::NOISE))
		return OBS_RIGHT;
	else
		return OBS_LEFT;
}

STATE* Tiger::CreateStartState() const
{
	TigerState* tigerstate = MemoryPool.Allocate();

	std::random_device rd;
    std::mt19937 gen(rd());

     std::bernoulli_distribution d(0.50);

  	//std::default_random_engine generator;

    //const int index = Random(100);

	//std::cout << "Sei entrato in CreateStartState! ora proverai a fare Bernoulli.." << std::endl;

	  if(d(gen)){
		tigerstate -> tiger_position = POS_RIGHT;
	}
	else{
		tigerstate->tiger_position = POS_LEFT;
	}

	return tigerstate;
}

void Tiger::FreeState(STATE* state) const 
{
	TigerState* tigerstate = safe_cast<TigerState*>(state);
	MemoryPool.Free(tigerstate);
}


void Tiger::Validate(const STATE& state) const 
{
	const TigerState& tigerstate = safe_cast<const TigerState&>(state);
	assert(tigerstate.tiger_position == POS_LEFT || tigerstate.tiger_position == POS_RIGHT);
}



 bool TigerState::isEqual (STATE* a)const{
 	 TigerState* A = safe_cast< TigerState*>(a);

 	if(tiger_position == A->tiger_position)
 		return true;
 	return false;

 }


 void Tiger::DisplayState(const STATE& state, std::ostream& ostr) const{

        const TigerState tigerstate = static_cast<const TigerState&>(state);
         std:: string temp= tigerstate.tiger_position == POS_LEFT ? "LEFT" : "RIGHT";
        ostr << "STATO: " << temp << " ";
  

    //std::cout << std::endl;

}

 double Tiger::ProbObs(int observation, const STATE& startingState, int action, const STATE& finalState) const{
 	  const TigerState startTigerstate = static_cast<const TigerState&>(startingState);
 	  const TigerState finalTigerstate = static_cast<const TigerState&>(finalState);

 	  if(startTigerstate.tiger_position == POS_RIGHT){

 	   if(action == ACT_OBS && observation == OBS_RIGHT)
 	  		return 1-Tiger::NOISE;
 	  	else if(action == ACT_OBS && observation == OBS_LEFT)// manca il caso (action == ACT_OBS && observation == OBS_LEFT)
 	  		return Tiger::NOISE;

 	  } else{ //caso startTigerstate.tiger_position == POS_LEFT

 	 	 if(action == ACT_OBS && observation == OBS_LEFT)
 	  		return 1-Tiger::NOISE;
 	 	 else if(action == ACT_OBS && observation == OBS_RIGHT) // manca il caso (action == ACT_OBS && observation == OBS_RIGHT)
 	  		return Tiger::NOISE;


 	  }

 	  return 1.0;

 	  //nb. in questo problema non è necessario utilizzare lo stato successivo.
}



    void Tiger::DisplayBeliefs(const BELIEF_STATE& beliefState, std::ostream& ostr) const{

    }
    void Tiger::DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const{


    }



    /*

		if(action == ACT_LEFT && observation == OBS_GOOD)
 	  		return #bo;
 	  	else if(action == ACT_LEFT && observation == OBS_BAD)
 	  		return #bo;
 	  	else if(action == ACT_LEFT && observation == OBS_NONE)
 	  		return #bo;
 	  	else if(action == ACT_RIGHT && observation == OBS_GOOD)
 	  		return #bo;
 	  	else if(action == ACT_RIGHT && observation == OBS_BAD)
 	  		return #bo;
 	  	else if(action == ACT_LEFT && observation == OBS_NONE)
 	  		return #bo;
 	  	else if(action == ACT_OBS && observation == OBS_GOOD)
 	  		return #bo;
 	  	else if(action == ACT_OBS && observation == OBS_BAD)
 	  		return #bo;
 	  	else if(action == ACT_OBS && observation == OBS_NONE)
 	  		return #bo;


    */