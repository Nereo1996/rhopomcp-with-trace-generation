#include "bag.h"
#include "simulator.h"
#include "utils.h"
#include "tiger.h"
#include <ctime>

using namespace UTILS;

BAG::BAG() : Particles(), weight(), normalized(false){ 
}

void BAG::Free(const SIMULATOR& simulator){ 
    for (STATE * s: Particles) 
        simulator.FreeState(s);
    Particles.clear();
    weight.clear();
    normalized = false;
}

STATE* BAG::CreateSample(const SIMULATOR& simulator) const{
    assert(normalized);

    double r = UTILS::RandomDouble(0.0, 1.0);
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
    assert(!normalized);
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

    if (Particles.size() > 2) {
        std::cout << "\naddsample con particle e peso" << std::endl;
        std::cout << "error: size= " << Particles.size() << std::endl;
        std::cout << "check0: " << Particles[0]->isEqual(particle)
                  << " check 1: " << Particles[1]->isEqual(particle)
                  << std::endl;
        simulator.DisplayState(*Particles[0], std::cout);
        std::cout << " -- ";
        simulator.DisplayState(particle, std::cout);
        std::cout << "\nindirizzo 0 " << Particles[0]
                  << " indirizzo 1: " << Particles[1]
                  << " parametro: " << &particle << std::endl;
        exit(1);
    }
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
    normalized = bag.normalized;
}

void BAG::Move(BAG& b, const SIMULATOR& simulator) {
    std::swap(Particles, b.Particles);
    std::swap(weight, b.weight);
    std::swap(normalized, b.normalized);

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

void BAG::normalize(){
    double sum = 0;
    for (int i=0;i<weight.size();i++)
        sum += weight[i];
    for(int i=0;i<weight.size();i++)
        weight[i] /= sum;

    normalized = true;
}

int BAG::ParticlePosition(const STATE& state) const {
    for (int i = 0; i < Particles.size(); ++i) {
        if (Particles[i]->isEqual(state))
            return i;
    }
    return -1;
}
