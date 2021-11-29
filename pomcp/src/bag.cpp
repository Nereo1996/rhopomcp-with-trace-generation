    #include "bag.h"
    #include "simulator.h"
    #include "utils.h"
    #include "tiger.h"
    #include <ctime>

    using namespace UTILS;
    
    static int counter =0;
    //corretto
    BAG::BAG(){ Particles.clear(); weight.clear(); normalized = false;}


    void BAG::Free(const SIMULATOR& simulator){ 

        for (std::vector<STATE*>::iterator i_state = Particles.begin(); i_state != Particles.end(); ++i_state){
            //simulator.FreeState(*i_state);
            counter--;
        }

        Particles.clear();
        weight.clear();

    }

    STATE* BAG::CreateSample(const SIMULATOR& simulator) const{
        //srand(time(NULL));
        double r = ((double) rand() / (RAND_MAX));
        for(int i=0; i< Particles.size() ; i++){
            if(r <= weight[i]){
                return simulator.Copy(*GetSample(i));
            } else{
                r = r-weight[i];
            }
        }


    }
    void BAG::print_addedSample(){
        std::cout << "counter = " << counter<< std::endl;
    }
    //aggiunge una particle alla bag
    void BAG::AddSample(STATE* particle, double peso, const SIMULATOR& simulator){

        bool flag = true;
        for(int i = 0; i < Particles.size();++i){
            if( Particles[i]->isEqual(particle)){
                flag = false;
                weight[i] = weight[i]+peso;
               // simulator.FreeState(particle);
                break;
            }
        }

        if(flag){
            counter++;
            Particles.push_back(particle);
            weight.push_back(peso);
        }

    }

    //aggiunge una particle alla bag
    void BAG::AddSample(STATE* particle, const SIMULATOR& simulator){

        bool flag = true;
        for(int i = 0; i < Particles.size();++i){

            if( Particles[i]->isEqual(particle)){
                flag = false;
                weight[i]= weight[i]+1.0;
               // simulator.FreeState(particle);
                break;
            }

        }

        if(flag){
           // std::cout << "BAG::AddSample(STATE* particle, const SIMULATOR& simulator) non aveva lo stato corretto" << std::endl;
            counter++;
            Particles.push_back(particle);
            weight.push_back(1.0);
        }

    }


    void BAG::AddSample(BAG& beta, const SIMULATOR& simulator){
        bool flag = true;


        for(int i =0;i < beta.GetNumSamples(); i++){

            for(int j = 0; j < Particles.size();++j){

                if(Particles[j]->isEqual(beta.GetSample(i))){
                    flag = false;
                    weight[j]= weight[j]+beta.GetWeight(i);
                }


            }
            if(flag){
                STATE* support = const_cast <STATE*> (beta.GetSample(i));
                //const STATE* support = beta.GetSample(i);
                Particles.push_back(support);
                weight.push_back(beta.GetWeight(i));
            } else{
                flag = true;
            }


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

        Free(simulator);
        const std::vector<STATE*>& iterator = particelle.GetBag_State();
        int count=0;
        for(std::vector<STATE*>::const_iterator i = iterator.begin(); i!=iterator.end();++i){
            STATE* newstate = simulator.Copy(**i);
            AddSample(newstate, particelle.GetWeight(count),simulator);
            count++;
        }  
          
    }


    void BAG::Move(BAG& particelle, const SIMULATOR& simulator){

        std::vector<STATE*>& iterator = particelle.GetBag_State();
        int count =0;

        for(std::vector<STATE*>::const_iterator i = iterator.begin(); i!=iterator.end();++i){
            AddSample(*i,particelle.GetWeight(count),simulator);
            count++;
    }

        particelle.Free(simulator);

    }



    void BAG::Display(std::ostream& ostr, const SIMULATOR& simulator) const{

        if(Particles.size()==0)
            return;

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


    int BAG::ParticlePosition(STATE& state){
        for(int i =0; i < Particles.size(); ++i){
            if( Particles[i]->isEqual(&state) ){
                return i;
            }
        }

        return -1;
    }

    /*
    //campiona uno stato e lo manda al chiamante
    STATE* BAG::CreateSample(const SIMULATOR& simulator) const {

        const int index = Random(Particles.size());
        const STATE* temp = (const STATE*)GetSample(index);
        return simulator.Copy(*GetSample(index));
    }
*/