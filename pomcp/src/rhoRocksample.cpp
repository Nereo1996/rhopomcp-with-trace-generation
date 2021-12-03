#include "rhoRocksample.h"
#include "utils.h"


using namespace UTILS;
using namespace std;


 RHOROCKSAMPLE::RHOROCKSAMPLE(int size, int rocks) : ROCKSAMPLE(size,rocks){
}


RHOROCKSAMPLE::~RHOROCKSAMPLE(){
}	





void RHOROCKSAMPLE::Rho_reward(STATE& s, BAG& beliefs, double& reward, int pos) const{

		reward = reward * beliefs.GetWeight(pos);
}


double RHOROCKSAMPLE::ProbObs(int observation, const STATE& startingState, int action, const STATE& finalState) const{
	const ROCKSAMPLE_STATE startRSstate = static_cast<const ROCKSAMPLE_STATE&>(startingState);
 	const ROCKSAMPLE_STATE finalRSstate = static_cast<const ROCKSAMPLE_STATE&>(finalState);
    // TODO
    return 0.0;
}
