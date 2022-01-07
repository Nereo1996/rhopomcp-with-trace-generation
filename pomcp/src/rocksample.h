#ifndef ROCKSAMPLE_H
#define ROCKSAMPLE_H

#include "simulator.h"
#include "coord.h"
#include "grid.h"




class ROCKSAMPLE_STATE : public STATE
{
public:

    COORD AgentPos;
    struct ENTRY
    {
        bool Valuable;
        bool Collected;
        int Count;    				// Smart knowledge
        int Measured; 				// Smart knowledge
        double LikelihoodValuable;	// Smart knowledge
        double LikelihoodWorthless;	// Smart knowledge
        double ProbValuable;		// Smart knowledge
    };

    virtual bool isEqual(const STATE &) const;

    std::vector<ENTRY> Rocks;
    std::string view;
    int Target; // Smart knowledge
};

class ROCKSAMPLE : public SIMULATOR
{
public:

    struct ROCKSAMPLEPARAM{

        double rNorth = -100;
        double rSouth = -100;
        double rWest = -100;
        double rEast = +10;

        double rValuable = +100;
        double rNotValuable = -10;

        double rAlreadySampled = -100;

        double HalfEfficiencyDistance = 20;
    };


    ROCKSAMPLE(int size, int rocks, const ROCKSAMPLEPARAM& rsParam);

    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action,
        int& observation, double& reward) const;

    void GenerateLegal(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    void GeneratePreferred(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    virtual bool LocalMove(STATE& state, const HISTORY& history,
        int stepObservation, const STATUS& status) const;

    virtual void DisplayBeliefs(const BAG& beliefState,
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

    virtual int reward(const STATE& state, int action) const;
    virtual double Rho_reward(const BAG& belief, int action) const;

    virtual double ProbObs(int observation, const STATE& startingState, int action, const STATE& finalState) const;

    //functions for xes logging
    virtual void log_problem_info() const;
    virtual void log_beliefs(const BAG& beliefState) const;
    virtual void log_state(const STATE& state) const;
    virtual void log_action(int action) const;
    //log observation: changed observation_t in int. idk if it's right at the moment.
    virtual void log_observation(const STATE&, int observation) const;
    virtual void log_reward(double reward) const;

protected:

    enum
    {
        E_NONE,
        E_GOOD,
        E_BAD
    };

    enum
    {
        E_SAMPLE = 4
    };

    void InitGeneral();
    void Init_7_8();
    void Init_11_11();
    int GetObservation(const ROCKSAMPLE_STATE& rockstate, int rock) const;
    int SelectTarget(const ROCKSAMPLE_STATE& rockstate) const;

    GRID<int> Grid;
    std::vector<COORD> RockPos;
    int Size, NumRocks;
    COORD StartPos;
    //double HalfEfficiencyDistance;
    double SmartMoveProb;
    int UncertaintyCount;

private:
    ROCKSAMPLEPARAM RsParam;
    mutable MEMORY_POOL<ROCKSAMPLE_STATE> MemoryPool;

};

#endif
