//Written by Andrew Davison for CSC 372: Survey of Artificial Intelligence
//Assignment A6: Markov Decision Processes
//Implements both Value Iteration and Policy Iteration to discover the optimal policy.

#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>

#define epsilon 0.0001
#define gamma 0.95

//MDP Setup

//States are represented by a corresponding integer
const std::vector<int> states = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

//Actions are also defined by integers
enum action {Up, Down, Left, Right};
const std::vector<std::string> action_names = {"Up", "Down", "Left", "Right"};

//Reward function is a length 16 array
const std::vector<int> rewards = {0, 50, 0, 50, 0, 50, 0, 50, 0, 50, 100, 50, 200, 50, 0, 50};

//Transition function is a 3-dimensional array: s, s', a
std::vector<std::vector<std::vector<float>>> transitions;

//Initialize transition function
void initializeTransition() {
    for (int i = 0; i < 16; i++) {
        transitions.push_back({});
        for (int j = 0; j < 16; j++) {
            transitions[i].push_back({});
            for (int k = 0; k < 4; k++) {
                //Only look at possible end states
                
                if (i == j) {
                    //Same state
                    transitions[i][j].push_back(0.1);
                } else if (i + 1 == j && (i + 1) % 4 != 0) {
                    //State to the right

                    if (k == Right) {
                        if (i % 4 == 0) {
                            transitions[i][j].push_back(0.9);
                        } else {
                            transitions[i][j].push_back(0.7);
                        }
                    } else if (k == Left) {
                        if (i % 4 == 0) {
                            transitions[i][j].push_back(0.9);
                        } else {
                            transitions[i][j].push_back(0.2);
                        }
                    } else {
                        transitions[i][j].push_back(0);
                    }
                } else if (i - 1 == j && (i - 1) % 4 != 3 && i != 0) {
                    //State to the left

                    if (k == Left) {
                        if (i % 4 == 3) {
                            transitions[i][j].push_back(0.9);
                        } else {
                            transitions[i][j].push_back(0.7);
                        }
                    } else if (k == Right) {
                        if (i % 4 == 3) {
                            transitions[i][j].push_back(0.9);
                        } else {
                            transitions[i][j].push_back(0.2);
                        }
                    } else {
                        transitions[i][j].push_back(0);
                    }
                } else if (i + 4 == j && (i / 4) != 3) {
                    //State directly up

                    if (k == Up) {
                        if (i / 4 == 0) {
                            transitions[i][j].push_back(0.9);
                        } else {
                            transitions[i][j].push_back(0.7);
                        }
                    } else if (k == Down) {
                        if (i / 4 == 0) {
                            transitions[i][j].push_back(0.9);
                        } else {
                            transitions[i][j].push_back(0.2);
                        }
                    } else {
                        transitions[i][j].push_back(0);
                    }
                } else if (i - 4 == j && i - 4 >= 0) {
                    //State directly down

                    if (k == Down) {
                        if (i / 4 == 3) {
                            transitions[i][j].push_back(0.9);
                        } else {
                            transitions[i][j].push_back(0.7);
                        }
                    } else if (k == Up) {
                        if (i / 4 == 3) {
                            transitions[i][j].push_back(0.9);
                        } else {
                            transitions[i][j].push_back(0.2);
                        }
                    } else {
                        transitions[i][j].push_back(0);
                    }
                } else {
                    transitions[i][j].push_back(0);
                }
            }
        }
    }
}

//Create a struct for returning solutions
struct solution {
    std::vector<double> utility;
    std::vector<int> policy;
};

//Run value iteration
solution valueIteration() {
    std::vector<double> u_prime, u;
    std::vector<int> pi_star;
    double delta;

    //Initialize U'
    for (int i = 0; i < 16; i++) {
        u_prime.push_back(0);
    }

    //Begin utility iteration
    do {
        u = u_prime;
        delta = 0;

        //For each state
        for (int s = 0; s < 16; s++) {
            std::vector<float> sum_over_actions;

            //Sum over the next states for each action
            for (int a = 0; a < 4; a++) {
                sum_over_actions.push_back(0);
                
                for (int s_prime = 0; s_prime < 16; s_prime++) {
                    sum_over_actions[a] += transitions[s][s_prime][a] * u[s_prime];
                }
            }

            //Select max and add to utility
            u_prime[s] = rewards[s] + gamma * (*std::max_element(sum_over_actions.begin(), sum_over_actions.end()));
            delta = std::max(delta, std::abs(u_prime[s] - u[s]));
        }
    } 
    while (delta > epsilon * (1 - gamma) / gamma);

    //Now find pi*
    for (int s = 0; s < 16; s++) {
        std::vector<float> action_values;
        
        for (int a = 0; a < 4; a++) {
            action_values.push_back(0);
            
            for (int s_prime = 0; s_prime < 16; s_prime++) {
                action_values[a] += transitions[s][s_prime][a] * u[s_prime];
            }
        }

        pi_star.push_back(std::distance(action_values.begin(), std::max_element(action_values.begin(), action_values.end())));
    }

    return solution{u, pi_star};
}

solution policyIteration(int numIterations = 100) {
    std::vector<double> u;
    std::vector<int> pi;
    bool unchanged = false;

    for (int s = 0; s < 16; s++) {
        u.push_back(0);
        pi.push_back(rand() % 4);
    }

    while (!unchanged) {
        //Iterate for utilities
        for (int i = 0; i < numIterations; i++) {
            for (int s = 0; s < 16; s++) {
                double total = 0;

                for (int s_prime = 0; s_prime < 16; s_prime++) {
                    auto action = pi[s];
                    total += transitions[s][s_prime][action] * u[s_prime];
                }

                u[s] = rewards[s] + gamma * total;
            }
        }

        unchanged = true;
        //Determine policy
        for (int s = 0; s < 16; s++) {
            std::vector<float> sum_over_actions;

            for (int a = 0; a < 4; a++) {
                sum_over_actions.push_back(0);
                
                for (int s_prime = 0; s_prime < 16; s_prime++) {
                    sum_over_actions[a] += transitions[s][s_prime][a] * u[s_prime];
                }
            }

            auto maxUtilityPointer = std::max_element(sum_over_actions.begin(), sum_over_actions.end());
            int bestAction = std::distance(sum_over_actions.begin(), maxUtilityPointer);

            //Select best policy
            if (*maxUtilityPointer > sum_over_actions[pi[s]]) {
                pi[s] = bestAction;
                unchanged = false;
            }
        }
    }

    return solution{u, pi};
}

//Pretty prints solution
void printSolution(solution sol) {
    std::cout << "Utility function:\n";
    for (int i = 3; i > -1; i--) {
        for (int j = 0; j < 4; j++) {
             std::cout << "state " << i*4 + j << ":\t" << sol.utility[i*4 + j] << "\t\t";
        }
        std::cout << "\n";
    }
    
    std::cout << "\n\nOptimal Policy:\n";
    for (int i = 3; i > -1; i--) {
        for (int j = 0; j < 4; j++) {
            std::cout << "state " << i*4 + j << ":\t" << action_names[sol.policy[i*4 + j]] << "\t\t";
        }
        std::cout <<  "\n";
    }
}

int main() {
    initializeTransition();

    auto value_sol = valueIteration();

    std::cout << "Value Iteration Solution:\n";
    printSolution(value_sol);

    auto policy_sol = policyIteration(10000);

    std::cout << "\nPolicy Iteration Solution:\n";
    printSolution(policy_sol);
}