#include <iostream>
#include <string>
#include "SMTSolver.h"
#include "Communication.h"

int main(int argc, char** argv) {

    if (argc == 1) {
        std::cout << "Usage: " << argv[0] << " <seed>" << std::endl;
        return 1;
    }

    std::srand(atoi(argv[1]));

    SMTSolver solver;
    Communication(solver).communicate(std::chrono::seconds(4));

    return 0;
}

