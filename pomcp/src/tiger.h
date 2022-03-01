#ifndef TIGER_H
#define TIGER_H

#include "simulator.h"
#include <vector>

struct TigerState: public STATE {
    int tiger_position;

    TigerState();
    TigerState(int position);
    void x(const STATE*) const;
    virtual bool isEqual(const STATE &) const;
    //bool isEqual(const TigerState &) const;
};

class Tiger : public SIMULATOR
{
private:
    mutable MEMORY_POOL<TigerState> MemoryPool;
  
public:
    static const double NOISE;
    Tiger();
    bool Step(STATE& s, int action, int& observation, double& reward) const;

    int NumStates() const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual STATE* Copy(const STATE& state) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;

    virtual void DisplayBeliefs(const BAG& beliefState, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;

    virtual int reward(const STATE& state, int action) const;
    virtual int observation(STATE& state, int action) const;

    //functions for rho parts
    virtual double Rho_reward(const BAG& belief, int action) const;
    virtual double ProbObs(int observation, const STATE& startingState, int action, const STATE& finalState) const;

    //functions for xes logging
    virtual void log_problem_info() const;
    virtual void log_beliefs(const BAG& beliefState, int action, int observation) const;
    virtual void log_state(const STATE& state) const;
    virtual void log_action(int action) const;
    //log observation: changed observation_t in int. idk if it's right at the moment.
    virtual void log_observation(const STATE&, int observation) const;
    virtual void log_reward(double reward) const;


};

enum
{
    POS_RIGHT = 1,
    POS_LEFT = 2
};

enum
{
    OBS_TIGER = 0,
    OBS_TREASURE = 1,
    OBS_RIGHT = 2,
    OBS_LEFT = 3
};

enum
{
    ACT_OBS = 0,
    ACT_RIGHT = 1,
    ACT_LEFT = 2
};

#endif // TIGER_H
