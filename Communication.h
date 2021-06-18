#pragma once
#include "Channel.h"
#include "SMTSolver.h"
#include <thread>

class Communication {
    Channel<char>& channel;
    SMTSolver & solver;
    static std::string resultToString(SMTSolver::Result res);
    void synchronizeClauses() {}
public:
    Communication(SMTSolver & solver) : channel(solver.getChannel()), solver(solver) {}
    void communicate(std::chrono::duration<long long> minSleep);
};
