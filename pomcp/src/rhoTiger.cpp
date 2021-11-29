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
 	  	return 0.5;

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
}
