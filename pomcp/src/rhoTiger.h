#ifndef RHOTIGER_H
#define RHOTIGER_H

#include "tiger.h"
#include "simulator.h"
#include <vector>

class RhoTiger : public Tiger {  
public:
    RhoTiger();
    virtual double Rho_reward(const BAG& belief, int action) const;
    virtual double ProbObs(int observation, const STATE& startingState, int action, const STATE& finalState) const;
};

#endif // RHOTIGER_H
