#ifndef BAG_H
#define BAG_H

#include <vector>
#include <iostream>
#include <unordered_map>

class STATE;
class SIMULATOR;


class BAG
{
public:

    BAG();

    // libera memoria
    void Free(const SIMULATOR& simulator);

    // estrae uno particle dalla bag 
    STATE* CreateSample(const SIMULATOR& simulator) const;
    
    //aggiunge uno stato alla bag di particle
    void AddSample(const SIMULATOR& simulator, const STATE &particle, double peso = 1.0);

    //aggiunge una bag alla cumulative BAG
    void AddSample(const SIMULATOR& simulator, const BAG &beta);

    //copia un'intera bag
    void Copy(const BAG& particelle, const SIMULATOR& simulator);

    //sposta un'intera bag
    void Move(BAG& particelle, const SIMULATOR& simulator);

    //stampa la bag
    void Display(std::ostream& ostr, const SIMULATOR& simulator) const;

    //controlla se la bag contiene qualche elemento
    bool Empty() const { return container.empty(); }

    //numero di elementi in una bag
     int GetNumSamples() const { return container.size(); }

    //ritorna l'elemento in posizione index della bag
    const STATE *GetFirstSample() const { return container.begin()->first; }

    //ritorna il peso normalizzato associato all'elemento
    double GetNormalizedWeight(STATE* state) const{ return container.at(state)/totalWeight; }


    //ritorna il peso totale di tutti gli elementi della bag
    double GetTotalWeight() const{ return totalWeight; }


    //ritorna l'intera bag
    const std::unordered_map<STATE*,double> &getContainer() const{ return container;}



private:
    double totalWeight;
    std::unordered_map<STATE*,double> container;




};

#endif // BAG_H
