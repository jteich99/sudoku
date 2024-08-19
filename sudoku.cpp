#include <iostream>
#include <cstring>
using namespace std;

class Sudoku {
public:
    int gameSize; // declare number of columns and rows
    int** matrix; // declare the matrix
    // pointer in memory to an array of arrays
    
    // constructor method 
    Sudoku(int gameSizeSet)
    {
        this->gameSize = gameSizeSet;

        matrix = new int*[gameSize];
        for (int i = 0; i < gameSize; i++){
            matrix[i] = new int[gameSize];

            memset(matrix[i], 0, gameSize * sizeof(int));
        }

        printBoard();
        fillValues();
        printBoard();
    };

    void printBoard() {
        for (int i = 0; i < gameSize; i++){
            if (i % 3 == 0){
                cout << "-------------------------\n";
            }
            for (int j = 0; j < gameSize; j++){
                if (j % 3 == 0) {
                    cout << "| ";
                }
                cout << matrix[i][j] << " ";
            }
            cout << "|\n";
        }
        cout << "-------------------------\n";
    }

    void fillValues() {
        matrix[0][0] = 1;
        matrix[1][1] = 1;
    };

    void fillDiagonalSquares(){
    }
};

int main() {
    int gameSize = 9;
    Sudoku* sudoku = new Sudoku(gameSize);
}
