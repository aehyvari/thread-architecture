#include <iostream>
#include <string>
#include "SMTSolver.h"

bool SMTSolver::shouldStop() {
    std::scoped_lock<std::mutex> lk(channel.getMutex());
    return channel.shouldStop();
}


bool SMTSolver::learnSomeClauses() {
    for (auto & c : learned_clauses) {
        c = 0;
    }

    bool allUnit = false;
    while (not allUnit) {
        int idx = std::rand() % learned_clauses.size();
        learned_clauses[idx] = 1;
        allUnit = true;
        for (auto i : learned_clauses) {
            if (i != 1) {
                allUnit = false;
            }
        }
    }
    // With probability we find a solution
    if (std::rand() < RAND_MAX/100) {
        return false;
    }
    return true;
}

SMTSolver::Result SMTSolver::doThings() {
    if (not learnSomeClauses()) {
        return std::rand() < RAND_MAX / 2 ? Result::SAT : Result::UNSAT;
    }

    {
        std::scoped_lock<std::mutex> lk(channel.getMutex());
        if (channel.empty()) {
            std::cout << "[t2] add clauses" << std::endl;
            for (auto c : learned_clauses) {
                channel.push(c);
            }
            channel.notify_one();
        } else {
            std::cout << "[t2] buffer nonempty" << std::endl;
        }
    }

    return Result::UNKNOWN;
}

void SMTSolver::search() {
    result = Result::UNKNOWN;
    while ( result == Result::UNKNOWN and not shouldStop() )
        result = doThings();

    // Either a result or request to stop
    {
        std::scoped_lock<std::mutex> lk(channel.getMutex());
        channel.setShallStop();
        channel.notify_one();
    }
}

