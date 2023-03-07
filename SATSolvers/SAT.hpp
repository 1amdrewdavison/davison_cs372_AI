//Written by Andrew Davison for Project A3 for CSC 372: Artificial Intelligence with Dr. Thomas Allen
//Contains code to read DIMACS and the structures used to record info from DIMACS
//Implementations in SAT.cpp

#include <vector>
#include <string>
#include <iostream>
#include <chrono>

struct Solution {
    std::vector<bool> literals;
    std::chrono::nanoseconds CPUTime;
    short int c;
};

struct Clause {
    std::vector<int> literals;
    bool isActive = true;
};

struct Conjunction {
    std::vector<Clause> clauses;
    std::vector<bool> literals;
};

Conjunction readDIMACS(std::ifstream& filename);

Solution simpleHillClimbing(Conjunction);

Solution constraintSolvingWithForwardSearch(Conjunction);

Solution DPLL(Conjunction);

