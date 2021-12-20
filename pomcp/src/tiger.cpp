#include "tiger.h"
#include "simulator.h"
#include "utils.h"
#include <random>

using namespace UTILS;
using namespace std;

const double Tiger::NOISE = 0.15;


std::random_device rd;
std::mt19937 gen(rd());

TigerState::TigerState() : tiger_position(1) {}

TigerState::TigerState(int position) : tiger_position(position) {}

int Tiger::NumStates() const { return 2; }

Tiger::Tiger(){
    NumActions = 3;
    NumObservations = 3;
    RewardRange = 102;
    Discount = 0.95;
}

bool Tiger::Step(STATE& s, int action, int& observation, double& reward) const 
{
    reward = Tiger::reward(s, action);
    observation = Tiger::observation(s, action);

    return action != ACT_OBS;
}

int Tiger::reward(const STATE& state, int action ) const{
    if (action == ACT_OBS)
        return -1;

    const TigerState &s = safe_cast<const TigerState&>(state);

    // if open left
    if (action == ACT_LEFT) {
        if (s.tiger_position == POS_LEFT) // if tiger left
            return -100;
        else
            return 10;
    }

    // it must be open right
    if (s.tiger_position == POS_RIGHT) // if tiger right
        return -100;
    else 
        return 10;
}

int Tiger::observation(STATE& state , int action) const {
    // if open, no observation
    if (action != ACT_OBS)
        return OBS_NONE;

    TigerState s = safe_cast<TigerState&>(state);

    // it must be action == ACT_OBS 
    double r = RandomDouble(0.0, 1.0);

    if (r <= 0.85)
        return s.tiger_position == POS_RIGHT ?  OBS_RIGHT : OBS_LEFT;
    else
        return s.tiger_position == POS_RIGHT ? OBS_LEFT : OBS_RIGHT;
}

STATE* Tiger::Copy(const STATE& state) const {
    const TigerState& tigerstate = safe_cast<const TigerState&>(state);

    TigerState* newstate = MemoryPool.Allocate();
    newstate->tiger_position = tigerstate.tiger_position;
    return newstate;
}


STATE* Tiger::CreateStartState() const {
    TigerState* tigerstate = MemoryPool.Allocate();
    tigerstate->tiger_position = UTILS::Bernoulli(0.5) ?  POS_RIGHT : POS_LEFT;

    return tigerstate;
}

void Tiger::FreeState(STATE* state) const {
    TigerState* tigerstate = safe_cast<TigerState*>(state);
    MemoryPool.Free(tigerstate);
}

void Tiger::Validate(const STATE& state) const 
{
    const TigerState& tigerstate = safe_cast<const TigerState&>(state);
    assert(tigerstate.tiger_position == POS_LEFT ||
            tigerstate.tiger_position == POS_RIGHT);
}

bool TigerState::isEqual(const STATE &a)const{
    const auto &A = safe_cast<const TigerState&>(a);
    return tiger_position == A.tiger_position;
}

bool TigerState:: isEqual(const TigerState &s) const {
    return tiger_position == s.tiger_position;
}

void TigerState::x(const STATE* a)const{
    const TigerState* A = safe_cast<const TigerState*>(a);
    std::cout << "tiger_position di Particles[i]" << tiger_position
        << " tiger_position di particle: " << A->tiger_position
        << std::endl;
}

void Tiger::DisplayState(const STATE& state, std::ostream& ostr) const{
    const TigerState tigerstate = safe_cast<const TigerState&>(state);
    string temp= tigerstate.tiger_position == POS_LEFT ? "LEFT" : "RIGHT";
    if (tigerstate.tiger_position < 1 || tigerstate.tiger_position > 2)
        ostr << "error, tiger position = " << tigerstate.tiger_position ;
    else
        ostr << "STATO: " << temp << " ";
}




double Tiger::Rho_reward(const BAG& belief, int action) const {
    //assert(belief.is_normalized());
    double r = 0;
    /*for (int i = 0; i < belief.GetNumSamples(); i++) {
        int rew = reward(belief.GetSample(i), action);
        r += rew * belief.GetNormalizedWeight(i);
    }
    */
    for (auto& element : belief.getContainer()){
        r += reward(*element.first,action) * belief.GetNormalizedWeight(element.first);
    }


    return r;
}

double Tiger::ProbObs(int observation, const STATE& startingState, int action, const STATE&) const{
    const TigerState startTigerstate = static_cast<const TigerState&>(startingState);
    /*  if (observation == OBS_NONE)
        return action == ACT_OBS ? 0.0 : 1.0;

        if (startTigerstate.tiger_position == POS_RIGHT)
        return observation == OBS_RIGHT ? 1.0-Tiger::NOISE : Tiger::NOISE;
        else
        return observation == OBS_LEFT ? 1.0-Tiger::NOISE : Tiger::NOISE;

*/
    if(action == ACT_LEFT || action == ACT_RIGHT)
        return 0.5;
    else{                   //(action == ACT_OBS)

        if (startTigerstate.tiger_position == POS_RIGHT)
            return observation == OBS_RIGHT ? 1.0-Tiger::NOISE : Tiger::NOISE;
        else
            return observation == OBS_LEFT ? 1.0-Tiger::NOISE : Tiger::NOISE;

    }

}












void Tiger::DisplayBeliefs(const BAG& beliefState, std::ostream& ostr) const {}
/*void Tiger::DisplayBeliefs(const BAG& beliefState, std::ostream& ostr) const {

    double right =0;
    double left =0;

    for(int i=0; i< beliefState.GetNumSamples(); i++){
        const TigerState& state = safe_cast<const TigerState& >(beliefState.GetSample(i));
        if (state.tiger_position == POS_RIGHT)
            right++;
        else
            left++;
    }
    ostr << "BELIEF_STATE: \n";
    ostr << "[ RIGHT, PESO: " << right
        << ", NORMALIZZATO: " << right / beliefState.GetNumSamples() << " ]\n";
    ostr << "[ LEFT, PESO: " << left
        << ", NORMALIZZATO: " << left / beliefState.GetNumSamples() << " ]\n"
        << std::endl;
}
*/
void Tiger::DisplayObservation(const STATE &state, int observation,
        std::ostream &ostr) const {
    ostr << "Observation ";
    if (observation == OBS_NONE)
        ostr << "NONE";
    else if (observation == OBS_LEFT)
        ostr << "Open Left";
    else if (observation == OBS_RIGHT)
        ostr << "Open Right";
}
