#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "history.h"
#include "node.h"
#include "bag.h"
#include "XES_logger.h"
//#include "beliefstate.h"
#include "utils.h"
#include <iostream>
#include <math.h>

class BELIEF_STATE;

class STATE : public MEMORY_OBJECT
{
public:
    virtual bool isEqual(const STATE &) const {return false;};
};

class SIMULATOR
{
public:

    struct KNOWLEDGE
    {
        enum
        {
            PURE,
            LEGAL,
            SMART,
            NUM_LEVELS
        };

        KNOWLEDGE();
        
        int RolloutLevel;
        int TreeLevel;
        int SmartTreeCount;
        double SmartTreeValue;
        
        int Level(int phase) const
        {
            assert(phase < STATUS::NUM_PHASES);
            if (phase == STATUS::TREE)
                return TreeLevel;
            else
                return RolloutLevel;
        }
    };

    struct STATUS
    {
        STATUS();
        
        enum
        {
            TREE,
            ROLLOUT,
            NUM_PHASES
        };
        
        enum
        {
            CONSISTENT,
            INCONSISTENT,
            RESAMPLED,
            OUT_OF_PARTICLES
        };
        
        int Phase;
        int Particles;
    };

    SIMULATOR();
    SIMULATOR(int numActions, int numObservations, double discount = 1.0);    
    virtual ~SIMULATOR();

    // Create start start state (can be stochastic)
    virtual STATE* CreateStartState() const = 0;

    // Free memory for state
    virtual void FreeState(STATE*) const = 0;

    // Update state according to action, and get observation and reward. 
    // Return value of true indicates termination of episode (if episodic)
    virtual bool Step(STATE& , int , 
        int& , double& ) const = 0;

    // Create new state and copy argument (must be same type)
    virtual STATE* Copy(const STATE& ) const = 0;
    
    // Sanity check
    virtual void Validate(const STATE& ) const;

    // Modify state stochastically to some related state
    virtual bool LocalMove(STATE& , const HISTORY& ,
        int , const STATUS& ) const;

    // Use domain knowledge to assign prior value and confidence to actions
    // Should only use fully observable state variables
    void Prior(const STATE* state, const HISTORY& history, VNODE* vnode,
        const STATUS& status) const;

    // Use domain knowledge to select actions stochastically during rollouts
    // Should only use fully observable state variables
    int SelectRandom(const STATE& state, const HISTORY& history,
        const STATUS& status) const;

    // Generate set of legal actions
    virtual void GenerateLegal(const STATE& state, const HISTORY& history, 
        std::vector<int>& actions, const STATUS& status) const;

    // Generate set of preferred actions
    virtual void GeneratePreferred(const STATE& state, const HISTORY& history, 
        std::vector<int>& actions, const STATUS& status) const;

    // For explicit POMDP computation only
    virtual bool HasAlpha() const;
    virtual void AlphaValue(const QNODE& qnode, double& q, int& n) const;
    virtual void UpdateAlpha(QNODE& qnode, const STATE& state) const;

    // Textual display
    virtual void DisplayBeliefs(const BAG& beliefState, 
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;
    virtual void DisplayReward(double reward, std::ostream& ostr) const;

    // xes logging
    virtual void log_problem_info() const {}
    virtual void log_beliefs(const BAG& beliefState) const {}
    virtual void log_state(const STATE& state) const {}
    virtual void log_action(int action) const {}
    //log observation: changed observation_t in int. idk if it's right at the moment.
    virtual void log_observation(const STATE& state, int observation) const {}
    virtual void log_reward(double reward) const {}    

    // Accessors
    void SetKnowledge(const KNOWLEDGE& knowledge) { Knowledge = knowledge; }
    int GetNumActions() const { return NumActions; }
    int GetNumObservations() const { return NumObservations; }
    bool IsEpisodic() const { return false; }
    double GetDiscount() const { return Discount; }
    double GetRewardRange() const { return RewardRange; }
    double GetHorizon(double accuracy, int undiscountedHorizon = 100) const;


    //Probabilità di aver osservato Z_t+1 dallo stato s_t con azione a_t e essere andati in s_t+1
    virtual double ProbObs(int , const STATE& , int , const STATE& ) const{ return 1; }

    virtual int reward(const STATE&, int ) const {
        assert(false); // not implemented
        return 0;
    };

    virtual int observation(STATE&, int) const {
        assert(false); // not implemented
        return 0;
    };

    //assegnamento reward caso rho
    virtual double Rho_reward(const BAG& , int) const {
        assert(false); // not implemented
        return 0.0;
    };
    
protected:
    int NumActions, NumObservations;
    double Discount, RewardRange;
    KNOWLEDGE Knowledge;
};

#endif // SIMULATOR_H
