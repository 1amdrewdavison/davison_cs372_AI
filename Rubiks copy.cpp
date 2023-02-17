//Written by Andrew Davison for Project A1 for CSC 372: Artificial Intelligence with Dr. Thomas Allen
//Models a 2x2 Rubiks cube and provides a command line interface for a user to rotate a cube.
//This model is heavily dervied from the work of a blog post at https://k-l-lambda.github.io/2020/12/14/rubik-cube-notation/ that models a 3x3 cube.
//This model has been adapted by me into a 2x2 cube model.

#include <utility>
#include <vector>
#include <string>
#include <iostream>

//Enum for enumerating possible cube orientations and a mapping vector for printing out orientations
enum {alpha, beta, gamma, delta, epsilon, zeta, eta, theta, iota, kappa, lambda, mu, nu, xi, omicron, pi, rho, sigma, tau, upsilon, phi, chi, psi, omega};
std::vector<std::string> orientationLiterals = {"alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta", "iota", "kappa", "lambda", "mu", "nu", 
                                                "xi", "omicron", "pi", "rho", "sigma", "tau", "upsilon", "phi", "chi", "psi", "omega"};


//This is a lookup table for the orientation of a cube. There are 24 different possible orientations (6 different top sides and 4 different side rotations of each) and the table entry
//indicates the new orientation obtained by applying a rotation from the second index, or column, to an existing orientation to the row, or first index.
//Table dervied from https://k-l-lambda.github.io/2020/12/14/rubik-cube-notation/

int rotation_table[24][24] = {
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

/*This is a representation of the orientation of each cubelet that exists in the position of the index. This uniquely identifies the cubelet and 
uniquely identifies a state, although it is subject to prespective and does represent redundant states. Currentlry, this model is actually entirely ignorant of
the sticker colors of the cube, and instead is entirely reliant on cubelet orientations.*/
//Design derived from https://k-l-lambda.github.io/2020/12/14/rubik-cube-notation/
using cube = std::vector<int>;

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

//This function makes a new cube matrix that copies another existing cube object.
//PARAMETER: rubiks is the cube to be copied
//RETURNS: a new cube that has the same data as the cube passed in the parameter
cube clone(const cube rubiks) {
    cube copy = rubiks;
    return copy;
}

//This function prints out the cube state.
//PARAMETER: rubiks is the cube to displayed.
//RETURNS: none
//SIDE EFFECTS: Prints out the orientations of the cubelets to the command line
void printCube(const cube rubiks) {
    std::cout << orientationLiterals[rubiks[0]] << "\t" << orientationLiterals[rubiks[1]] << "\t" << orientationLiterals[rubiks[2]] << "\t" << orientationLiterals[rubiks[3]] << "\t" 
              << orientationLiterals[rubiks[4]] << "\t" << orientationLiterals[rubiks[5]] << "\t" << orientationLiterals[rubiks[6]] << "\t" << orientationLiterals[rubiks[7]] << "\n";
}

//Main function that runs and handles the command-line interface of allowing a user to interact with a cube
int main() {
    //Variables for input and workflow
    auto playCube = Cube();
    bool runInteractive = true;
    std::string action;

    while (runInteractive) {
        //Evaluate whether the cube is solved, and modify isSolvedState to indicate whether it is in later output
        auto isSolvedState = isSolved(playCube) ? "" : " not";

        //Display cube, its solved status, and prompt for an action
        std::cout << "Here is the current cube:\n";
        printCube(playCube);
        std::cout << "The cube is" << isSolvedState << " in a solved state.\n";
        std::cout << "Enter a move in standard Rubik's cube notation or X to exit the program:" << std::endl;
        std::cin >> action;

        //Execute action
        if (action == "X") {
            std::cout << "Terminating program.\n";
            runInteractive = false;
        } else if (action == "U") {
            upperClockwise(playCube);
        } else if (action == "U'") {
            upperCounterClockwise(playCube);
        } else if (action == "D") {
            downClockwise(playCube);
        } else if (action == "D'") {
            downCounterClockwise(playCube);
        } else if (action == "F") {
            frontClockwise(playCube);
        } else if (action == "F'") {
            frontCounterClockwise(playCube);
        } else if (action == "B") {
            backClockwise(playCube);
        } else if (action == "B'") {
            backCounterClockwise(playCube);
        } else if (action == "L") {
            leftClockwise(playCube);
        } else if (action == "L'") {
            leftCounterClockwise(playCube);
        } else if (action == "R") {
            rightClockwise(playCube);
        } else if (action == "R'") {
            rightCounterClockwise(playCube);
        } else {
            //Give feedback if error
            std::cout << "Invalid command. Please enter one of the following moves: U, U', D, D', F, F', B, B', L, L', R, R', or X to exit the program.\n";
        }
    }
}