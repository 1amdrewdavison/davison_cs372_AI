#include <stdlib.h>
#include <array>
#include <string>
#include <fstream>
#include <sstream>

using puzzle = std::array<std::array<char, 25>, 25>;

puzzle readPuzzle(std::string filename) {
    std::ifstream file;
    file.open(filename);

    puzzle alpha;
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

void toDIMACS(puzzle alpha) {
    std::ofstream file;
    file.open("alphadoku.cnf");
    int clause_count = 0;
    //Need num of clauses
    file << "p cnf 15625 375625\n";

    //Put in game rules

    //At least one symbol in each entry
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            for (int sym = 1; sym <= 25; sym++) {
                file << row * 625 + col * 25 + sym << " ";
            }
            file << "0\n";
            clause_count++;
        }
    }

    //At most one symbol in each entry
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            for (int sym = 1; sym <= 25; sym++) {
                for (int other_sym = 1; other_sym <= 25; other_sym++) {
                    if (sym != other_sym) {
                        file << "-" << row * 625 + col * 25 + sym << " -" << row * 625 + col * 25 + other_sym << " 0\n";
                        clause_count++;
                    }
                }
            }
        }
    }

    //Only one symbol in each row
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            for (int sym = 1; sym <= 25; sym++) {
                for (int rest_of_row = col + 1; rest_of_row < 25; rest_of_row++) {
                    file << "-" << row * 625 + col * 25 + sym << " -" << row * 625 + (col + rest_of_row) * 25 + sym << " 0\n";
                    clause_count++;
                }
            }
        }
    }

    //Only one symbol in each column
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            for (int sym = 1; sym <= 25; sym++) {
                for (int rest_of_col = row + 1; rest_of_col < 25; rest_of_col++) {
                    file << "-" << row * 625 + col * 25 + sym << " -" << (row + rest_of_col) * 625 + col * 25 + sym << " 0\n";
                    clause_count++;
                }
            }
        }
    }

    

    file.close();
}

int main() {
    puzzle alpha = readPuzzle("./puzzles/alpha_0.txt");

    toDIMACS(alpha);

    system(".\\slime.exe alphadoku.cnf");
}