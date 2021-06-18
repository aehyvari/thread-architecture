#pragma once

#include "Channel.h"

class SMTSolver {
public:
    enum class Result { SAT, UNSAT, UNKNOWN };
private:
    int count;
    Channel<char> channel;
    std::vector<char> learned_clauses;
    bool learnSomeClauses();
    Result doThings();
    bool shouldStop();
    Result result;
public:
    SMTSolver() : learned_clauses(1000), count(0), result(Result::UNKNOWN) {}
    void search();
    Result getResult() const { return result; }
    Channel<char> & getChannel() { return channel; }
};
