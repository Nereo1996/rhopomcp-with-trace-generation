#include "mcts.h"
#include <math.h>
#include <algorithm>
#include "sys/types.h"
#include "sys/sysinfo.h"



using namespace std;
using namespace UTILS;

//-----------------------------------------------------------------------------

static constexpr bool PRINT_VALUES = false;
static constexpr bool PRINT_IT = false;
static constexpr bool PRINT_DEBUG_TREE = false;


MCTS::PARAMS::PARAMS() :
    Verbose(0),
    MaxDepth(100),
    NumSimulations(1000),
    NumStartStates(1000),
    UseTransforms(true),
    NumTransforms(0),
    MaxAttempts(0),
    ExpandCount(1),
    ExplorationConstant(1),
    UseRave(false),
    RaveDiscount(1.0),
    RaveConstant(0.01),
    bagsize(50),
    DisableTree(false)
{
}

using std::cout;
using std::endl;

MCTS::MCTS(const SIMULATOR& simulator, const PARAMS& params)
    :   Simulator(simulator),
    Params(params),
    TreeDepth(0)
{
    VNODE::NumChildren = Simulator.GetNumActions();
    QNODE::NumChildren = Simulator.GetNumObservations();

    Root = ExpandNode(Simulator.CreateStartState());

    //creazione della prima bag di particelle
    for (int i = 0; i < Params.NumStartStates; i++) {
        STATE* s = Simulator.CreateStartState();
        Root->Beliefs().AddSample(Simulator, *s);
        Simulator.FreeState(s);
    }

}

MCTS::~MCTS()
{
    VNODE::Free(Root, Simulator);
    VNODE::FreeAll();
}

bool MCTS::Update(int action, int observation, double reward)
{
    History.Add(action, observation);
    BAG belief;

    // Find matching vnode from the rest of the tree
    QNODE& qnode = Root->Child(action);
    VNODE* vnode = qnode.Child(observation);

    if (XES::enabled()){
        XES::logger().start_event();
        XES::logger().add_attribute({"even type", "Root" });
        Simulator.log_beliefs(Root->Beliefs(), -1, observation);
        Simulator.log_action(action);
        Simulator.log_observation(*Root->Beliefs().GetFirstSample(), observation);
        Simulator.log_reward(reward);

        XES::logger().start_list("Valuable actions");
        for (int a = 0; a < Simulator.GetNumActions(); a++){
            Simulator.log_action(a);
            XES::logger().add_attribute({"Q Value", Root->Child(a).Value.GetValue()}); 
        }
        XES::logger().end_list();
        XES::logger().end_event();
    }


            
    for (int a = 0; a < Simulator.GetNumActions(); a++){
        for (int o = 0; o < Simulator.GetNumObservations(); o++){
            if (Root->Child(a).Child(o) && (a != action || o != observation)){

                if (XES::enabled()){
                    XES::logger().start_event();
                    XES::logger().add_attribute({"even type", "depth +1" });
                    Simulator.log_beliefs(Root->Child(a).Child(o)->Beliefs(), a, o);
                    Simulator.log_action(a);
                    Simulator.log_observation(*Root->Beliefs().GetFirstSample(), o);
                    Simulator.log_reward(reward);

                    XES::logger().start_list("Valuable actions");
                    for (int ac = 0; ac < Simulator.GetNumActions(); ac++){
                        Simulator.log_action(ac);
                        XES::logger().add_attribute({"Q Value", Root->Child(a).Child(o)->Child(ac).Value.GetValue()}); 
                    }
                    XES::logger().end_list();
                    XES::logger().end_event();
                }


            }
        }
    }
    if (vnode) {
      if (Params.Verbose >= 1)
        cout << "Matched " << vnode->Beliefs().GetNumSamples() << " states"
             << endl;

      belief.Copy(vnode->Beliefs(), Simulator);
    }
    else {
        if (Params.Verbose >= 1)
            cout << "No matching node found" << endl;
    }

    // Generate transformed states to avoid particle deprivation
    if (Params.UseTransforms)
        AddTransforms(Root, belief);

    // If we still have no particles, fail
    if (belief.Empty() && (!vnode || vnode->Beliefs().Empty()))
        return false;

    //if (Params.Verbose >= 1)
    //    Simulator.DisplayBeliefs(belief, cout);

    // Find a state to initialise prior (only requires fully observed state)
    const STATE* state = 0;
    if (vnode && !vnode->Beliefs().Empty())
        state = vnode->Beliefs().GetFirstSample();
    else
        state = belief.GetFirstSample();

    // Delete old tree and create new root
    VNODE::Free(Root, Simulator);

    VNODE* newRoot = ExpandNode(state);
    newRoot->Beliefs().Move(belief,Simulator);

    Root = newRoot;
    //Root->Bags().printInsert(); //funzione che conta il numero di insert che vengono fatte nella bag

    if(PRINT_VALUES){
        cout << "bag dopo aver fatto un passo" << endl;
        Root->Beliefs().Display(std::cout, Simulator);
    }  

    return true;
}

int MCTS::SelectAction()
{
    if (Params.DisableTree)
        RolloutSearch();
    else
        UCTSearch();

    return GreedyUCB(Root, false);
}


void MCTS::UCTSearch()
{
    ClearStatistics();
    int historyDepth = History.Size();

    //SEARCH in rhopomcp
    for (int n = 0; n < Params.NumSimulations; n++)    
    {
        STATE* state = BeliefState().CreateSample(Simulator);

        Simulator.Validate(*state);

        Status.Phase = SIMULATOR::STATUS::TREE;
        if (Params.Verbose >= 2)
        {
            cout << "Starting simulation" << endl;
            Simulator.DisplayState(*state, cout);
        }

        TreeDepth = 0;
        PeakTreeDepth = 0;

        BAG bag = generateInitialBag(state, BeliefState()); //free gestita

        double totalReward = SimulateV_rho(*state, Root, bag);

        bag.Free(Simulator);

        StatTotalReward.Add(totalReward);
        StatTreeDepth.Add(PeakTreeDepth);

        if (Params.Verbose >= 2)
            cout << "Total reward = " << totalReward << endl;
        if (Params.Verbose >= 3)
            DisplayValue(4, cout);

        Simulator.FreeState(state);
        History.Truncate(historyDepth);

    }
    DisplayStatistics(cout);
}

double MCTS::SimulateV_rho(STATE& state, VNODE* vnode, BAG& beta)
{
    int action = GreedyUCB(vnode, true);

    PeakTreeDepth = TreeDepth;
    if (TreeDepth >= Params.MaxDepth) // search horizon reached
        return 0;

    if (TreeDepth >= 1) { // in RHO, insert in all levels after the root
        AddSample(vnode, state);
    }

    QNODE& qnode = vnode->Child(action);
    double totalReward = SimulateQ_rho(state, qnode, action, beta, vnode->Beliefs());
    vnode->Value.Add(totalReward);
    AddRave(vnode, totalReward);

    return totalReward;

}

double MCTS::SimulateQ_rho(STATE& state, QNODE& qnode, int action, BAG& beta, BAG& prev)
{
    int observation;
    double immediateReward, delayedReward = 0;

    //salvo lo stato precedente
    STATE* previous_state = Simulator.Copy(state);

    if (Simulator.HasAlpha())
        Simulator.UpdateAlpha(qnode, state);
    bool terminal = Simulator.Step(state, action, observation, immediateReward);
    //cout << "reward step normale: " << immediateReward << endl;

    assert(observation >= 0 && observation < Simulator.GetNumObservations());
    History.Add(action, observation);

    if (Params.Verbose >= 3)
    {
        Simulator.DisplayAction(action, cout);
        Simulator.DisplayObservation(state, observation, cout);
        Simulator.DisplayReward(immediateReward, cout);
        Simulator.DisplayState(state, cout);
    }

    VNODE*& vnode = qnode.Child(observation);

    if (!vnode && !terminal && qnode.Value.GetCount() >= Params.ExpandCount)
        vnode = ExpandNode(&state);

    BAG betaprime =
        CreateBag_beta(*previous_state, action, observation, beta, state);

    if (TreeDepth > 1)
        prev.AddSample(Simulator, beta);
    beta.Free(Simulator);

    Simulator.FreeState(previous_state);

    if (!terminal)
    {
        TreeDepth++;
        if (vnode)
            delayedReward = SimulateV_rho(state, vnode, betaprime);
        else
            delayedReward = Rho_Rollout(state, betaprime);
        TreeDepth--;
    }
    betaprime.Free(Simulator);

    immediateReward = Simulator.Rho_reward(prev, action);
    double totalReward = immediateReward + Simulator.GetDiscount() * delayedReward;
    qnode.Value.Add(totalReward);
    return totalReward;
}


BAG MCTS::CreateBag_beta(const STATE& previous, int action, int observation, const BAG &bag, const STATE &next){
    BAG generatedBag;

    double tmp =0;
    int temp_obs = 0;
    for (int i =0; i < Params.bagsize; i++){
        //simulate
        STATE* state = bag.CreateSample(Simulator); 
        STATE* previous_state = Simulator.Copy(*state);
        Simulator.Step(*state, action, temp_obs, tmp);
        double probability =
            Simulator.ProbObs(temp_obs, *previous_state, action, *state);

        if (probability > 0) {
          generatedBag.AddSample(Simulator, *state, probability);
        }

        Simulator.FreeState(state);
        Simulator.FreeState(previous_state);
    }

    double probability = Simulator.ProbObs(observation, previous, action, next);

    generatedBag.AddSample(Simulator, next, probability);

    if (generatedBag.Empty())
        cout << "no particles in the generated BAG" << endl;

    return generatedBag;
}

BAG MCTS::generateInitialBag(STATE* state, const BAG& initialBelief) {
    BAG bag;
    bag.AddSample(Simulator, *state);

    if (initialBelief.Empty())
        return bag;

    for (int i = 0; i < Params.bagsize; i++) {
        STATE* sampledS = initialBelief.CreateSample(Simulator);
        bag.AddSample(Simulator, *sampledS);
        Simulator.FreeState(sampledS);
    }  

    if(PRINT_DEBUG_TREE){
        cout << "\n----" << endl;
        bag.Display(cout,Simulator);
        cout << "----" << endl;
    }
    return bag;
}

double MCTS::Rho_Rollout(STATE& state, BAG &beta)
{
    Status.Phase = SIMULATOR::STATUS::ROLLOUT;
    if (Params.Verbose >= 3)
        cout << "Starting rollout" << endl;

    double totalReward = 0.0;
    double discount = 1.0;
    bool terminal = false;
    int numSteps;

    for (numSteps = 0; numSteps + TreeDepth < Params.MaxDepth && !terminal; ++numSteps)
    {
        int observation;
        double reward;

        STATE* previous_state = Simulator.Copy(state);

        int action = Simulator.SelectRandom(state, History, Status);
        terminal = Simulator.Step(state, action, observation, reward);
        History.Add(action, observation);

        BAG betaprime =
            CreateBag_beta(*previous_state, action, observation, beta, state);

        Simulator.FreeState(previous_state);

        if (Params.Verbose >= 4)
        {
            Simulator.DisplayAction(action, cout);
            Simulator.DisplayObservation(state, observation, cout);
            Simulator.DisplayReward(reward, cout);
            Simulator.DisplayState(state, cout);
        }

        reward = Simulator.Rho_reward(beta, action);

        totalReward += reward * discount;
        discount *= Simulator.GetDiscount();

        beta.Move(betaprime, Simulator);
    }

    StatRolloutDepth.Add(numSteps);
    if (Params.Verbose >= 3)
        cout << "Ending rollout after " << numSteps
            << " steps, with total reward " << totalReward << endl;

    return totalReward;
}


//-----------------------------------------------------------------------------



void MCTS::DisplayValue(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "MCTS Values:" << endl;
    Root->DisplayValue(history, depth, ostr);
}

void MCTS::DisplayPolicy(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "MCTS Policy:" << endl;
    Root->DisplayPolicy(history, depth, ostr);
}

void MCTS::ClearStatistics()
{
    StatTreeDepth.Clear();
    StatRolloutDepth.Clear();
    StatTotalReward.Clear();
}

void MCTS::DisplayStatistics(ostream& ostr) const
{
    if (Params.Verbose >= 1)
    {
        StatTreeDepth.Print("Tree depth", ostr);
        StatRolloutDepth.Print("Rollout depth", ostr);
        StatTotalReward.Print("Total reward", ostr);
    }

    if (Params.Verbose >= 2)
    {
        ostr << "Policy after " << Params.NumSimulations << " simulations" << endl;
        DisplayPolicy(6, ostr);
        ostr << "Values after " << Params.NumSimulations << " simulations" << endl;
        DisplayValue(6, ostr);
    }
}

void MCTS::AddTransforms(VNODE* root, BAG& beliefs)
{
    int attempts = 0, added = 0;

    // Local transformations of state that are consistent with history
    while (added < Params.NumTransforms && attempts < Params.MaxAttempts)
    {
        STATE* transform = CreateTransform();
        if (transform)
        {
            beliefs.AddSample(Simulator, *transform);
            added++;
        }
        attempts++;
    }

    if (Params.Verbose >= 1)
    {
        cout << "Created " << added << " local transformations out of "
            << attempts << " attempts" << endl;
    }
}

STATE* MCTS::CreateTransform() const
{
    int stepObs;
    double stepReward;

    STATE* state = Root->Beliefs().CreateSample(Simulator);
    Simulator.Step(*state, History.Back().Action, stepObs, stepReward);
    if (Simulator.LocalMove(*state, History, stepObs, Status))
        return state;
    Simulator.FreeState(state);
    return 0;
}

double MCTS::UCB[UCB_N][UCB_n];
bool MCTS::InitialisedFastUCB = true;

void MCTS::InitFastUCB(double exploration)
{
    cout << "Initialising fast UCB table... ";
    for (int N = 0; N < UCB_N; ++N)
        for (int n = 0; n < UCB_n; ++n)
            if (n == 0)
                UCB[N][n] = Infinity;
            else
                UCB[N][n] = exploration * sqrt(log(N + 1) / n);
    cout << "done" << endl;
    InitialisedFastUCB = true;
}

inline double MCTS::FastUCB(int N, int n, double logN) const
{
    if (InitialisedFastUCB && N < UCB_N && n < UCB_n)
        return UCB[N][n];

    if (n == 0)
        return Infinity;
    else
        return Params.ExplorationConstant * sqrt(logN / n);
}

void MCTS::AddRave(VNODE* vnode, double totalReward)
{
    double totalDiscount = 1.0;
    for (int t = TreeDepth; t < History.Size(); ++t)
    {
        QNODE& qnode = vnode->Child(History[t].Action);
        qnode.AMAF.Add(totalReward, totalDiscount);
        totalDiscount *= Params.RaveDiscount;
    }
}

VNODE* MCTS::ExpandNode(const STATE* state)
{
    VNODE* vnode = VNODE::Create();
    vnode->Value.Set(0, 0);
    Simulator.Prior(state, History, vnode, Status);

    if (Params.Verbose >= 2)
    {
        cout << "Expanding node: ";
        History.Display(cout);
        cout << endl;
    }

    return vnode;
}

void MCTS::AddSample(VNODE* node, const STATE& state)
{
    node->Beliefs().AddSample(Simulator, state);
    if (Params.Verbose >= 2)\
    {
        cout << "Adding sample:" << endl;
        Simulator.DisplayState(state, cout);
    }
}

int MCTS::GreedyUCB(VNODE* vnode, bool ucb) const
{
    static vector<int> besta;
    besta.clear();
    double bestq = -Infinity;
    int N = vnode->Value.GetCount();
    double logN = log(N + 1);
    bool hasalpha = Simulator.HasAlpha();

    for (int action = 0; action < Simulator.GetNumActions(); action++)
    { 
        double q, alphaq;
        int n, alphan;

        QNODE& qnode = vnode->Child(action);
        q = qnode.Value.GetValue();
        n = qnode.Value.GetCount();

        //cout << "UCB " << action << " " <<  n << " " <<  q << " bestq:" << bestq <<endl;

        if (Params.UseRave && qnode.AMAF.GetCount() > 0)
        {
            double n2 = qnode.AMAF.GetCount();
            double beta = n2 / (n + n2 + Params.RaveConstant * n * n2);
            q = (1.0 - beta) * q + beta * qnode.AMAF.GetValue();
        }

        if (hasalpha && n > 0)
        {
            Simulator.AlphaValue(qnode, alphaq, alphan);
            q = (n * q + alphan * alphaq) / (n + alphan);
            //cout << "N = " << n << ", alphaN = " << alphan << endl;
            //cout << "Q = " << q << ", alphaQ = " << alphaq << endl;
        }

        if (ucb)
            q += FastUCB(N, n, logN);

        if (q >= bestq)
        {
            if (q > bestq)
                besta.clear();
            bestq = q;
            besta.push_back(action);
        }

    }

    assert(!besta.empty());
    return besta[Random(besta.size())];
}



void MCTS::RolloutSearch()
{
    cout << "sei dentro rollout " << endl;
    std::vector<double> totals(Simulator.GetNumActions(), 0.0);
    int historyDepth = History.Size();
    std::vector<int> legal;
    assert(BeliefState().GetNumSamples() > 0);

    
    const STATE& s = safe_cast<const STATE&>(*BeliefState().GetFirstSample());

    Simulator.GenerateLegal(s, GetHistory(), legal, GetStatus());
    random_shuffle(legal.begin(), legal.end());

    for (int i = 0; i < Params.NumSimulations; i++){
        int action = legal[i % legal.size()];
        STATE* state = Root->Beliefs().CreateSample(Simulator);

        Simulator.Validate(*state);

        BAG bag = generateInitialBag(state, BeliefState());

        int observation;
        double immediateReward, delayedReward, totalReward;
        bool terminal = Simulator.Step(*state, action, observation, immediateReward);

        VNODE*& vnode = Root->Child(action).Child(observation);
        if (!vnode && !terminal)
        {
            vnode = ExpandNode(state);
            AddSample(vnode, *state);
        }   
        History.Add(action, observation);
        //delayedReward = Rollout(*state);
        delayedReward = Rho_Rollout(*state, bag);
        totalReward = immediateReward + Simulator.GetDiscount() * delayedReward;
        Root->Child(action).Value.Add(totalReward);

        Simulator.FreeState(state);
        History.Truncate(historyDepth);
    }
}
