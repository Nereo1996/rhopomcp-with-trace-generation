#include "rhoTiger.h"
#include "simulator.h"
#include "utils.h"
#include <random>

using namespace UTILS;
using namespace std;

 RhoTiger::~RhoTiger(){

    }

    RhoTiger::RhoTiger(){

    	NumActions = 3;
		NumObservations = 3;
		RewardRange = 100;
    }


     double RhoTiger::Rho_reward(STATE& s, BAG& beliefs, int action) const
    {

    	double r = 0;
    	for(int i =0 ; i < beliefs.GetNumSamples(); i++){
    		int x = reward(*beliefs.GetSample(i), action);
    		r+= x * beliefs.GetWeight(i);
    	}
    	return r;

/*
    	TigerState state = static_cast< TigerState&>(s);

    	for(int i =0 ; i < beliefs.GetNumSamples(); i++){
    		if(s.isEqual(beliefs.GetSample(i))){
    			int x = reward(*beliefs.GetSample(i), action);
    			return x * beliefs.GetWeight(i);
    		}
    	}
    	return 0;
*/

	}

	double RhoTiger::ProbObs(int observation, const STATE& startingState, int action, const STATE& finalState) const{
 	  const TigerState startTigerstate = static_cast<const TigerState&>(startingState);
 	  const TigerState finalTigerstate = static_cast<const TigerState&>(finalState);

 	  //std::cout << "azione fatta: " << action << " osservazione fatta: " << observation << std::endl;
 	  if(observation == OBS_NONE)
 	  	return 1.0;

 	  if(startTigerstate.tiger_position == POS_RIGHT){

 	   if(observation == OBS_RIGHT) //action == ACT_OBS inutile, in caso di altre azioni avrebbe già terminato.
 	  		return 1.0-Tiger::NOISE;
 	  	else if(observation == OBS_LEFT)//action == ACT_OBS 
 	  		return Tiger::NOISE;

 	  } else{ //caso startTigerstate.tiger_position == POS_LEFT

 	 	 if(observation == OBS_LEFT)
 	  		return 1.0-Tiger::NOISE;
 	 	 else if(observation == OBS_RIGHT) // manca il caso (action == ACT_OBS && observation == OBS_RIGHT)
 	  		return Tiger::NOISE;


 	  }

 	  //nb. in questo problema non è necessario utilizzare lo stato successivo.

 	  return 0.5;
}



/*
int RhoTiger::NumStates() const {
	return 2;
}

   

    void RhoTiger::Rho_reward(STATE& s, BAG& beliefs, double& reward) const
    {
    	//DisplayState(s,std::cout);
		int pos = beliefs.ParticlePosition(s);


		reward = reward * beliefs.GetWeight(pos);

	}

 double RhoTiger::ProbObs(int observation, const STATE& startingState, int action, const STATE& finalState) const{
 	  const RhoTigerState startTigerstate = static_cast<const RhoTigerState&>(startingState);
 	  const RhoTigerState finalTigerstate = static_cast<const RhoTigerState&>(finalState);

 	  //std::cout << "azione fatta: " << action << " osservazione fatta: " << observation << std::endl;
 	  if(observation == Tiger::OBS_NONE)
 	  	return 1.0;

 	  if(startTigerstate.tiger_position == Tiger::POS_RIGHT){

 	   if(observation == Tiger::OBS_RIGHT) //action == ACT_OBS inutile, in caso di altre azioni avrebbe già terminato.
 	  		return 1.0-Tiger::NOISE;
 	  	else if(observation == Tiger::OBS_LEFT)//action == ACT_OBS 
 	  		return Tiger::NOISE;

 	  } else{ //caso startTigerstate.tiger_position == POS_LEFT

 	 	 if(observation == Tiger::OBS_LEFT)
 	  		return 1.0-Tiger::NOISE;
 	 	 else if(observation == Tiger::OBS_RIGHT) // manca il caso (action == ACT_OBS && observation == OBS_RIGHT)
 	  		return Tiger::NOISE;


 	  }

 	  //nb. in questo problema non è necessario utilizzare lo stato successivo.
}





 void RhoTiger::Validate(const STATE& state) const{
 	Tiger::Validate(state);
 }

 STATE* RhoTiger::CreateStartState() const{
		return Tiger::CreateStartState();
	}
void RhoTiger::FreeState(STATE* state) const{
    	Tiger::FreeState(state);
    }

STATE* RhoTiger::Copy(const STATE& state) const{
	return Tiger::Copy(state);
}
void RhoTiger::DisplayState(const STATE& state, std::ostream& ostr) const{
	Tiger::DisplayState(state,ostr);
}


void RhoTiger::DisplayBeliefs(const BELIEF_STATE& beliefState, std::ostream& ostr) const{
	Tiger::DisplayBeliefs(beliefState,ostr);
}

void RhoTiger::DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const{

}
*/