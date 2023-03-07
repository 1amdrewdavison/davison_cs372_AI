//Written by Andrew Davison for Project A3 for CSC 372: Artificial Intelligence with Dr. Thomas Allen
//Contains code to read DIMACS and the structures used to recored info from DIMACS
//Implements general-use defined in SAT.hpp and contains the main() function

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>

#include "SAT.hpp"

Conjunction readDIMACS(std::string filename) {
    std::ifstream file;
    file.open(filename);

    Conjunction cnf;
    int variables, clauses;

    std::string line, info;
    char first;

    while(std::getline(file, line)) {
        
        std::istringstream streamLine(line);
        streamLine >> first;

        switch (first) {
            case 'c':
                //This line is a comment
                break;
            case 'p':
                //This line contains number of variables and clauses
                //Skip the p cnf part
                streamLine >> info;

                streamLine >> info;
                variables = std::stoi(info);
                streamLine >> info;
                clauses = std::stoi(info);
                break;
            default:
                info = first;
                Clause newClause;

                while (info != "0") {
                    if (info == "-") {
                        std::string number;
                        streamLine >> number;
                        info += number;
                    }
                    newClause.literals.push_back(std::stoi(info));
                    streamLine >> info;
                }

                cnf.clauses.push_back(newClause);
                break;
        }
    }

    file.close();

    return cnf;
}

int main() {
    auto cnf = readDIMACS("10.40.160707067.cnf");
}