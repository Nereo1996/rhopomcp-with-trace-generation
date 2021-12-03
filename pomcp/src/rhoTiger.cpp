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

double RhoTiger::ProbObs(int observation, const STATE& startingState, int action, const STATE&) const{
    const TigerState startTigerstate = static_cast<const TigerState&>(startingState);
    if (observation == OBS_NONE)
        return action == ACT_OBS ? 0.0 : 1.0;

    if (startTigerstate.tiger_position == POS_RIGHT)
        return observation == OBS_RIGHT ? 1.0-Tiger::NOISE : Tiger::NOISE;
    else
        return observation == OBS_LEFT ? 1.0-Tiger::NOISE : Tiger::NOISE;
}
