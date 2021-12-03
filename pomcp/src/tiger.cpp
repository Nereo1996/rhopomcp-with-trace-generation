#include "tiger.h"
#include "simulator.h"
#include "utils.h"
#include <random>

using namespace UTILS;
using namespace std;

const double Tiger::NOISE = 0.15;


	std::random_device rd;
    std::mt19937 gen(rd());

TigerState::TigerState() :
tiger_position(1) {
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
 }


Tiger::~Tiger(){ }



bool Tiger::Step(STATE& s, int action, int& observation, double& reward) const 
{
	TigerState& state = static_cast<TigerState&>(s);
	reward = 0;
	reward = Tiger::reward(s,action);
	observation = Tiger::observation(s,action);

	if(action == ACT_OBS)
		return false;
	else
		return true;

/*
	//std::cout << "azione da fare: " << action << std::endl; 
	if (action == ACT_LEFT){
		reward = state.tiger_position != action ? 10 : -100; 
		observation = OBS_NONE;
		return true;

	} else if (action == ACT_RIGHT ){
		reward = state.tiger_position != action ? 10 : -100;
		observation = OBS_NONE;
		return true;
	} else{
		reward = -1;

     	std::bernoulli_distribution d(0.85);

     	if(d(gen))
     		observation = (state.tiger_position == POS_RIGHT?  OBS_RIGHT : OBS_LEFT);
     	else
     		observation = (state.tiger_position == POS_RIGHT? OBS_LEFT : OBS_RIGHT);

     	//std::cout << "posizione tigre: " << state.tiger_position << " osservazione: " << observation << std::endl;	
		return false;
	}
*/
}

 int Tiger::reward(const STATE& state, int action ) const{
 	 const TigerState s = static_cast<const TigerState&>(state);


	if (action == ACT_OBS) {
			// cost -1
			return -1;
		}

		// if open left
		if (action == ACT_LEFT) {
			// if tiger left
			if (s.tiger_position == POS_LEFT)
				return -100;
			else
				return 10;

		}

		// if open right
		if (action == ACT_RIGHT) {
			// if tiger right
			if (s.tiger_position == POS_RIGHT)
				return -100;
			else 
				return 10;
		}

		return 0;
	}



int Tiger::observation(STATE& state , int action) const {
    // if open, any observation
    if((action == ACT_RIGHT) || (action == ACT_LEFT)) {
        return OBS_NONE;
    }

    TigerState s = static_cast<TigerState&>(state);

    // it must be action == ACT_OBS 
    double r = RandomDouble(0.0, 1.0);

    if(r <= 0.85)
        return (s.tiger_position == POS_RIGHT?  OBS_RIGHT : OBS_LEFT);
    else
        return (s.tiger_position == POS_RIGHT? OBS_LEFT : OBS_RIGHT);
}


STATE* Tiger::Copy(const STATE& state) const{
	
	const TigerState& tigerstate = safe_cast<const TigerState&>(state);
	TigerState* newstate = MemoryPool.Allocate();

	newstate->tiger_position = tigerstate.tiger_position;
	return newstate;

}


STATE* Tiger::CreateStartState() const
{
	TigerState* tigerstate = MemoryPool.Allocate();

	double r = ((double) rand() / (RAND_MAX));
	std::bernoulli_distribution d(0.5);

     
    if(d(gen)){
	//if(r>=0.5){
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


 bool TigerState::isEqual(const STATE* a)const{
 	const TigerState* A = safe_cast<const TigerState*>(a);

 	if(tiger_position == A->tiger_position)
 		return true;
 	return false;
 }


 void TigerState::x(const STATE* a)const{
 	const TigerState* A = safe_cast<const TigerState*>(a);
 	std::cout << "tiger_position di Particles[i]" << tiger_position << " tiger_position di particle: " << A->tiger_position << std::endl;
 }


 bool TigerState::isEqual (STATE* a)const{
 	 TigerState* A = safe_cast<TigerState*>(a);

 	if(tiger_position == A->tiger_position)
 		return true;
 	return false;

 }


 void Tiger::DisplayState(const STATE& state, std::ostream& ostr) const{

        const TigerState tigerstate = static_cast<const TigerState&>(state);
        string temp= tigerstate.tiger_position == POS_LEFT ? "LEFT" : "RIGHT";
        if(tigerstate.tiger_position<1 || tigerstate.tiger_position >2)
        	ostr << "error, tiger position = " << tigerstate.tiger_position ;
        else
        	ostr << "STATO: " << temp << " ";
  

    //std::cout << std::endl;

}

 



    void Tiger::DisplayBeliefs(const BELIEF_STATE& beliefState, std::ostream& ostr) const{

		double right =0;
		double left =0;

    	for(int i=0; i< beliefState.GetNumSamples(); i++){
    		//const TigerState* state = static_cast<const TigerState*>(beliefState.GetSample(i));
    		const TigerState* state = safe_cast<const TigerState*>(beliefState.GetSample(i));
    		if(state->tiger_position == POS_RIGHT)
    			right++;
    		else
    			left++;

		}
			ostr << "BELIEF_STATE: \n";
            ostr << "[ RIGHT";
            ostr << ", PESO: ";
            ostr <<  right << ", NORMALIZZATO: " << right/beliefState.GetNumSamples() << " ]\n";
             ostr << "[ LEFT";
            ostr << ", PESO: ";
            ostr << left << ", NORMALIZZATO: "  <<left/beliefState.GetNumSamples() <<  " ]\n" << std::endl;
       
    }







    void Tiger::DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const{
    }

