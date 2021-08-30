#include "bag.h"
#include "simulator.h"
#include "utils.h"
#include "tiger.h"
#include <ctime>

using namespace UTILS;

//srand(time(NULL));






//corretto
BAG::BAG(){ Particles.clear(); weight.clear();}


void BAG::Free(const SIMULATOR& simulator){ 

    for (std::vector<STATE*>::iterator i_state = Particles.begin(); i_state != Particles.end(); ++i_state)
    {
        simulator.FreeState(*i_state);
    }
    Particles.clear();
   
    weight.clear();
}



//campiona uno stato e lo manda al chiamante
STATE* BAG::CreateSample(const SIMULATOR& simulator) const {


    const int index = Random(Particles.size());
    const STATE* temp = (const STATE*)GetSample(index);
    //std::cout << "index, temp = " << index << std::endl;

    return simulator.Copy(*GetSample(index));


}

//aggiunge una particle alla bag
void BAG::AddSample(STATE* particle, int peso){

    //std::cout << "faccio addsample" << std::endl;

    bool flag = true;
    for(int i = 0; i < Particles.size();++i){
        if( Particles[i]->isEqual(particle)){
            //std::cout << "Particles[i]: " << Simulator.DisplayState(Particles[i],std::cout) << "particle: " << Simulator.DisplayState(particle,std::cout) << std::endl;
            flag = false;
            weight[i] = weight[i]+peso;
            break;
        }
    }

    if(flag || Particles.empty()){
        Particles.push_back(particle);
        weight.push_back(peso);
    }





   // Particles.clear();
   // Particles.push_back(particle);

    //weight.push_back(peso);

   //std::cout << "finito addsample" << std::endl;


}

//aggiunge una particle alla bag
void BAG::AddSample(STATE* particle){


    bool flag = true;
    for(int i = 0; i < Particles.size();++i){

        if( Particles[i]->isEqual(particle)){
            //std::cout << "Particles[i]: " << Simulator.DisplayState(Particles[i],std::cout) << "particle: " << Simulator.DisplayState(particle,std::cout) << std::endl;
            flag = false;
            weight[i]++;
            break;
        }
    }

    if(flag || Particles.empty()){
        Particles.push_back(particle);
        weight.push_back(1);
    }
}




void BAG::Copy(const BAG& particelle, const SIMULATOR& simulator){

    std::vector<STATE*> iterator = particelle.GetBag_State();
    int count=0;
    for(std::vector<STATE*>::const_iterator i = iterator.begin(); i!=iterator.end();++i){
        //std::cout << "copia venuta da bag.copy" << std::endl;
        AddSample(simulator.Copy(**i), particelle.GetWeight(count));
        count++;
    }

}

void BAG::Display(std::ostream& ostr, const SIMULATOR& simulator) const{

    //std::cout << "Particle.size(): " << Particles.size() << std::endl;
    ostr << "bag: \n";
    for(int i=0;i!=Particles.size();++i){
        ostr << "[ ";
        simulator.DisplayState(*Particles[i],ostr);
        ostr << ",PESO: ";
        ostr <<  GetWeight(i) << " ]\n";
    }
    if(!Particles.empty())
        ostr << std::endl;
}


void BAG::Move(BAG& particelle, const SIMULATOR& simulator){

std::vector<STATE*> iterator = particelle.GetBag_State();
int count =0;

for(std::vector<STATE*>::const_iterator i = iterator.begin(); i!=iterator.end();++i){
    AddSample(*i,particelle.GetWeight(count));
    count++;
}

    particelle.Free(simulator);


}

