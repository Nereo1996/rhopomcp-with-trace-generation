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
 }


Tiger::~Tiger(){ }



bool Tiger::Step(STATE& s, int action, int& observation, double& reward) const 
{
	TigerState& state = static_cast<TigerState&>(s);
	reward = 0;
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

}


   



STATE* Tiger::Copy(const STATE& state) const{
	
	const TigerState& tigerstate = safe_cast<const TigerState&>(state);
	TigerState* newstate = MemoryPool.Allocate();
	*newstate = tigerstate;

	return newstate;

}


STATE* Tiger::CreateStartState() const
{
	TigerState* tigerstate = MemoryPool.Allocate();
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


 bool TigerState::isEqual(const STATE* a)const{
 	const TigerState* A = safe_cast<const TigerState*>(a);

 	if(tiger_position == A->tiger_position)
 		return true;
 	return false;
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

