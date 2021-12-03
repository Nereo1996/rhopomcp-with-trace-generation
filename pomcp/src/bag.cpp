    #include "bag.h"
    #include "simulator.h"
    #include "utils.h"
    #include "tiger.h"
    #include <ctime>

    using namespace UTILS;
    
    BAG::BAG(){ 
        Particles.clear(); 
        weight.clear();
        normalized = false;
    }


    void BAG::Free(const SIMULATOR& simulator){ 

        for (std::vector<STATE*>::iterator i_state = Particles.begin(); i_state != Particles.end(); ++i_state){
            simulator.FreeState(*i_state);
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


    //aggiunge una particle alla bag
    void BAG::AddSample(STATE* particle, double peso, const SIMULATOR& simulator){

        bool flag = true;
        for(int i = 0; i < Particles.size();++i){
            if( Particles[i]->isEqual(particle)){
                flag = false;
                weight[i] = weight[i]+peso;
                simulator.FreeState(particle);
                break;
            }
        }

        if(flag){
            Particles.push_back(simulator.Copy(*particle));
            weight.push_back(peso);
        }

           if(Particles.size()>2){

            std::cout << "\naddsample con particle e peso" << std::endl;
            std::cout << "error: size= " << Particles.size()<< std::endl;
            std::cout << "check0: " << Particles[0]->isEqual(particle) <<  " check 1: " <<Particles[1]->isEqual(particle) << std::endl;
            simulator.DisplayState(*Particles[0],std::cout);
            std::cout << " -- ";
            simulator.DisplayState(*particle,std::cout);
            std::cout << "\nindirizzo 0 " << Particles[0] << " indirizzo 1: " << Particles[1] <<" parametro: " << particle<<std::endl;
            exit(1);

        }

    }

    //aggiunge una particle alla bag
    void BAG::AddSample(STATE* particle, const SIMULATOR& simulator){

        /*
        bool flag = true;
        for(int i = 0; i < Particles.size();++i){

            if( Particles[i]->isEqual(particle)){
                flag = false;
                weight[i]= weight[i]+1.0;
                simulator.FreeState(particle);
                break;
            }

        }

        if(flag){
            Particles.push_back(particle);
            weight.push_back(1.0);
        }

        */
        AddSample(particle,1.0,simulator);

        
        if(Particles.size()>2){
            std::cout << "\naddsample solo particle" << std::endl;
            std::cout << "error: size= " << Particles.size()<< std::endl;
            std::cout << "check0: " << Particles[0]->isEqual(particle) <<  " check 1: " <<Particles[1]->isEqual(particle) << std::endl;
            simulator.DisplayState(*Particles[0],std::cout);
            std::cout << " -- ";
            simulator.DisplayState(*particle,std::cout);
            std::cout << "indirizzo 0 " << Particles[0] << " indirizzo 1: " << Particles[1] <<" parametro: " << particle<<std::endl;
            exit(1);

        }

    }


    void BAG::AddSample(BAG& beta, const SIMULATOR& simulator){

        for(int i =0;i < beta.GetNumSamples(); i++){
            bool flag = true;
/*
            for(int j = 0; j < Particles.size();++j){

                if(Particles[j]->isEqual(beta.GetSample(i))){
                    flag = false;
                    weight[j]= weight[j]+beta.GetWeight(i);
                    break;
                }

            }
            if(flag){
                //STATE* support = const_cast <STATE*> (beta.GetSample(i));
                Particles.push_back(simulator.Copy(*beta.GetSample(i)));
                weight.push_back(beta.GetWeight(i));
            }
*/
            AddSample(simulator.Copy(*beta.GetSample(i)),beta.GetWeight(i),simulator);
               
            if(Particles.size()>2){
                std::cout << "\naddsample con bag" << std::endl;
                std::cout << "error: size cumulative= " << Particles.size()<< " size della bag: " << beta.GetNumSamples() <<std::endl;
                std::cout << "check0: " << Particles[0]->isEqual(beta.GetSample(i)) <<  " check 1: " <<Particles[1]->isEqual(beta.GetSample(i)) << std::endl;
                simulator.DisplayState(*Particles[0],std::cout);
                std::cout << " -- ";
                simulator.DisplayState(*beta.GetSample(i),std::cout);
                std::cout << "indirizzo 0 " << Particles[0] << " indirizzo 1: " << Particles[1] <<" parametro: " << beta.GetSample(i)<<std::endl;
                exit(1);

            }
        }

        beta.Free(simulator);

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
