#include <iostream>
#include <string>
#include "Communication.h"
#include "SMTSolver.h"

std::string Communication::resultToString(SMTSolver::Result res) {
    if (res == SMTSolver::Result::UNKNOWN)
        return "unknown";
    else if (res == SMTSolver::Result::SAT)
        return "sat";
    else if (res == SMTSolver::Result::UNSAT)
        return "unsat";
    return "undefined"; // Unreachable
}

void
Communication::communicate(std::chrono::duration<long long> minSleep) {
    bool solved = false;

    std::chrono::time_point wakeupAt(std::chrono::system_clock::now() + minSleep);

    while (not solved) {
        std::thread t([&]() { solver.search(); });

        int count = 0;

        while (true) {
            std::unique_lock<std::mutex> lk(channel.getMutex());

            // Wait for a notification from the solver or the timeout.
            // After waitUntil we own the lock
            if (!channel.waitUntil(lk, wakeupAt)) {
                // timeout
                std::cout << "Timeout" << std::endl;
                synchronizeClauses();
                channel.setShouldStop();
                wakeupAt = std::chrono::system_clock::now() + minSleep;
                break;
            }

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

}
