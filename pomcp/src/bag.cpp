    #include "bag.h"
    #include "simulator.h"
    #include "utils.h"
    #include "tiger.h"
    #include <ctime>

    using namespace UTILS;

    //corretto
    BAG::BAG(){ Particles.clear(); weight.clear();}


    void BAG::Free(const SIMULATOR& simulator){ 

        for (std::vector<STATE*>::iterator i_state = Particles.begin(); i_state != Particles.end(); ++i_state)
            simulator.FreeState(*i_state);

        Particles.clear();
        weight.clear();

    }
/*
    //campiona uno stato e lo manda al chiamante
    STATE* BAG::CreateSample(const SIMULATOR& simulator) const {

        const int index = Random(Particles.size());
        const STATE* temp = (const STATE*)GetSample(index);
        return simulator.Copy(*GetSample(index));
    }
*/
    STATE* BAG::CreateSample(const SIMULATOR& simulator) const{
        //srand(time(NULL));

        double r = ((double) rand() / (RAND_MAX));
        //std::cout << "r= " << r << std::endl;
        for(int i=0; i< Particles.size() ; i++){
            if(r <= weight[i]){
                //std::cout << "ho scelto lo stato con weight= " << weight[i] << std::endl;
                //const STATE* temp = (const STATE*)GetSample(i);
                return simulator.Copy(*GetSample(i));
            } else{
                r = r-weight[i];
            }
        }


    }

    //aggiunge una particle alla bag
    void BAG::AddSample(STATE* particle, double peso){

        bool flag = true;
        for(int i = 0; i < Particles.size();++i){
            if( Particles[i]->isEqual(particle)){
                flag = false;
                weight[i] = weight[i]+peso;
                break;
            }
        }

        if(flag || Particles.empty()){
            Particles.push_back(particle);
            weight.push_back(peso);
        }

    }

    //aggiunge una particle alla bag
    void BAG::AddSample(STATE* particle){

        bool flag = true;
        for(int i = 0; i < Particles.size();++i){

            if( Particles[i]->isEqual(particle)){
                flag = false;
                weight[i]= weight[i]+1.0;
                break;
            }
        }

        if(flag){
            Particles.push_back(particle);
            weight.push_back(1.0);
        }

    }


    bool BAG::checkParticle(STATE* newstate){

        for(int i = 0; i < Particles.size();++i){

            if( Particles[i]->isEqual(newstate)){
                return true;
            }
        }
        return false;



    }

    void BAG::Copy(const BAG& particelle, const SIMULATOR& simulator){

        std::vector<STATE*> iterator = particelle.GetBag_State();
        int count=0;
        for(std::vector<STATE*>::const_iterator i = iterator.begin(); i!=iterator.end();++i){
            STATE* newstate = simulator.Copy(**i);
            if(checkParticle(newstate)){
                AddSample(newstate, particelle.GetWeight(count));
                simulator.FreeState(newstate);
            } else{
                AddSample(newstate, particelle.GetWeight(count));
            }
            count++;
        }  
          
    }


    void BAG::Display(std::ostream& ostr, const SIMULATOR& simulator) const{

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

    void BAG::normalize(){



        double sum = 0;
        for (int i=0;i<weight.size();i++){
            sum += weight[i];
        }

        for(int i=0;i<weight.size();i++){
            weight[i]= weight[i]/sum;
            //weight[i] = ((float)((int)(weight[i]*1000.0f)))/1000.0f; 
        }


    }