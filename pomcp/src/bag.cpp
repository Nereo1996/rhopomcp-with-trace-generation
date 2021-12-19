#include "bag.h"
#include "simulator.h"
#include "utils.h"
#include "tiger.h"
#include <ctime>

using namespace UTILS;

BAG::BAG() : Particles(), weight(), totalWeight(0){ 
}

void BAG::Free(const SIMULATOR& simulator){ 
    for (STATE * s: Particles) 
        simulator.FreeState(s);
    Particles.clear();
    weight.clear();
    totalWeight= 0;
}

STATE* BAG::CreateSample(const SIMULATOR& simulator) const{

    double r = UTILS::RandomDouble(0.0, totalWeight);
    for (int i = 0; i < Particles.size() ; i++) {
        if(r <= weight[i])
            return simulator.Copy(GetSample(i));
        else
            r -= weight[i];
    }
    return nullptr; // dummy
}


//aggiunge una particle alla bag
void BAG::AddSample(const SIMULATOR& simulator, const STATE &particle, double peso) {
    if (count) insert++;

    bool is_new = true;
    for(int i = 0; i < Particles.size();++i){
        if (Particles[i]->isEqual(particle)) {
            is_new = false;
            weight[i] += peso;
            break;
        }
    }
    if (is_new) {
        Particles.push_back(simulator.Copy(particle));
        weight.push_back(peso);
    }
    
    totalWeight += peso;

}

void BAG::AddSample(const SIMULATOR& simulator, const BAG &beta) {
    for (int i = 0; i < beta.GetNumSamples(); i++)
        AddSample(simulator, beta.GetSample(i), beta.GetWeight(i));
}

void BAG::Copy(const BAG& bag, const SIMULATOR& simulator) {
    Free(simulator);

    for (STATE * s: bag.Particles)
        Particles.push_back(simulator.Copy(*s));
    weight = bag.weight;
    totalWeight = bag.totalWeight;
}

void BAG::Move(BAG& b, const SIMULATOR& simulator) {
    std::swap(Particles, b.Particles);
    std::swap(weight, b.weight);
    std::swap(totalWeight, b.totalWeight);

    b.Free(simulator);
}

void BAG::Display(std::ostream& ostr, const SIMULATOR& simulator) const{
    if(Particles.empty())
        return;

    ostr << "bag: \n";
    for(int i=0;i!=Particles.size();++i){
        ostr << "[ ";
        simulator.DisplayState(*Particles[i],ostr);
        ostr << ",PESO: ";
        ostr <<  GetWeight(i) << " ]\n";
    }
    ostr << std::endl;
}

int BAG::ParticlePosition(const STATE& state) const {
    for (int i = 0; i < Particles.size(); ++i) {
        if (Particles[i]->isEqual(state))
            return i;
    }
    return -1;
}
