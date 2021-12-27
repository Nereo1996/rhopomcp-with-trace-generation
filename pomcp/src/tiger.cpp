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


void Tiger::DisplayBeliefs(const BAG& beliefState, std::ostream& ostr) const {

  if(beliefState.Empty())
        return;

    ostr << "bag: \n";
    for (auto& element : beliefState.getContainer()){
        ostr << "[ ";
        DisplayState(*element.first,ostr);
        ostr << " PESO: " << element.second << " ]\n";
    }
    
    ostr << std::endl;
}

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



// xes



//da rivedere
void Tiger::log_beliefs(const BAG& beliefState) const {

    XES::logger().start_list("belief");
    for (auto& element : beliefState.getContainer()){
        TigerState* ts = safe_cast<TigerState* >(element.first);
        ts->tiger_position == POS_RIGHT? XES::logger().add_attribute({"tiger right", element.second}) : XES::logger().add_attribute({"tiger left", element.second});
    }
    
    XES::logger().end_list();
}


//da rivedere
void Tiger::log_state(const STATE& state) const {
    const TigerState& tiger_state = safe_cast<const TigerState&>(state);
    XES::logger().start_list("state");
    tiger_state.tiger_position == POS_RIGHT? XES::logger().add_attribute({"tiger on right", tiger_state.tiger_position}) : XES::logger().add_attribute({"tiger on left", tiger_state.tiger_position});
    XES::logger().end_list();
}


//va bene
void Tiger::log_action(int action) const {
    switch (action) {
        case ACT_OBS:
            XES::logger().add_attribute({"action", "listen"});
            break;
        case ACT_LEFT:
            XES::logger().add_attribute({"action", "open left"});
            break;
        case ACT_RIGHT:
            XES::logger().add_attribute({"action", "open right"});
            break;
    }
}



//da guardare 
void Tiger::log_observation(const STATE&, int observation) const {
    switch (observation) {
        case OBS_LEFT:
            XES::logger().add_attribute({"observation", "roar left"});
            break;
        case OBS_RIGHT:
            XES::logger().add_attribute({"observation", "right"});
            break;
        case OBS_NONE:
            XES::logger().add_attribute({"observation", "none"});
            break;
    }
}


//easy giusto
void Tiger::log_reward(double reward) const {
    XES::logger().add_attribute({"reward", reward});
}

//se ci sono altri attributi aggiungili
void Tiger::log_problem_info() const {
    XES::logger().add_attributes({
            {"problem", "tiger"},
            {"RewardRange", RewardRange}
        });
}