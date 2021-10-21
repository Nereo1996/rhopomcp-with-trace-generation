#ifndef RHOROCKSAMPLE_H
#define RHOROCKSAMPLE_H

#include "rocksample.h"
#include "simulator.h"
#include "coord.h"
#include "grid.h"

class RHOROCKSAMPLE : public ROCKSAMPLE{
	public:

  		RHOROCKSAMPLE(int size, int rocks);
  		//RHOROCKSAMPLE();
  		~RHOROCKSAMPLE();
  		
   		virtual void Rho_reward(STATE& s, BAG& beliefs, double& reward, int pos) const;
   		virtual double ProbObs(int observation, const STATE& startingState, int action, const STATE& finalState) const;

};

#endif