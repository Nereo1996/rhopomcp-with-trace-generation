#include "bag.h"
#include "simulator.h"
#include "utils.h"
#include "tiger.h"
#include <ctime>

using namespace UTILS;

BAG::BAG() : Particles(), weight(), normalized(false){ 
}

int BAG::insert =0;

void BAG::Free(const SIMULATOR& simulator){ 
    for (STATE * s: Particles) 
        simulator.FreeState(s);
    Particles.clear();
    weight.clear();
    normalized = false;
}

STATE* BAG::CreateSample(const SIMULATOR& simulator) const{
    double r = UTILS::RandomDouble(0.0, 1.0);
    for (int i = 0; i < Particles.size() ; i++) {
        if(r <= weight[i])
            return simulator.Copy(*GetSample(i));
        else
            r -= weight[i];
    }

    return nullptr; // dummy
}

//aggiunge una particle alla bag
void BAG::AddSample(STATE* particle, double peso, const SIMULATOR& simulator, bool count){
    assert(!normalized);
    if(count){
        insert++;
    }

    bool flag = true;
    for(int i = 0; i < Particles.size();++i){
        if( Particles[i]->isEqual(particle)){
            flag = false;
            weight[i] = weight[i]+peso;
            //simulator.FreeState(particle);
            break;
        }
    }
    if (flag) {
        Particles.push_back(simulator.Copy(*particle));
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
        simulator.DisplayState(*particle, std::cout);
        std::cout << "\nindirizzo 0 " << Particles[0]
                  << " indirizzo 1: " << Particles[1]
                  << " parametro: " << particle << std::endl;
        exit(1);
    }

}

//aggiunge una particle alla bag
void BAG::AddSample(STATE* particle, const SIMULATOR& simulator, bool count){
    AddSample(particle,1.0,simulator,count);
}


void BAG::AddSample(BAG& beta, const SIMULATOR& simulator, bool count) {
    for(int i =0;i < beta.GetNumSamples(); i++){
        bool flag = true;
        STATE* support = const_cast <STATE*> (beta.GetSample(i));
        AddSample(support,beta.GetWeight(i),simulator,count);
    }
    //beta.Free(simulator);
}

bool BAG::checkParticle(STATE* newstate) {
    for (int i = 0; i < Particles.size();++i) {
        if (Particles[i]->isEqual(newstate))
            return true;
    }
    return false;
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


int BAG::ParticlePosition(STATE& state){
    for (int i = 0; i < Particles.size(); ++i) {
        if (Particles[i]->isEqual(&state))
            return i;
    }
    return -1;
}
