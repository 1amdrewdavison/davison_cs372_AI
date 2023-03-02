//Written by Andrew Davison for Project A2 for CSC 372: Artificial Intelligence with Dr. Thomas Allen
//Models a 2x2 Rubiks cube and generates random cubes to be solved using IDA*
//This function collects time complexity data and nodes expanded during the IDA* algorithm
//This model is heavily dervied from the work of a blog post at https://k-l-lambda.github.io/2020/12/14/rubik-cube-notation/ that models a 3x3 cube.
//This model has been adapted by me into a 2x2 cube model.

#include <utility>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <random>
#include <chrono>
#include <fstream>

/*This is a representation of the orientation of each cubelet that exists in the position of the index. This uniquely identifies the cubelet and 
uniquely identifies a state, although it is subject to prespective and does represent redundant states. Currentlry, this model is actually entirely ignorant of
the sticker colors of the cube, and instead is entirely reliant on cubelet orientations.*/
//Design derived from https://k-l-lambda.github.io/2020/12/14/rubik-cube-notation/
using cube = std::vector<int>;

//Enum for enumerating possible cube orientations and a mapping vector for printing out orientations
enum {alpha, beta, gamma, delta, epsilon, zeta, eta, theta, iota, kappa, lambda, mu, nu, xi, omicron, pi, rho, sigma, tau, upsilon, phi, chi, psi, omega};
const std::vector<std::string> orientationLiterals = {"alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta", "iota", "kappa", "lambda", "mu", "nu", 
                                                "xi", "omicron", "pi", "rho", "sigma", "tau", "upsilon", "phi", "chi", "psi", "omega"};


//Enum for actions
const std::vector<std::string> actionLiterals = {"U", "U'", "D", "D'", "F", "F'", "B", "B'", "L", "L'", "R", "R'"};


//This is a lookup table for the orientation of a cube. There are 24 different possible orientations (6 different top sides and 4 different side rotations of each) and the table entry
//indicates the new orientation obtained by applying a rotation from the second index, or column, to an existing orientation to the row, or first index.
//Table dervied from https://k-l-lambda.github.io/2020/12/14/rubik-cube-notation/

const int rotation_table[24][24] = {
    {alpha, beta, gamma, delta, epsilon, zeta, eta, theta, iota, kappa, lambda, mu, nu, xi, omicron, pi, rho, sigma, tau, upsilon, phi, chi, psi, omega},
    {beta, alpha, delta, gamma, theta, chi, omega, epsilon, phi, psi, pi, omicron, sigma, rho, mu, lambda, xi, nu, upsilon, tau, iota, zeta, kappa, eta},
    {gamma, delta, alpha, beta, tau, iota, psi, upsilon, zeta, omega, mu, lambda, rho, sigma, pi, omicron, nu, xi, epsilon, theta, chi, phi, eta, kappa},
    {delta, gamma, beta, alpha, upsilon, phi, kappa, tau, chi, eta, omicron, pi, xi, nu, lambda, mu, sigma, rho, theta, epsilon, zeta, iota, omega, psi},
    {epsilon, theta, upsilon, tau, beta, xi, lambda, alpha, nu, mu, omega, psi, phi, chi, kappa, eta, zeta, iota, gamma, delta, sigma, rho, omicron, pi},
    {zeta, phi, iota, chi, lambda, gamma, rho, omicron, alpha, xi, tau, epsilon, eta, omega, upsilon, theta, psi, kappa, mu, pi, delta, beta, nu, sigma},
    {eta, psi, omega, kappa, nu, lambda, delta, rho, pi, alpha, phi, iota, upsilon, epsilon, zeta, chi, tau, theta, sigma, xi, omicron, mu, gamma, beta},
    {theta, epsilon, tau, upsilon, alpha, rho, pi, beta, sigma, omicron, eta, kappa, iota, zeta, psi, omega, chi, phi, delta, gamma, nu, xi, mu, lambda},
    {iota, chi, zeta, phi, mu, alpha, nu, pi, gamma, sigma, epsilon, tau, psi, kappa, theta, upsilon, eta, omega, lambda, omicron, beta, delta, rho, xi},
    {kappa, omega, psi, eta, xi, omicron, alpha, sigma, mu, delta, zeta, chi, epsilon, upsilon, phi, iota, theta, tau, rho, nu, lambda, pi, beta, gamma},
    {lambda, omicron, pi, mu, phi, omega, tau, zeta, eta, epsilon, sigma, nu, delta, beta, xi, rho, gamma, alpha, iota, chi, kappa, psi, upsilon, theta},
    {mu, pi, omicron, lambda, chi, kappa, epsilon, iota, psi, tau, xi, rho, beta, delta, sigma, nu, alpha, gamma, zeta, phi, omega, eta, theta, upsilon},
    {nu, rho, xi, sigma, psi, epsilon, phi, eta, upsilon, iota, beta, gamma, omicron, mu, alpha, delta, lambda, pi, omega, kappa, theta, tau, zeta, chi},
    {xi, sigma, nu, rho, omega, upsilon, zeta, kappa, epsilon, chi, gamma, beta, lambda, pi, delta, alpha, omicron, mu, psi, eta, tau, theta, phi, iota},
    {omicron, lambda, mu, pi, zeta, psi, theta, phi, kappa, upsilon, rho, xi, alpha, gamma, nu, sigma, beta, delta, chi, iota, eta, omega, epsilon, tau},
    {pi, mu, lambda, omicron, iota, eta, upsilon, chi, omega, theta, nu, sigma, gamma, alpha, rho, xi, delta, beta, phi, zeta, psi, kappa, tau, epsilon},
    {rho, nu, sigma, xi, eta, tau, chi, psi, theta, zeta, delta, alpha, pi, lambda, gamma, beta, mu, omicron, kappa, omega, upsilon, epsilon, iota, phi},
    {sigma, xi, rho, nu, kappa, theta, iota, omega, tau, phi, alpha, delta, mu, omicron, beta, gamma, pi, lambda, eta, psi, epsilon, upsilon, chi, zeta},
    {tau, upsilon, theta, epsilon, delta, sigma, mu, gamma, rho, lambda, kappa, eta, chi, phi, omega, psi, iota, zeta, alpha, beta, xi, nu, pi, omicron},
    {upsilon, tau, epsilon, theta, gamma, nu, omicron, delta, xi, pi, psi, omega, zeta, iota, eta, kappa, phi, chi, beta, alpha, rho, sigma, lambda, mu},
    {phi, zeta, chi, iota, omicron, beta, sigma, lambda, delta, nu, theta, upsilon, kappa, psi, epsilon, tau, omega, eta, pi, mu, alpha, gamma, xi, rho},
    {chi, iota, phi, zeta, pi, delta, xi, mu, beta, rho, upsilon, theta, omega, eta, tau, epsilon, kappa, psi, omicron, lambda, gamma, alpha, sigma, nu},
    {psi, eta, kappa, omega, rho, mu, beta, nu, omicron, gamma, chi, zeta, theta, tau, iota, phi, epsilon, upsilon, xi, sigma, pi, lambda, alpha, delta},
    {omega, kappa, eta, psi, sigma, pi, gamma, xi, lambda, beta, iota, phi, tau, theta, chi, zeta, upsilon, epsilon, nu, rho, mu, omicron, delta, alpha}};

//This implements the above lookup table.
//PARAMETER: startingOrientation is the orientation of a corner before a rotation is applied
//PARAMETER: rotation is the orientation of a rotation to be applied to that corner
//RETURNS: returns an integer indicating the new orientation of the corner
//EX: calling lookupOrientation(delta, gamma) should return beta
int lookupOrientation(const int startingOrientation, const int rotation) {
    return rotation_table[startingOrientation][rotation];
}

/*There are 12 different quarter twists. Each of the following functions rotate the faces of a cube. These functions operate by labeling the 2x2 rubiks cube accordingly:
0 is the upper left front corner
1 is the upper left back corner
2 is the upper right back corner
3 is the upper right front corner
4 is the lower left front corner
5 is the lower left back corner
6 is the lower right back corner
7 is the lower right front corner
*/

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the upper face clockwise
void upperClockwise(cube& rubiks) {
    auto orientation0 = rubiks[0];
    auto orientation1 = rubiks[1];
    auto orientation2 = rubiks[2];
    auto orientation3 = rubiks[3];

    rubiks[0] = lookupOrientation(orientation3, zeta);
    rubiks[1] = lookupOrientation(orientation0, zeta);
    rubiks[2] = lookupOrientation(orientation1, zeta);
    rubiks[3] = lookupOrientation(orientation2, zeta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the upper face counterclockwise
void upperCounterClockwise(cube& rubiks) {
    auto orientation0 = rubiks[0];
    auto orientation1 = rubiks[1];
    auto orientation2 = rubiks[2];
    auto orientation3 = rubiks[3];

    rubiks[0] = lookupOrientation(orientation1, iota);
    rubiks[1] = lookupOrientation(orientation2, iota);
    rubiks[2] = lookupOrientation(orientation3, iota);
    rubiks[3] = lookupOrientation(orientation0, iota);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the bottom face clockwise
void downClockwise(cube&rubiks) {
    auto orientation4 = rubiks[4];
    auto orientation5 = rubiks[5];
    auto orientation6 = rubiks[6];
    auto orientation7 = rubiks[7];

    rubiks[4] = lookupOrientation(orientation5, iota);
    rubiks[5] = lookupOrientation(orientation6, iota);
    rubiks[6] = lookupOrientation(orientation7, iota);
    rubiks[7] = lookupOrientation(orientation4, iota);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the down face counterclockwise
void downCounterClockwise(cube& rubiks) {
    auto orientation4 = rubiks[4];
    auto orientation5 = rubiks[5];
    auto orientation6 = rubiks[6];
    auto orientation7 = rubiks[7];
    
    rubiks[4] = lookupOrientation(orientation7, zeta);
    rubiks[5] = lookupOrientation(orientation4, zeta);
    rubiks[6] = lookupOrientation(orientation5, zeta);
    rubiks[7] = lookupOrientation(orientation6, zeta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the front face clockwise
void frontClockwise(cube& rubiks) {
    auto orientation0 = rubiks[0];
    auto orientation3 = rubiks[3];
    auto orientation4 = rubiks[4];
    auto orientation7 = rubiks[7];

    rubiks[0] = lookupOrientation(orientation4, eta);
    rubiks[3] = lookupOrientation(orientation0, eta);
    rubiks[4] = lookupOrientation(orientation7, eta);
    rubiks[7] = lookupOrientation(orientation3, eta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the front face counterclockwise
void frontCounterClockwise(cube& rubiks) {
    auto orientation0 = rubiks[0];
    auto orientation3 = rubiks[3];
    auto orientation4 = rubiks[4];
    auto orientation7 = rubiks[7];

    rubiks[0] = lookupOrientation(orientation3, kappa);
    rubiks[3] = lookupOrientation(orientation7, kappa);
    rubiks[4] = lookupOrientation(orientation0, kappa);
    rubiks[7] = lookupOrientation(orientation4, kappa);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the back face clockwise
void backClockwise(cube& rubiks) {\
    auto orientation1 = rubiks[1];
    auto orientation2 = rubiks[2];
    auto orientation5 = rubiks[5];
    auto orientation6 = rubiks[6];

    rubiks[1] = lookupOrientation(orientation2, kappa);
    rubiks[2] = lookupOrientation(orientation6, kappa);
    rubiks[5] = lookupOrientation(orientation1, kappa);
    rubiks[6] = lookupOrientation(orientation5, kappa);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the back face counterclockwise
void backCounterClockwise(cube& rubiks) {
    auto orientation1 = rubiks[1];
    auto orientation2 = rubiks[2];
    auto orientation5 = rubiks[5];
    auto orientation6 = rubiks[6];
    
    rubiks[1] = lookupOrientation(orientation5, eta);
    rubiks[2] = lookupOrientation(orientation1, eta);
    rubiks[5] = lookupOrientation(orientation6, eta);
    rubiks[6] = lookupOrientation(orientation2, eta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the left face clockwise
void leftClockwise(cube& rubiks) {
    auto orientation0 = rubiks[0];
    auto orientation1 = rubiks[1];
    auto orientation4 = rubiks[4];
    auto orientation5 = rubiks[5];

    rubiks[0] = lookupOrientation(orientation1, theta);
    rubiks[1] = lookupOrientation(orientation5, theta);
    rubiks[4] = lookupOrientation(orientation0, theta);
    rubiks[5] = lookupOrientation(orientation4, theta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the left face counterclockwise
void leftCounterClockwise(cube& rubiks) {
    auto orientation0 = rubiks[0];
    auto orientation1 = rubiks[1];
    auto orientation4 = rubiks[4];
    auto orientation5 = rubiks[5];

    rubiks[0] = lookupOrientation(orientation4, epsilon);
    rubiks[1] = lookupOrientation(orientation0, epsilon);
    rubiks[4] = lookupOrientation(orientation5, epsilon);
    rubiks[5] = lookupOrientation(orientation1, epsilon);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the right face clockwise
void rightClockwise(cube& rubiks) {
    auto orientation2 = rubiks[2];
    auto orientation3 = rubiks[3];
    auto orientation6 = rubiks[6];
    auto orientation7 = rubiks[7];

    rubiks[2] = lookupOrientation(orientation3, epsilon);
    rubiks[3] = lookupOrientation(orientation7, epsilon);
    rubiks[6] = lookupOrientation(orientation2, epsilon);
    rubiks[7] = lookupOrientation(orientation6, epsilon);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the right face counterclockwise
void rightCounterClockwise(cube& rubiks) {
    auto orientation2 = rubiks[2];
    auto orientation3 = rubiks[3];
    auto orientation6 = rubiks[6];
    auto orientation7 = rubiks[7];

    rubiks[2] = lookupOrientation(orientation6, theta);
    rubiks[3] = lookupOrientation(orientation2, theta);
    rubiks[6] = lookupOrientation(orientation7, theta);
    rubiks[7] = lookupOrientation(orientation3, theta);
}

//Checks to see if a given cube has been solved.
//PARAMETER: rubiks is the cube to be checked
//RETURNS: A boolean that is true if the cube is solved and false otherwise
//If the corners all have the same orientation, it is a solved cube
bool isSolved(const cube& rubiks) {
    auto firstOrientation = rubiks[0];

    for (int i = 1; i < 8; i++) {
        if (rubiks[i] != firstOrientation) {
            return false;
        }
    }

    return true;
}

//This function generates a default cube
//PARAMETER: none
//RETURNS: a cube defaulted to a solved state
cube Cube() {
    return {alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha};
}

//This function takes a cube and resets all of its values to be a solved cube.
//PARAMETER: rubiks is the cube to be reset
//RETURNS: none
//SIDE EFFECTS: The passed cube's data is erased and reset to a default, solved version.
void reset(cube& rubiks) {
    rubiks = Cube();
}

//This function prints out the cube state.
//PARAMETER: rubiks is the cube to displayed.
//RETURNS: none
//SIDE EFFECTS: Prints out the orientations of the cubelets to the command line
std::string printCube(const cube rubiks) {
    return "{" + orientationLiterals[rubiks[0]] + ", " + orientationLiterals[rubiks[1]] + ", " + orientationLiterals[rubiks[2]] + ", " + orientationLiterals[rubiks[3]] + ", " + 
            orientationLiterals[rubiks[4]] + ", " + orientationLiterals[rubiks[5]] + ", " + orientationLiterals[rubiks[6]] + ", " + orientationLiterals[rubiks[7]] + "}";
}

bool isReverse(int action, int oldAction) {
    return ((action + oldAction) % 4 == 1 && abs(action - oldAction) == 1);
}

void applyMove(cube& rubiks, int move) {
    switch (move) {
        case 0:
            upperClockwise(rubiks);
            break;
        case 1:
            upperCounterClockwise(rubiks);
            break;
        case 2:
            downClockwise(rubiks);
            break;
        case 3:
            downCounterClockwise(rubiks);
            break;
        case 4:
            frontClockwise(rubiks);
            break;
        case 5:
            frontCounterClockwise(rubiks);
            break;
        case 6:
            backClockwise(rubiks);
            break;
        case 7:
            backCounterClockwise(rubiks);
            break;
        case 8:
            leftClockwise(rubiks);
            break;
        case 9:
            leftCounterClockwise(rubiks);
            break;
        case 10:
            rightClockwise(rubiks);
            break;
        case 11:
            rightCounterClockwise(rubiks);
            break;
    }
}

//This function randomizes a cube to a given depth of random turns
//The cube can make
//PARAMETER: rubiks is the cube to be randomized
//PARAMETER: depth is the number of turns to be made
//RETURNS: none
std::string randomize(cube& rubiks, const int depth, std::mt19937 moveGenerator) {
    int previousMove = 100;
    std::string moveOrder = "";
    std::uniform_int_distribution<int> distribution(0, 11);

    for (int i = 0; i < depth; i++) {
        int move = distribution(moveGenerator);

        if (isReverse(move, previousMove)) {
            i--;
            continue;
        } else {
            applyMove(rubiks, move);
            previousMove = move;
            moveOrder = moveOrder + actionLiterals[move] + " ";
        } 
    }

    if (isSolved(rubiks)) {
        return randomize(rubiks, depth, moveGenerator);
    }

    return moveOrder;
}

//This function calculates a heuristic according to the orientations of cubelets
//PARAMETER: rubiks is the state to be evaluated
//RETURNS: an integer indicating the number of different orientations between the eight cubelets minus one
//This is similar to a Hamming distance
int heuristic(const cube& rubiks) {
    std::set<int> uniqueOrientations(rubiks.begin(), rubiks.end());

    return uniqueOrientations.size() / 2;
}

/*This is a node struct for generating open trees using IDA*.*/
struct node {
    cube state; 
    node* parent;
    int action;
    int heuristic;
    int path_cost;
    bool isSolved;
};

std::vector<node*> generateChildren(node* scrambledCube) {
    std::vector<node*> children;

    for (int action = 0; action < 12; action++) {
        if (!isReverse(action, scrambledCube->action)) {
            cube newState = scrambledCube->state;
            applyMove(newState, action);
            auto child = new node{newState, scrambledCube, action, heuristic(newState), scrambledCube->path_cost + 1, isSolved(newState)};
            children.push_back(child);
        }
    }

    return children;
}

//Helper function for IDA*
node* limitedSearch(node* scrambledCube, int valueLimit, int* exploredNodes) {
    if (scrambledCube->isSolved) {
        return scrambledCube;
    }
    
    std::vector<node*> children = generateChildren(scrambledCube);

    for (auto child : children) {
        *exploredNodes = *exploredNodes + 1;
        if (child->heuristic + child->path_cost <= valueLimit) {
            node* solution = limitedSearch(child, valueLimit, exploredNodes);
            if (solution && solution->isSolved) {
                return solution;
            }
        }
    }

    return nullptr;
}

struct solveResult {
    node startState;
    int exploredNodes;
    std::chrono::nanoseconds timeCost;
    node* solution = nullptr;
};


//This function performs IDA* to find a solution
//PARAMETER: rubiks is the cube to be solved
//RETURNS: a node that is in a solved state if a solution is found, or null if no solution is found
solveResult solveIDA(node startNode) {
    node* solution = nullptr;
    int valueLimit = startNode.heuristic;
    int exploredNodes = 0;
    auto startTime = std::chrono::steady_clock::now();

    while (!solution) {
        solution = limitedSearch(&startNode, valueLimit, &exploredNodes);
        valueLimit++;
    }

    auto endTime = std::chrono::steady_clock::now();

    auto timeCost = endTime - startTime;

    return solveResult{startNode, exploredNodes, timeCost, solution};
}

std::string printSolutionMoves(solveResult* solution) {
    std::string output = "";
    node* itr = solution->solution;
    int numActions = 0;

    while (itr->parent) {
        output = output + actionLiterals[itr->action] + " ";
        itr = itr->parent;
        numActions++;
    }

    return output + "for " + std::to_string(numActions) + " total actions.\n";
}

std::string printSolutionInfo(solveResult* solution) {
    return printCube(solution->startState.state) + "\n" + std::to_string(solution->exploredNodes) + "\n" + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(solution->timeCost).count()) + "\n";
}

//Main function that runs and handles the command-line interface of allowing a user to interact with a cube
int main() {
    //Create output file
    std::ofstream idastar_output;
    idastar_output.open("idastar_output.txt");

    //Safety check
    if (!idastar_output.is_open()) {
        perror("Failed to open the output file.");
        return EXIT_FAILURE;
    }

    int depth = 1;
    std::chrono::nanoseconds longestCase = std::chrono::nanoseconds(0);

    while(longestCase < std::chrono::seconds(300) && depth < 15) {
        for (int i = 0; i < 10; i++) {
            std::mt19937 moveGenerator;
            auto playCube = Cube();
            moveGenerator.seed(i);

            std::string randomSequence = randomize(playCube, depth, moveGenerator);
            node startNode = {playCube, nullptr, -1, heuristic(playCube), 0, false};
            idastar_output << std::to_string(heuristic(playCube)) + "\n";
            auto solution = solveIDA(startNode);
            idastar_output << randomSequence + "\n";
            idastar_output << printSolutionMoves(&solution);
            idastar_output << printSolutionInfo(&solution) + "\n";
            if (solution.timeCost > longestCase) {
                longestCase = solution.timeCost;
            }
        }

        depth++;
    }
}