#ifndef TIGER_H
#define TIGER_H



#include "simulator.h"
#include <vector>


class TigerState: public STATE {
public:

	int tiger_position;

	TigerState();
	TigerState(int position);
    bool isEqual (STATE* a)const;
    bool isEqual(const STATE*) const;

};

class Tiger : public SIMULATOR
{
private:
    mutable MEMORY_POOL<TigerState> MemoryPool;
  
public:
	static const double NOISE;
	Tiger();
	~Tiger();
	bool Step(STATE& s, int action, int& observation, double& reward) const;


	int NumStates() const;
	virtual void Validate(const STATE& state) const;
	virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual STATE* Copy(const STATE& state) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;


    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;


    //virtual double ProbObs(int observation, const STATE& startingState, int action, const STATE& finalState) const;
    //virtual void Rho_reward(STATE& s, BAG& beliefs, double& reward, int pos) const;



 enum
    {
        POS_RIGHT = 1,
        POS_LEFT = 2
    };

   enum
    {
        OBS_NONE = 0,
        OBS_RIGHT = 1,
        OBS_LEFT = 2
    };

    enum
    {
        ACT_OBS = 0,
        ACT_RIGHT = 1,
        ACT_LEFT = 2
    };

};

#endif // TIGER_H
