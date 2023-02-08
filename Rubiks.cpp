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

/*This is a mapping from the starting positions to the current positions of the cube. All but one of the elements will be non-zero, which indicates an orientation of a corner,
or subcube. The row of the non-negatibe element indicates a corner that can move, and the column indicates a fixed position where the non-zero element currently is. For example,
call the corner in the first row A, which in position 0. Rotating the cube so that the corner is in the previous position, call it 1, where corner B used to be, would change
the table so that there existed some non-negative orientation in the second column, since it exists in the labeled 1 fixed position, and the first position, or column, would go to -1. */
//Table derived from https://k-l-lambda.github.io/2020/12/14/rubik-cube-notation/
using cube = std::vector<std::vector<int>>;

//This program, given a cube, finds the corner that is in the input fixed position and returns the label of that corner, along with its orientation.
//PARAMETER: rubiks is a 8x8 matrix that completely describes a 2x2 rubiks cube
//PARAMETER: position is an integer indicates a fixed position in the cube that does not change as it is rotated. This is important when making turns.
//RETURNS: returns a pair of integers where the first is the label of the corner in the given position, or the row in rubiks, and the second is the orientation of that corner
//SIDE EFFECT: after finding the corner, this function preemptively clears the position of data to simplify making turns.
//EX: Consider a cube with the 3 corner in position 0 with orientation pi. Calling this function with findCornerInPosition(rubiks, 0) should return <3, pi>
std::pair<int, int> findCornerInPosition(cube& rubiks, const int position) {
    for (int i = 0; i < 8; i++) {
        if (rubiks[i][position] != -1) {
            auto tempOrientation = rubiks[i][position];
            rubiks[i][position] = -1;
            return std::pair<int, int>{i, tempOrientation};
        }
    }
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
    auto position0Corner = findCornerInPosition(rubiks, 0);
    auto position1Corner = findCornerInPosition(rubiks, 1);
    auto position2Corner = findCornerInPosition(rubiks, 2);
    auto position3Corner = findCornerInPosition(rubiks, 3);
    
    rubiks[position0Corner.first][1] = lookupOrientation(position0Corner.second, zeta);
    rubiks[position1Corner.first][2] = lookupOrientation(position1Corner.second, zeta);
    rubiks[position2Corner.first][3] = lookupOrientation(position2Corner.second, zeta);
    rubiks[position3Corner.first][0] = lookupOrientation(position3Corner.second, zeta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the upper face counterclockwise
void upperCounterClockwise(cube& rubiks) {
    auto position0Corner = findCornerInPosition(rubiks, 0);
    auto position1Corner = findCornerInPosition(rubiks, 1);
    auto position2Corner = findCornerInPosition(rubiks, 2);
    auto position3Corner = findCornerInPosition(rubiks, 3);
    
    rubiks[position0Corner.first][3] = lookupOrientation(position0Corner.second, iota);
    rubiks[position1Corner.first][0] = lookupOrientation(position1Corner.second, iota);
    rubiks[position2Corner.first][1] = lookupOrientation(position2Corner.second, iota);
    rubiks[position3Corner.first][2] = lookupOrientation(position3Corner.second, iota);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the bottom face clockwise
void downClockwise(cube&rubiks) {
    auto position4Corner = findCornerInPosition(rubiks, 4);
    auto position5Corner = findCornerInPosition(rubiks, 5);
    auto position6Corner = findCornerInPosition(rubiks, 6);
    auto position7Corner = findCornerInPosition(rubiks, 7);
    
    rubiks[position4Corner.first][7] = lookupOrientation(position4Corner.second, iota);
    rubiks[position5Corner.first][4] = lookupOrientation(position5Corner.second, iota);
    rubiks[position6Corner.first][5] = lookupOrientation(position6Corner.second, iota);
    rubiks[position7Corner.first][6] = lookupOrientation(position7Corner.second, iota);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the down face counterclockwise
void downCounterClockwise(cube& rubiks) {
    auto position4Corner = findCornerInPosition(rubiks, 4);
    auto position5Corner = findCornerInPosition(rubiks, 5);
    auto position6Corner = findCornerInPosition(rubiks, 6);
    auto position7Corner = findCornerInPosition(rubiks, 7);
    
    rubiks[position4Corner.first][5] = lookupOrientation(position4Corner.second, zeta);
    rubiks[position5Corner.first][6] = lookupOrientation(position5Corner.second, zeta);
    rubiks[position6Corner.first][7] = lookupOrientation(position6Corner.second, zeta);
    rubiks[position7Corner.first][4] = lookupOrientation(position7Corner.second, zeta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the front face clockwise
void frontClockwise(cube& rubiks) {
    auto position0Corner = findCornerInPosition(rubiks, 0);
    auto position3Corner = findCornerInPosition(rubiks, 3);
    auto position4Corner = findCornerInPosition(rubiks, 4);
    auto position7Corner = findCornerInPosition(rubiks, 7);
    
    rubiks[position0Corner.first][3] = lookupOrientation(position0Corner.second, eta);
    rubiks[position3Corner.first][7] = lookupOrientation(position3Corner.second, eta);
    rubiks[position4Corner.first][0] = lookupOrientation(position4Corner.second, eta);
    rubiks[position7Corner.first][4] = lookupOrientation(position7Corner.second, eta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the front face counterclockwise
void frontCounterClockwise(cube& rubiks) {
    auto position0Corner = findCornerInPosition(rubiks, 0);
    auto position3Corner = findCornerInPosition(rubiks, 3);
    auto position4Corner = findCornerInPosition(rubiks, 4);
    auto position7Corner = findCornerInPosition(rubiks, 7);
    
    rubiks[position0Corner.first][4] = lookupOrientation(position0Corner.second, kappa);
    rubiks[position3Corner.first][0] = lookupOrientation(position3Corner.second, kappa);
    rubiks[position4Corner.first][7] = lookupOrientation(position4Corner.second, kappa);
    rubiks[position7Corner.first][3] = lookupOrientation(position7Corner.second, kappa);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the back face clockwise
void backClockwise(cube& rubiks) {
    auto position1Corner = findCornerInPosition(rubiks, 1);
    auto position2Corner = findCornerInPosition(rubiks, 2);
    auto position5Corner = findCornerInPosition(rubiks, 5);
    auto position6Corner = findCornerInPosition(rubiks, 6);
    
    rubiks[position1Corner.first][5] = lookupOrientation(position1Corner.second, kappa);
    rubiks[position2Corner.first][1] = lookupOrientation(position2Corner.second, kappa);
    rubiks[position5Corner.first][6] = lookupOrientation(position5Corner.second, kappa);
    rubiks[position6Corner.first][2] = lookupOrientation(position6Corner.second, kappa);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the back face counterclockwise
void backCounterClockwise(cube& rubiks) {
    auto position1Corner = findCornerInPosition(rubiks, 1);
    auto position2Corner = findCornerInPosition(rubiks, 2);
    auto position5Corner = findCornerInPosition(rubiks, 5);
    auto position6Corner = findCornerInPosition(rubiks, 6);
    
    rubiks[position1Corner.first][2] = lookupOrientation(position1Corner.second, eta);
    rubiks[position2Corner.first][6] = lookupOrientation(position2Corner.second, eta);
    rubiks[position5Corner.first][1] = lookupOrientation(position5Corner.second, eta);
    rubiks[position6Corner.first][5] = lookupOrientation(position6Corner.second, eta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the left face clockwise
void leftClockwise(cube& rubiks) {
    auto position0Corner = findCornerInPosition(rubiks, 0);
    auto position1Corner = findCornerInPosition(rubiks, 1);
    auto position4Corner = findCornerInPosition(rubiks, 4);
    auto position5Corner = findCornerInPosition(rubiks, 5);
    
    rubiks[position0Corner.first][4] = lookupOrientation(position0Corner.second, theta);
    rubiks[position1Corner.first][0] = lookupOrientation(position1Corner.second, theta);
    rubiks[position4Corner.first][5] = lookupOrientation(position4Corner.second, theta);
    rubiks[position5Corner.first][1] = lookupOrientation(position5Corner.second, theta);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the left face counterclockwise
void leftCounterClockwise(cube& rubiks) {
    auto position0Corner = findCornerInPosition(rubiks, 0);
    auto position1Corner = findCornerInPosition(rubiks, 1);
    auto position4Corner = findCornerInPosition(rubiks, 4);
    auto position5Corner = findCornerInPosition(rubiks, 5);
    
    rubiks[position0Corner.first][1] = lookupOrientation(position0Corner.second, epsilon);
    rubiks[position1Corner.first][5] = lookupOrientation(position1Corner.second, epsilon);
    rubiks[position4Corner.first][0] = lookupOrientation(position4Corner.second, epsilon);
    rubiks[position5Corner.first][4] = lookupOrientation(position5Corner.second, epsilon);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the right face clockwise
void rightClockwise(cube& rubiks) {
    auto position2Corner = findCornerInPosition(rubiks, 2);
    auto position3Corner = findCornerInPosition(rubiks, 3);
    auto position6Corner = findCornerInPosition(rubiks, 6);
    auto position7Corner = findCornerInPosition(rubiks, 7);
    
    rubiks[position2Corner.first][6] = lookupOrientation(position2Corner.second, epsilon);
    rubiks[position3Corner.first][2] = lookupOrientation(position3Corner.second, epsilon);
    rubiks[position6Corner.first][7] = lookupOrientation(position6Corner.second, epsilon);
    rubiks[position7Corner.first][3] = lookupOrientation(position7Corner.second, epsilon);
}

//PARAMETER: rubiks is the cube to be rotated
//RETURNS: None
//SIDE EFFECTS: rotates the corners of the right face counterclockwise
void rightCounterClockwise(cube& rubiks) {
    auto position2Corner = findCornerInPosition(rubiks, 2);
    auto position3Corner = findCornerInPosition(rubiks, 3);
    auto position6Corner = findCornerInPosition(rubiks, 6);
    auto position7Corner = findCornerInPosition(rubiks, 7);
    
    rubiks[position2Corner.first][3] = lookupOrientation(position2Corner.second, theta);
    rubiks[position3Corner.first][7] = lookupOrientation(position3Corner.second, theta);
    rubiks[position6Corner.first][2] = lookupOrientation(position6Corner.second, theta);
    rubiks[position7Corner.first][6] = lookupOrientation(position7Corner.second, theta);
}

//TODO: The following section of code requires implementation.

//Checks to see if a given cube has been solved.
//PARAMETER: rubiks is the cube to be checked
//RETURNS: A boolean that is true if the cube is solved and false otherwise
bool isSolved(const cube& rubiks) {

}

//This function takes a cube and resets all of its values to be a solved cube.
//PARAMETER: rubiks is the cube to be reset
//RETURNS: none
//SIDE EFFECTS: The passed cube's data is erased and reset to a default, solved version.
void reset(cube& rubiks) {

}

//This function makes a new cube matrix that copies another existing cube object.
//PARAMETER: rubiks is the cube to be copied
//RETURNS: a new cube that has the same data as the cube passed in the parameter
cube clone(const cube rubiks) {

}

//This function generates a default cube
//PARAMETER: none
//RETURNS: a cube defaulted to a solved state
cube Cube() {
    return {
        {alpha, -1, -1, -1, -1, -1, -1, -1},
        {-1, alpha, -1, -1, -1, -1, -1, -1}, 
        {-1, -1, alpha, -1, -1, -1, -1, -1},
        {-1, -1, -1, alpha, -1, -1, -1, -1},
        {-1, -1, -1, -1, alpha, -1, -1, -1}, 
        {-1, -1, -1, -1, -1, alpha, -1, -1}, 
        {-1, -1, -1, -1, -1, -1, alpha, -1}, 
        {-1, -1, -1, -1, -1, -1, -1, alpha}};
}

//Main function that runs and handles the command-line interface of allowing a user to interact with a cube
int main() {

}