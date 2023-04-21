//Written by Andrew Davison for project A5 inCSC 372: Survey of Artificial Intelligence
//This program encodes a 5 node Bayesian network. The program runs two different types of inference.
//There is an exact inference method by computing the full joint distribution table.
//There is an approximate inference method by using rejection sampling.

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>

//Defines how many times rejection sampling is run.
#define NUM_ITERATIONS 10000000

using distribution = std::vector<std::vector<std::vector<std::vector<std::vector<double>>>>>;
using assignment = std::vector<int>;

//These functions all act as lookup CPTs.
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

//This function generates the full joint distribution.
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

//This function calculates the probability of a given assignment according to the full joint distribution
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
                        //Check to see if this is a corresponding row to the input assignment
                        if ((arguments[0] == 4 || arguments[0] % 2 == b) && (arguments[1] == 4 || arguments[1] % 2 == e) && (arguments[2] == 4 || arguments[2] % 2 == a) &&
                            (arguments[3] == 4 || arguments[3] % 2 == j) && (arguments[4] == 4 || arguments[4] % 2 == m)) {
                            prob += joint[b][e][a][j][m];
                        }
                        //Check to see if this row also fulfills the "given" clause
                        if ((arguments[0] / 2 != 1 || arguments[0] - 2 == b) && (arguments[1] / 2 != 1 || arguments[1] - 2 == e) && (arguments[2] / 2 != 1 || arguments[2] - 2 == a) &&
                            (arguments[3] / 2 != 1 || arguments[3] - 2 == j) && (arguments[4] / 2 != 1 || arguments[4] - 2 == m)) {
                            given += joint[b][e][a][j][m];
                        }
                    }
                } 
            }
        }
    }

    //If this is conditional, use the definition of conditional probability to find the answer.
    if (given > 0) {
        return prob / given;
    } else {
        return prob;
    }
}

//Generates a random event
assignment generateSample() {
    assignment sample;

    sample.push_back(((double)rand() / RAND_MAX) < burglary(true));
    sample.push_back(((double)rand() / RAND_MAX) < earthquake(true));
    sample.push_back(((double)rand() / RAND_MAX) < alarm(true, sample[0], sample[1]));
    sample.push_back(((double)rand() / RAND_MAX) < johnCalls(true, sample[2]));
    sample.push_back(((double)rand() / RAND_MAX) < maryCalls(true, sample[2]));

    return sample;
}

//Runs rejection sampling
double rejection(assignment arguments) {
    int acceptedTrials = 0;
    int givenTrials = 0;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        //Generate a random event
        assignment newSample = generateSample();

        //Check to see if this is a valid corresponding event
        if ((arguments[0] == 4 || arguments[0] % 2 == newSample[0]) && (arguments[1] == 4 || arguments[1] % 2 == newSample[1]) && (arguments[2] == 4 || arguments[2] % 2 == newSample[2]) &&
            (arguments[3] == 4 || arguments[3] % 2 == newSample[3]) && (arguments[4] == 4 || arguments[4] % 2 == newSample[4])) {
            acceptedTrials++;
        }
        //Check to see if this event fulfills "given" conditions
        if ((arguments[0] / 2 != 1 || arguments[0] - 2 == newSample[0]) && (arguments[1] / 2 != 1 || arguments[1] - 2 == newSample[1]) && (arguments[2] / 2 != 1 || arguments[2] - 2 == newSample[2]) &&
            (arguments[3] / 2 != 1 || arguments[3] - 2 == newSample[3]) && (arguments[4] / 2 != 1 || arguments[4] - 2 == newSample[4])) {
            givenTrials++;
        }
    }

    //If this is conditional, use the definition of conditional probability to find the answer.
    if (givenTrials > 0) {
        return (double)acceptedTrials / givenTrials;
    } else {
        return (double)acceptedTrials / NUM_ITERATIONS;
    }
}

int main(int argc, char* argv[]) {
    //Initialize an assignment to all undetermined
    assignment arguments(5, 4);
    bool validInput = true;
    int given = 0;

    //Take Input:
    //In the assignment, a 0 is false, 1 is true, 2 is false in the given clause, 3 is true in the given clause, and 4 is undefined.
    for (int i = 1; i < argc && validInput; i++) {
        std::string argument = argv[i];
        
        if (argument == "given") {
            //Cannot appear twice or as the last element
            !given && i != argc - 1 ? given = 2 : validInput = false;
        } else {
            //Determine the corresponding node
            char variable = argument.at(0);

            switch (variable) {
                case 'B':
                    //Check to see that this node has not appeared already
                    if (arguments[0] != 4) {
                        validInput = false;
                        break;
                    }

                    //Assign a value according to its truth value and whether it is in the "given" clause
                    argument.at(1) == 't' ? arguments[0] = 1 + given: arguments[0] = 0 + given;
                    break;
                case 'E':
                    //Do the same
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

    //If the input was invalid, inform the user and terminate.
    if (!validInput) {
        std::cout << "Invalid input format.";
        return -1;
    }

    //Find the probabilities
    double exact = fullJoint(arguments);
    double approx = rejection(arguments);

    //Display results to user with a precision of 10^-6
    std::cout.precision(6);
    std::cout << "Exact inference by enumeration found probability " << exact << "\n";
    std::cout << "Approximate inference by rejection sampling found probability " << approx << "\n";
}