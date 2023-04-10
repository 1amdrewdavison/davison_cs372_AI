//Written by Andrew Davison for CSC 372: Survey of Artificial Intelligence
//This converts Alphadoku (25x25) puzzles into SAT, and then calls the SLIME SAT-solver to solve the puzzles

#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <memory>
#include <string>
#include <array>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using puzzle = std::array<std::array<char, 25>, 25>;

const char alphadoku_symbols[25] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y'};

//Reads in alphadoku puzzles
puzzle readPuzzle(std::string filename) {
    std::ifstream file;
    file.open(filename);

    puzzle alpha = {0};
    char info;
    int position = 0;

    while(file.get(info)) {
        switch (info) {
            case ' ':
            case '\n':
                //This is a space or newline
                break;
            default:
                //Insert char into the array
                alpha[(position/25)][(position%25)] = info;
                position++;
                break;
        }
    }

    file.close();

    return alpha;
}

//This calls SLIME to solve a puzzle once it is in CNF
std::istringstream runSAT() {
    std::array<char, 128> buf;
    std::string solution = "";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(".\\slime alphadoku_temp.cnf", "r"), pclose);

    while (fgets(buf.data(), buf.size(), pipe.get()) != nullptr) {
        solution += buf.data();
    }

    remove("alphadoku_temp.cnf");

    return std::istringstream(solution);
}

//This reads in the output from SLIME
puzzle readSolution(std::istringstream* solution) {
    std::string first, line;
    puzzle doku = {0};
    int position = 0;
    
    while(*solution >> first) {
        if (first == "c") {
            //Comment line
            std::getline(*solution, line);
        } else if (first == " " || first == "v") {
            //Ignore space or beginning of solution
        } else if (first == "Assertion"){
            //This means the solver failed
            return doku;
        } else if (first == "0") {
            //End when end of line indicator found
            break;
        } else {
            //Convert numbers into the alphabetic symbol and insert into data structure
            int var = std::stoi(first);
            if (var > 0) {
                doku[(position/25)][(position%25)] = alphadoku_symbols[(var - 1) % 25];
                position++;
            }
        }
    }

    return doku;
}

//Prints out first solution
int printSolution(puzzle alphadoku) {
    std::ofstream file;
    file.open("alphadoku_solution.txt");

    if (alphadoku[0][0] == 0) {
        file << "No solution";
        return 0;
    }
    
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            file << alphadoku[row][col] << " ";
            if ((col + 1) % 5 == 0) {
                file << " ";
            }
        }
        if (row != 24) {
            file << "\n";
        }
        if ((row + 1) % 5 == 0) {
            file << "\n";
        }
    }

    return 1;
}

//Prints out a second solution or confirms uniqueness
int printSecondSolution(puzzle alphadoku) {
    std::ofstream file;
    file.open("alphadoku_solution.txt", std::ios_base::app);

    if (alphadoku[0][0] == 0) {
        file << "\nSolution is unique.";
        return 0;
    }
    
    file << "\n\n\n";

    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            file << alphadoku[row][col] << " ";
            if ((col + 1) % 5 == 0) {
                file << " ";
            }
        }
        if (row != 24) {
            file << "\n";
        }
        if ((row + 1) % 5 == 0) {
            file << "\n";
        }
    }

    return 1;
}

//This counts the number of clauses needed for the assertions that make the puzzle
int countPuzzleClauses(puzzle alphadoku) {
    int counter = 0;
    
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            if (alphadoku[row][col] != '_') {
                counter++;
            }
        }
    }

    return counter;
}

//This turns a puzzle into SAT
void toDIMACS(puzzle alpha, puzzle firstSol) {
    std::ofstream file;
    file.open("alphadoku_temp.cnf");

    bool solutionFound = firstSol[0][0] != 0;

    int clause_count = countPuzzleClauses(alpha) + solutionFound;
    file << "p cnf 15625 " << 906875 + clause_count << "\n";

    //Put in game rules

    //At least one symbol in each entry
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            for (int sym = 1; sym <= 25; sym++) {
                file << row * 625 + col * 25 + sym << " ";
            }
            file << "0\n";
        }
    }

    //At most one symbol in each entry
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            for (int sym = 1; sym <= 25; sym++) {
                for (int other_sym = 1; other_sym <= 25; other_sym++) {
                    if (sym != other_sym) {
                        file << "-" << row * 625 + col * 25 + sym << " -" << row * 625 + col * 25 + other_sym << " 0\n";
                    }
                }
            }
        }
    }

    //Only one symbol in each row
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            for (int sym = 1; sym <= 25; sym++) {
                for (int rest_of_row = 1; rest_of_row < 25 - col; rest_of_row++) {
                    file << "-" << row * 625 + col * 25 + sym << " -" << row * 625 + (col + rest_of_row) * 25 + sym << " 0\n";
                }
            }
        }
    }

    //Only one symbol in each column
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            for (int sym = 1; sym <= 25; sym++) {
                for (int rest_of_col = 1; rest_of_col < 25 - row; rest_of_col++) {
                    file << "-" << row * 625 + col * 25 + sym << " -" << (row + rest_of_col) * 625 + col * 25 + sym << " 0\n";
                }
            }
        }
    }

    //Only one symbol in each 5x5 grid
    for (int sym = 1; sym <= 25; sym++) {
        for (int row_inc = 0; row_inc < 5; row_inc++) {
            for (int col_inc = 0; col_inc < 5; col_inc++) {
                for (int row = 0; row < 5; row++) {
                    for (int col = 0; col < 5; col++) {
                        for (int rest_of_rows = row + 1; rest_of_rows < 5; rest_of_rows++) {
                            for (int rest_of_cols = 0; rest_of_cols < 5; rest_of_cols++) {
                                file << "-" << (5 * row_inc + row) * 625 + (5 * col_inc + col) * 25 + sym << " -" << (5 * row_inc + rest_of_rows) * 625 + (5 * col_inc + rest_of_cols) * 25 + sym << " 0\n";
                            }
                        }
                    }
                }
            }
        }
    }

    //Add puzzle
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            if (alpha[row][col] != '_') {
                file << row * 625 + col * 25 + (int)alpha[row][col] - 64 << " 0\n";
            }
        }
    }

    //Add firstSolution (if applicable)
    if (solutionFound) {
        for (int row = 0; row < 25; row++) {
            for (int col = 0; col < 25; col++) {
                for (int sym = 1; sym <= 25; sym++) {
                    if ((int)firstSol[row][col] - 64 == sym) {
                        file << "-" << row * 625 + col * 25 + sym << " ";
                    } else {
                        file << row * 625 + col * 25 + sym << " ";
                    }
                }
            }
        }
        file << " 0\n";
    }

    file.close();
}

int main() {
    for (int i = 0; i <= 30; i++) {
        //First read in puzzle
        auto alpha = readPuzzle("./puzzles/alpha_" + std::to_string(i) + ".txt");
        //Then turn it into SAT
        puzzle empty = {0};
        toDIMACS(alpha, empty);
        //Then solve it with SAT
        auto solution = runSAT();
        //Then turn that back into the internal representation
        auto doku = readSolution(&solution);
        //And print it out
        auto isSAT = printSolution(doku);

        if (isSAT == 1) {
            //Try again
            toDIMACS(alpha, doku);
            auto secondSolution = runSAT();
            auto dokuTwo = readSolution(&secondSolution);
            printSecondSolution(dokuTwo);
        }

        //Copy output file into solutions folder
        std::ifstream src("alphadoku_solution.txt", std::ios::binary);
        std::ofstream dst("./solutions/alphadoku_solution_" + std::to_string(i) + ".txt", std::ios::binary);

        dst << src.rdbuf();
    }
}