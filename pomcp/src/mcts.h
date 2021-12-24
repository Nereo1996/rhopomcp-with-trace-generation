#ifndef MCTS_H
#define MCTS_H

#include "bag.h"
#include "node.h"
#include "simulator.h"
#include "statistic.h"

class MCTS
{
public:

    struct PARAMS
    {
        PARAMS();

        int Verbose;
        int MaxDepth;
        int NumSimulations;
        int NumStartStates;
        bool UseTransforms;
        int NumTransforms;
        int MaxAttempts;
        int ExpandCount;
        double ExplorationConstant;
        bool UseRave;
        double RaveDiscount;
        double RaveConstant;
        bool DisableTree;
        int bagsize;
    };

    MCTS(const SIMULATOR& simulator, const PARAMS& params);
    ~MCTS();

    void Free();

    int SelectAction();
    bool Update(int action, int observation, double reward);

    void UCTSearch();
    void RolloutSearch();

    double Rho_Rollout(STATE& state, BAG &bag);

    const BAG& BeliefState() const { return Root->Beliefs(); }
    const HISTORY& GetHistory() const { return History; }
    const SIMULATOR::STATUS& GetStatus() const { return Status; }
    void ClearStatistics();
    void DisplayStatistics(std::ostream& ostr) const;
    void DisplayValue(int depth, std::ostream& ostr) const;
    void DisplayPolicy(int depth, std::ostream& ostr) const;

    static void InitFastUCB(double exploration);

private:

    const SIMULATOR& Simulator;
    int TreeDepth, PeakTreeDepth;
    PARAMS Params;
    VNODE* Root;
    HISTORY History;
    SIMULATOR::STATUS Status;

    STATISTIC StatTreeDepth;
    STATISTIC StatRolloutDepth;
    STATISTIC StatTotalReward;

    int GreedyUCB(VNODE* vnode, bool ucb) const;
    int SelectRandom() const;
    void AddRave(VNODE* vnode, double totalReward);
    VNODE* ExpandNode(const STATE* state);
    void AddSample(VNODE* node, const STATE& state);
    void AddTransforms(VNODE* root, BAG& beliefs);
    STATE* CreateTransform() const;
    void Resample(BAG& beliefs);

    //for bag
    BAG CreateBag_beta(const STATE& previous, int action, int observation, const BAG& bag, const STATE &next);
    double SimulateV_rho(STATE& state, VNODE* vnode, BAG& bag);
    double SimulateQ_rho(STATE& state, QNODE& qnode, int action, BAG& bag, BAG& prev );
    BAG generateInitialBag(STATE* state, const BAG& initialBelief);

    // Fast lookup table for UCB
    static const int UCB_N = 10000, UCB_n = 100;
    static double UCB[UCB_N][UCB_n];
    static bool InitialisedFastUCB;

    double FastUCB(int N, int n, double logN) const;

};

#endif // MCTS_H
