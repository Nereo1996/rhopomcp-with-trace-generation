#include "bag.h"
#include "simulator.h"
#include "utils.h"
#include "tiger.h"
#include <ctime>

using namespace UTILS;


BAG::BAG() : totalWeight(0), container(){ 
}

void BAG::Free(const SIMULATOR& simulator){ 

    for (auto& element : container){
        simulator.FreeState(element.first);
    }
    container.clear();
    totalWeight= 0;
}

STATE* BAG::CreateSample(const SIMULATOR& simulator) const{

    double r = UTILS::RandomDouble(0.0, totalWeight);

    for (auto& element : container){
        if(r <= element.second)
            return simulator.Copy(*element.first);
        else
            r -= element.second;
    }

    return nullptr; // dummy
}


//aggiunge una particle alla bag
void BAG::AddSample(const SIMULATOR& simulator, const STATE &particle, double peso) {
    if (count) insert++;
    bool is_new = true;
    /*
    if(container.contains(support)){
        container[support] += peso;
    }
    else{
        container.insert({simulator.Copy(*support),peso});
    }
    */
    for (auto& element : container){
        if(element.first->isEqual(particle)){
            is_new = false;
            element.second+=peso;
            break;
        }
    }
    if(is_new){
        STATE* support = const_cast <STATE*> (&particle);
        container.insert({simulator.Copy(*support),peso});
    }

    totalWeight += peso;

}

void BAG::AddSample(const SIMULATOR& simulator, const BAG &beta) {
    for (auto& element : beta.getContainer()){
        AddSample(simulator,*element.first,element.second);
    }

}

void BAG::Copy(const BAG& bag, const SIMULATOR& simulator) {
    Free(simulator);

    for (auto& element : bag.getContainer()){
        container.insert({simulator.Copy(*element.first),element.second});
    }

    totalWeight = bag.totalWeight;
}

void BAG::Move(BAG& b, const SIMULATOR& simulator) {
    std::swap(container, b.container);
    std::swap(totalWeight, b.totalWeight);
    b.Free(simulator);
}

void BAG::Display(std::ostream& ostr, const SIMULATOR& simulator) const{

    if(container.empty())
        return;

    ostr << "bag: \n";
    for (auto& element : container){
        ostr << "[ ";
        simulator.DisplayState(*element.first,ostr);
        ostr << " PESO: " << element.second << " ]\n";
    }
    
    ostr << std::endl;

}

