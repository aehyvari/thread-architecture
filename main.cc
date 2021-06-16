#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "Channel.h"
#include "SMTSolver.h"

SMTSolver makeSMTSolver(Channel<char> & channel) {
    return SMTSolver(channel);
}

void runSMTSolver(SMTSolver & solver) {
    solver.search();
}

std::string resultToString(SMTSolver::Result res) {
    if (res == SMTSolver::Result::UNKNOWN)
        return "unknown";
    else if (res == SMTSolver::Result::SAT)
        return "sat";
    else if (res == SMTSolver::Result::UNSAT)
        return "unsat";
    return "undefined"; // Unreachable
}

int main(int argc, char** argv) {

    if (argc == 1) {
        std::cout << "Usage: " << argv[0] << " <seed>" << std::endl;
        return 1;
    }

    std::srand(atoi(argv[1]));

    Channel<char> channel;
    auto solver = makeSMTSolver(channel);

    bool solved = false;

    while (not solved) {
        std::thread t(runSMTSolver, std::ref(solver));

        int count = 0;

        while (true) {
            std::unique_lock<std::mutex> lk(channel.getMutex());
            if (count == 25) {
                channel.setShouldStop();
                break;
            }

            // Wait for a reaction from the solver
            channel.wait(lk);

            // We own the lock
            if (not channel.empty()) {
                std::cout << "[t1] copying shared clauses " << count++ << std::endl;
                std::for_each(channel.cbegin(), channel.cend(), [&](char c) { assert(c == 1); });
                channel.clear();
            } else if (channel.shallStop()) {
                std::cout << "[t1] solver is stopping" << std::endl;
                break;
            } else {
                std::cout << "[t1] spurious wakeup?" << std::endl;
            }
        }
        t.join();
        // Single thread
        if (solver.getResult() != SMTSolver::Result::UNKNOWN) {
            solved = true;
        } else {
            channel.clearShouldStop();
            channel.clearShallStop();
        }
    }

    std::cout << "Instance is " << resultToString(solver.getResult()) << std::endl;

    return 0;
}

