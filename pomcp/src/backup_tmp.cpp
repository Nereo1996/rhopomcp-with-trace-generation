BAG MCTS::CreateBag(VNODE*& bag_successiva,STATE& previous, int action, int& observation, double& immediateReward, BAG& bag, STATE& next){
        /*
            // (1) sample a state s from βt
            STATE* s_bag = bag.CreateSample(Simulator);
            STATE* s_particle = Simulator.Copy(*s_bag);
            int temp_obs = observation;

            //(2) sample a state s' by using the generative model, s' ∼ G(s, at)
            bool terminal= Simulator.Step(*s_particle, action, temp_obs, immediateReward);

            //(3) store this particle s' in βt+1 with an associated weight of P (zt+1 |s, at , s')
            // corresponding to the probability of having generated observation zt+1.
            double probability = Simulator.ProbObs(temp_obs,*s_bag,action, *s_particle);
            //int peso = probability * NUM_PARTICLES;

            AddSample_Bag(bag_successiva,s_particle,probability);
        */
            BAG* generatedBag;

            BAG normalized;
            normalized.Copy(bag, Simulator);

            normalized.normalize();


            generatedBag->Copy(bag ,Simulator);


            
            generatedBag->normalize();


            //cout << "bag normalizzata:" << endl;
            //generatedBag.Display(std::cout,Simulator);


            for (int i =0; i < NUM_PARTICLES; i++){
                // (1) sample a state s from βt
                STATE* s_bag = normalized.CreateSample(Simulator);
                STATE* s_particle = Simulator.Copy(*s_bag);
                int temp_obs = observation;

                //(2) sample a state s' by using the generative model, s' ∼ G(s, at)
                bool terminal= Simulator.Step(*s_particle, action, temp_obs, immediateReward);

                //(3) store this particle s' in βt+1 with an associated weight of P (zt+1 |s, at , s')
                // corresponding to the probability of having generated observation zt+1.
                double probability = Simulator.ProbObs(temp_obs,*s_bag, action, *s_particle);

                //bag_successiva =ExpandNode(s_particle);

                if(probability >0){
                    generatedBag->AddSample(s_particle,probability);
                    //AddSample_Bag(generatedBag,*s_particle,probability);
                }
            }
           
            double probability = Simulator.ProbObs(observation,previous, action, next);
            //AddSample_Bag(generatedBag,*next,probability);
            generatedBag->AddSample(&next,probability);

             cout << "bag successiva:" << endl;
            generatedBag->Display(std::cout, Simulator);


            if (generatedBag->Empty())
                cout << "no particles in the generated BAG" << endl;

            cout << "qua ci sei???????" << endl;
            generatedBag->Display(std::cout, Simulator);

            bag_successiva->Bags().Move(*generatedBag, Simulator);

            return *generatedBag;
        }



        BAG MCTS::generateInitialBag(STATE* state, const BELIEF_STATE& initialBelief){

            // create empty bag
            BAG bag;

        // add state s to the bag
            bag.AddSample(state, 1.0);

        // add sampled states from initialbelief (nbWished times)
        for (int i = 0; i < NUM_PARTICLES; i++) {
            STATE* sampledS = initialBelief.CreateSample(Simulator);
            bag.AddSample(sampledS, 1.0);
        }

        return bag;
    }