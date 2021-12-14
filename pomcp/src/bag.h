#ifndef BAG_H
#define BAG_H

#include <vector>
#include <iostream>

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
    bool Empty() const { return Particles.empty(); }

    //numero di elementi in una bag
    int GetNumSamples() const { return Particles.size(); }

    //ritorna l'elemento in posizione index della bag
    const STATE &GetSample(int index) const { return *Particles[index]; }
    STATE &GetSample(int index) { return *Particles[index]; }

    //ritorna il peso associato all'elemento
    double GetWeight(int index) const{ return weight[index]; }
    
    //ritorna tutti gli stati delle particelle
    const std::vector<STATE*>& GetBag_State() const { return Particles; }

    //ritorna tutti i pesi delle particelle
    const std::vector<double>& GetBag_Weight() const {return weight;}

    void printInsert(){std::cout << "n. insert: " << insert << std::endl;}

    //normalizza i pesi
    void normalize();

    int ParticlePosition(const STATE& state) const;

    bool checkParticle(const STATE &newstate);
    
private:
    static constexpr bool count = true;

    std::vector<STATE*> Particles;
    std::vector<double> weight;

    bool normalized;

    int insert = 0;
};

#endif // BAG_H
