#ifndef BAG_H
#define BAG_H

#include <vector>
#include <set>
#include <iostream>

class STATE;
class SIMULATOR;

class BAG
{
public:

    BAG();
    //~BAG();


    // libera memoria
    void Free(const SIMULATOR& simulator);

    // estrae uno particle dalla bag 
    //std::pair<STATE*,int> CreateSample(const SIMULATOR& simulator) const;
    STATE* CreateSample(const SIMULATOR& simulator) const;

    

    //aggiunge uno stato alla bag di particle
    void AddSample(STATE* particle);

    //aggiunge uno stato alla bag di particle
    void AddSample(STATE* particle, double peso);


    //copia un'intera bag
    void Copy(const BAG& particelle, const SIMULATOR& simulator);

    //sposta un'intera bag
    void Move(BAG& particelle, const SIMULATOR& simulator);

    //stampa la bag
    void Display(std::ostream& ostr, const SIMULATOR& simulator) const;

    //controlla se la bag contiene qualche elemento
    bool Empty() const { return Particles.empty(); }

    //numero di elementi in una bag
    int GetNumSamples() const { return Particles.size(); }

    //ritorna l'elemento in posizione index della bag
    const STATE* GetSample(int index) const { return Particles[index]; }


    //ritorna il peso associato all'elemento
    const double GetWeight(int index) const{ return weight[index]; }
    
    //ritorna tutti gli stati delle particelle
    const std::vector<STATE*>& GetBag_State() const { return Particles; }
    std::vector<STATE*>& GetBag_State() { return Particles; }

    //ritorna tutti i pesi delle particelle
    const std::vector<double>& GetBag_Weight() const {return weight;}
    std::vector<double>& GetBag_Weight() {return weight;}

    //normalizza i pesi
    void normalize();
    
private:

    std::vector<STATE*> Particles;
    std::vector<double> weight;
};

#endif // BAG_H
