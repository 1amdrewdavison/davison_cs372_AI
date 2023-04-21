//Written by Andrew Davison for CSC 372: Survey of Artificial Intelligence
//A5: Part 2

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>

#define NUM_ITERATIONS 10000000

using distribution = std::vector<std::vector<std::vector<std::vector<std::vector<double>>>>>;
using assignment = std::vector<int>;

double burglary(bool b) {
    return b ? 0.02 : 0.98;
}

double earthquake(bool e) {
    return e ? 0.03 : 0.97;
}

double alarm(bool a, bool b, bool e) {
    double posterior;
    
    if (b && e) {
        posterior = 0.97;
    } else if (b && !e) {
        posterior = 0.92;
    } else if (!b && e) {
        posterior = 0.36;
    } else {
        posterior = 0.03;
    }

    return a ? posterior : 1 - posterior;
}

double johnCalls(bool j, bool a) {
    if (j && a) {
        return 0.85;
    } else if (j && !a) {
        return 0.07;
    } else if (!j && a) {
        return 0.15;
    }
        
    return 0.93;
}

double maryCalls(bool m, bool a) {
    if (m && a) {
        return 0.69;
    } else if (m && !a) {
        return 0.02;
    } else if (!m && a) {
        return 0.31;
    }
    
    return 0.98;
}

distribution generateDistribution() {
    distribution joint;
    
    for (int b = 0; b < 2; b++) {
        joint.push_back({});
        for (int e = 0; e < 2; e++) {
            joint[b].push_back({});
            for (int a = 0; a < 2; a++) {
                joint[b][e].push_back({});
                for (int j = 0; j < 2; j++) {
                    joint[b][e][a].push_back({});
                    for (int m = 0; m < 2; m++) {
                        joint[b][e][a][j].push_back(burglary(b) * earthquake(e) * alarm(a, b, e) * johnCalls(j, a) * maryCalls(m, a));
                    }
                } 
            }
        }
    }

    return joint;
}

double fullJoint(assignment arguments) {
    double prob = 0.0;
    double given = 0.0;
    
    distribution joint = generateDistribution();

    double safety_check = 0;

    for (int b = 0; b < 2; b++) {
        for (int e = 0; e < 2; e++) {
            for (int a = 0; a < 2; a++) {
                for (int j = 0; j < 2; j++) {
                    for (int m = 0; m < 2; m++) {
                        if ((arguments[0] == 4 || arguments[0] % 2 == b) && (arguments[1] == 4 || arguments[1] % 2 == e) && (arguments[2] == 4 || arguments[2] % 2 == a) &&
                            (arguments[3] == 4 || arguments[3] % 2 == j) && (arguments[4] == 4 || arguments[4] % 2 == m)) {
                            prob += joint[b][e][a][j][m];
                        }
                        if ((arguments[0] / 2 != 1 || arguments[0] - 2 == b) && (arguments[1] / 2 != 1 || arguments[1] - 2 == e) && (arguments[2] / 2 != 1 || arguments[2] - 2 == a) &&
                            (arguments[3] / 2 != 1 || arguments[3] - 2 == j) && (arguments[4] / 2 != 1 || arguments[4] - 2 == m)) {
                            given += joint[b][e][a][j][m];
                        }
                    }
                } 
            }
        }
    }

    if (given > 0) {
        return prob / given;
    } else {
        return prob;
    }
}

assignment generateSample() {
    assignment sample;

    sample.push_back(((double)rand() / RAND_MAX) < burglary(true));
    sample.push_back(((double)rand() / RAND_MAX) < earthquake(true));
    sample.push_back(((double)rand() / RAND_MAX) < alarm(true, sample[0], sample[1]));
    sample.push_back(((double)rand() / RAND_MAX) < johnCalls(true, sample[2]));
    sample.push_back(((double)rand() / RAND_MAX) < maryCalls(true, sample[2]));

    return sample;
}

double rejection(assignment arguments) {
    int acceptedTrials = 0;
    int givenTrials = 0;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        assignment newSample = generateSample();

        if ((arguments[0] == 4 || arguments[0] % 2 == newSample[0]) && (arguments[1] == 4 || arguments[1] % 2 == newSample[1]) && (arguments[2] == 4 || arguments[2] % 2 == newSample[2]) &&
            (arguments[3] == 4 || arguments[3] % 2 == newSample[3]) && (arguments[4] == 4 || arguments[4] % 2 == newSample[4])) {
            acceptedTrials++;
        }
        if ((arguments[0] / 2 != 1 || arguments[0] - 2 == newSample[0]) && (arguments[1] / 2 != 1 || arguments[1] - 2 == newSample[1]) && (arguments[2] / 2 != 1 || arguments[2] - 2 == newSample[2]) &&
            (arguments[3] / 2 != 1 || arguments[3] - 2 == newSample[3]) && (arguments[4] / 2 != 1 || arguments[4] - 2 == newSample[4])) {
            givenTrials++;
        }
    }

    if (givenTrials > 0) {
        return (double)acceptedTrials / givenTrials;
    } else {
        return (double)acceptedTrials / NUM_ITERATIONS;
    }
}

//0 is false, 1 is true, 2 is given false, 3 is given true
int main(int argc, char* argv[]) {
    assignment arguments(5, 4);
    bool validInput = true;
    int given = 0;

    for (int i = 1; i < argc && validInput; i++) {
        std::string argument = argv[i];
        
        if (argument == "given") {
            //Cannot appear twice or as the last element
            !given && i != argc - 1 ? given = 2 : validInput = false;
        } else {
            char variable = argument.at(0);

            switch (variable) {
                case 'B':
                    if (arguments[0] != 4) {
                        validInput = false;
                        break;
                    }

                    argument.at(1) == 't' ? arguments[0] = 1 + given: arguments[0] = 0 + given;
                    break;
                case 'E':
                    if (arguments[1] != 4) {
                        validInput = false;
                        break;
                    }
                    
                    argument.at(1) == 't' ? arguments[1] = 1 + given: arguments[1] = 0 + given;
                    break;
                case 'A':
                    if (arguments[2] != 4) {
                        validInput = false;
                        break;
                    }
                    
                    argument.at(1) == 't' ? arguments[2] = 1 + given: arguments[2] = 0 + given;
                    break;
                case 'J':
                    if (arguments[3] != 4) {
                        validInput = false;
                        break;
                    }
                    
                    argument.at(1) == 't' ? arguments[3] = 1 + given: arguments[3] = 0 + given;
                    break;
                case 'M':
                    if (arguments[4] != 4) {
                        validInput = false;
                        break;
                    }
                    
                    argument.at(1) == 't' ? arguments[4] = 1 + given: arguments[4] = 0 + given;
                    break;
                default:
                    validInput = false;
                    break;
            }
        }
    }

    if (!validInput) {
        std::cout << "Invalid input format.";
        return -1;
    }

    double exact = fullJoint(arguments);
    double approx = rejection(arguments);

    std::cout.precision(6);
    std::cout << "Exact inference by enumeration found probability " << exact << "\n";
    std::cout << "Approximate inference by rejection sampling found probability " << approx << "\n";
}