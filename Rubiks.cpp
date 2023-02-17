//Written by Andrew Davison for Project A1 for CSC 372: Artificial Intelligence with Dr. Thomas Allen
//Models a 2x2 Rubiks cube and provides a command line interface for a user to rotate a cube.
//This model is heavily dervied from the work of a blog post at https://k-l-lambda.github.io/2020/12/14/rubik-cube-notation/ that models a 3x3 cube.
//This model has been adapted by me into a 2x2 cube model.

#include <utility>
#include <vector>

//This is a lookup table for the orientation of a cube. There are 24 different possible orientations (6 different top sides and 4 different side rotations of each) and the table entry
//indicates the new orientation obtained by applying a rotation from the second index, or column, to an existing orientation to the row, or first index.
//Table dervied from https://k-l-lambda.github.io/2020/12/14/rubik-cube-notation/

enum {alpha, beta, gamma, delta, epsilon, zeta, eta, theta, iota, kappa, lambda, mu, nu, xi, omicron, pi, rho, sigma, tau, upsilon, phi, chi, psi, omega};

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

/*This is a mapping from the starting positions to the current positions of the cube and an orientation. A row indicates the cubelet considered, and the first number in
the tuple is a indication of its current position in the fixed cube. The second number in the tuple is the cubelet's current orientation. */
//Table derived from https://k-l-lambda.github.io/2020/12/14/rubik-cube-notation/
using cube = std::vector<std::pair<int, int>>;

/*There are 12 different quarter twists. Each of the following functions rotate the faces of a cube. These functions operate by labeling the 2x2 rubiks cube accordingly:
0 is the upper left front corner (arbitrarily White, Blue, Red)
1 is the upper left back corner (White, Red, Green)
2 is the upper right back corner (White, Green, Orange)
3 is the upper right front corner (White, Orange Blue)
4 is the lower left front corner (Red, Blue, Yellow)
5 is the lower left back corner (Red, Green, Yellow)
6 is the lower right back corner (Green, Orange, Yellow)
7 is the lower right front corner (Orange, Blue, Yellow)
*/

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the upper face clockwise
void upperClockwise(cube& rubiks) {
    rubiks[0].first = 1;
    rubiks[0].second = lookupOrientation(rubiks[0].second, zeta);
    rubiks[1].first = 2;
    rubiks[1].second = lookupOrientation(rubiks[1].second, zeta);
    rubiks[2].first = 3;
    rubiks[2].second = lookupOrientation(rubiks[2].second, zeta);
    rubiks[3].first = 0;
    rubiks[3].second = lookupOrientation(rubiks[3].second, zeta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the upper face counterclockwise
void upperCounterClockwise(cube& rubiks) {
    rubiks[0].first = 3;
    rubiks[0].second = lookupOrientation(rubiks[0].second, iota);
    rubiks[1].first = 0;
    rubiks[1].second = lookupOrientation(rubiks[1].second, iota);
    rubiks[2].first = 1;
    rubiks[2].second = lookupOrientation(rubiks[2].second, iota);
    rubiks[3].first = 2;
    rubiks[3].second = lookupOrientation(rubiks[3].second, iota);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the bottom face clockwise
void downClockwise(cube&rubiks) {
    rubiks[4].first = 7;
    rubiks[4].second = lookupOrientation(rubiks[4].second, iota);
    rubiks[5].first = 4;
    rubiks[5].second = lookupOrientation(rubiks[5].second, iota);
    rubiks[6].first = 5;
    rubiks[6].second = lookupOrientation(rubiks[6].second, iota);
    rubiks[7].first = 6;
    rubiks[7].second = lookupOrientation(rubiks[7].second, iota);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the down face counterclockwise
void downCounterClockwise(cube& rubiks) {
    rubiks[4].first = 5;
    rubiks[4].second = lookupOrientation(rubiks[4].second, zeta);
    rubiks[5].first = 6;
    rubiks[5].second = lookupOrientation(rubiks[5].second, zeta);
    rubiks[6].first = 7;
    rubiks[6].second = lookupOrientation(rubiks[6].second, zeta);
    rubiks[7].first = 4;
    rubiks[7].second = lookupOrientation(rubiks[7].second, zeta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the front face clockwise
void frontClockwise(cube& rubiks) {
    rubiks[0].first = 3;
    rubiks[0].second = lookupOrientation(rubiks[0].second, eta);
    rubiks[3].first = 7;
    rubiks[3].second = lookupOrientation(rubiks[3].second, eta);
    rubiks[4].first = 0;
    rubiks[4].second = lookupOrientation(rubiks[4].second, eta);
    rubiks[7].first = 4;
    rubiks[7].second = lookupOrientation(rubiks[7].second, eta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the front face counterclockwise
void frontCounterClockwise(cube& rubiks) {
    rubiks[0].first = 4;
    rubiks[0].second = lookupOrientation(rubiks[0].second, kappa);
    rubiks[3].first = 0;
    rubiks[3].second = lookupOrientation(rubiks[3].second, kappa);
    rubiks[4].first = 7;
    rubiks[4].second = lookupOrientation(rubiks[4].second, kappa);
    rubiks[7].first = 3;
    rubiks[7].second = lookupOrientation(rubiks[7].second, kappa);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the back face clockwise
void backClockwise(cube& rubiks) {
    rubiks[1].first = 5;
    rubiks[1].second = lookupOrientation(rubiks[1].second, kappa);
    rubiks[2].first = 1;
    rubiks[2].second = lookupOrientation(rubiks[2].second, kappa);
    rubiks[5].first = 6;
    rubiks[5].second = lookupOrientation(rubiks[5].second, kappa);
    rubiks[6].first = 2;
    rubiks[6].second = lookupOrientation(rubiks[6].second, kappa);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the back face counterclockwise
void backCounterClockwise(cube& rubiks) {
    rubiks[1].first = 2;
    rubiks[1].second = lookupOrientation(rubiks[1].second, eta);
    rubiks[2].first = 6;
    rubiks[2].second = lookupOrientation(rubiks[2].second, eta);
    rubiks[5].first = 1;
    rubiks[5].second = lookupOrientation(rubiks[5].second, eta);
    rubiks[6].first = 5;
    rubiks[6].second = lookupOrientation(rubiks[6].second, eta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the left face clockwise
void leftClockwise(cube& rubiks) {
    rubiks[0].first = 4;
    rubiks[0].second = lookupOrientation(rubiks[0].second, theta);
    rubiks[1].first = 0;
    rubiks[1].second = lookupOrientation(rubiks[1].second, theta);
    rubiks[4].first = 5;
    rubiks[4].second = lookupOrientation(rubiks[4].second, theta);
    rubiks[5].first = 1;
    rubiks[5].second = lookupOrientation(rubiks[5].second, theta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the left face counterclockwise
void leftCounterClockwise(cube& rubiks) {
    rubiks[0].first = 1;
    rubiks[0].second = lookupOrientation(rubiks[0].second, epsilon);
    rubiks[1].first = 5;
    rubiks[1].second = lookupOrientation(rubiks[1].second, epsilon);
    rubiks[4].first = 0;
    rubiks[4].second = lookupOrientation(rubiks[4].second, epsilon);
    rubiks[5].first = 4;
    rubiks[5].second = lookupOrientation(rubiks[5].second, epsilon);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the right face clockwise
void rightClockwise(cube& rubiks) {
    rubiks[2].first = 6;
    rubiks[2].second = lookupOrientation(rubiks[2].second, epsilon);
    rubiks[3].first = 2;
    rubiks[3].second = lookupOrientation(rubiks[3].second, epsilon);
    rubiks[6].first = 7;
    rubiks[6].second = lookupOrientation(rubiks[6].second, epsilon);
    rubiks[7].first = 3;
    rubiks[7].second = lookupOrientation(rubiks[7].second, epsilon);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the right face counterclockwise
void rightCounterClockwise(cube& rubiks) {
    rubiks[2].first = 3;
    rubiks[2].second = lookupOrientation(rubiks[2].second, theta);
    rubiks[3].first = 7;
    rubiks[3].second = lookupOrientation(rubiks[3].second, theta);
    rubiks[6].first = 2;
    rubiks[6].second = lookupOrientation(rubiks[6].second, theta);
    rubiks[7].first = 6;
    rubiks[7].second = lookupOrientation(rubiks[7].second, theta);
}

//Checks to see if a given cube has been solved.
//PARAMETER: rubiks is the cube to be checked
//RETURNS: A boolean that is true if the cube is solved and false otherwise
//If the corners all have the same orientation, it is a solved cube
bool isSolved(const cube& rubiks) {
    auto firstOrientation = rubiks[0].second;

    for (int i = 1; i < 8; i++) {
        if (rubiks[i].second != firstOrientation) {
            return false;
        }
    }

    return true;
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

//This function generates a default cube
//PARAMETER: none
//RETURNS: a cube defaulted to a solved state
cube Cube() {
    return {{0, alpha},
            {1, alpha},
            {2, alpha},
            {3, alpha},
            {4, alpha},
            {5, alpha},
            {6, alpha},
            {7, alpha}};
}

//This function takes the internal data structure and outputs it as a readable net of the cube
void displayCube(const cube& rubiks) {
    
}

//Main function that runs and handles the command-line interface of allowing a user to interact with a cube
int main() {
    auto test = Cube();

    rightClockwise(test);
    leftCounterClockwise(test);
}