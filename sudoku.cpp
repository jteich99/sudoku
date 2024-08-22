#include <iostream>
#include <map>
#include <vector>
#include <random>
#include <cstring>
#include <algorithm>
using namespace std;

using u32    = uint_least32_t; 
using engine = std::mt19937; 

class Sudoku {
public:
    int gameSize; // declare number of columns and rows
    int** matrix; // declare the matrix
    // pointer in memory to an array of arrays
    int* basicArray;
    int** solvedBoard;
    map<vector<int>,vector<int>> emptySpaces;
    
    // constructor method 
    Sudoku(int gameSizeSet)
    {
        this->gameSize = gameSizeSet;

        matrix = new int*[gameSize];
        for (int i = 0; i < gameSize; i++){
            matrix[i] = new int[gameSize];

            memset(matrix[i], 0, gameSize * sizeof(int));
        }

        basicArray = new int[9];
        memset(basicArray, 0, 9 * sizeof(int));

        printBoard();
        fillValues();
        printBoard();
        removeValues();
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
        // 1st we fill the 3 diagonal squares that are independent
        fillDiagonalSquares();

        // then we fill the rows taking into account the filled spaces
        fillRemainingSquares();
    };

    void fillDiagonalSquares(){
        // fill 1st diagonal square
        fillSquare(0,3);

        // fill 2nd diagonal square
        fillSquare(3,6);

        // fill 3rd diagonal square
        fillSquare(6,9);
    }

    void fillSquare(int min, int max){
        randomizeBasicArray();

        int counter = 0;
        for (int i = min; i < max; i++){
            for (int j = min; j < max; j++){
                matrix[i][j] = basicArray[counter];
                counter++;
            }
        }
    }

    void fillRemainingSquares(){
        int basicArrayCounter;
        bool valueNotInRow;
        bool valueNotInColumn;
        bool valueOkay;
        int randomValue;
        bool startOver = true;
        int counterBoards = 0;
        
        while (startOver) {
            startOver = false;
            cleanNonDiagonalSquares();
            for (int i = 0; i < 3; i++){ // i = square row index
                for (int j = 0; j < 3; j++){ // j = square column index
                    if (i != j) { // do not enter diagonals
                        randomizeBasicArray(); // random array to use to fill this square

                        for (int k = i * 3; k < i * 3 + 3; k++){ // k = row index
                            for (int m = j * 3; m < j * 3 + 3; m++){ // m = column index
                                basicArrayCounter = 0;
                                
                                valueOkay = false;
                                while (not valueOkay) {
                                    // while loop to check that value is not present in row and column
                                    
                                    if (basicArrayCounter > 9){
                                        startOver = true;
                                        // cout << "need to start over this square" << endl;
                                        break;
                                    }
                                    randomValue = basicArray[basicArrayCounter];

                                    if (randomValue != 0) {
                                        // check if value is in row or column
                                        valueNotInRow = checkValueNotInRow(randomValue, m);
                                        valueNotInColumn = checkValueNotInColumn(randomValue, k);  

                                        if (valueNotInRow && valueNotInColumn){
                                            valueOkay = true;
                                        } else {
                                            basicArrayCounter++;
                                        }
                                    } else {
                                        basicArrayCounter++;
                                    }
                                }
                                if (startOver){
                                    break;
                                }

                                // here we have a random value that has validated not being either in row nor column
                                matrix[k][m] = randomValue;
                                basicArray[basicArrayCounter] = 0;
                            }
                            if (startOver){
                                break;
                            }
                        }
                    }
                    if (startOver){
                        break;
                    }
                }
                if (startOver){
                    break;
                }
            }
            // printBoard();
            // if (startOver){
            //     cout << "board invalid. restarting board filling..." << endl;
            // }
            counterBoards++;
        }
        cout << "boards tried till success: " << counterBoards << endl;
    }

    bool checkValueNotInRow(int value, int columnIndex){
        bool returnCheck = true;
        for (int rowIndex = 0; rowIndex < 9; rowIndex++){
            if (matrix[rowIndex][columnIndex] == value){
                returnCheck = false;
            }
        }
        return returnCheck;
    }

    bool checkValueNotInColumn(int value, int rowIndex){
        bool returnCheck = true;
        for (int columnIndex = 0; columnIndex < 9; columnIndex++){
            if (matrix[rowIndex][columnIndex] == value){
                returnCheck = false;
            }
        }
        return returnCheck;
    }

    void cleanNonDiagonalSquares(){
        for (int i = 0; i < 3; i++){ // i = square row index
            for (int j = 0; j < 3; j++){ // j = square column index
            // for (int j = 0; j < 1; j++){ // j = square column index
                if (i != j) { // do not enter diagonals
                    for (int k = i * 3; k < i * 3 + 3; k++){ // k = row index
                        for (int m = j * 3; m < j * 3 + 3; m++){ // m = column index
                            matrix[k][m] = 0;
                        }
                    }
                }
            }
        }
    }

    void removeValues(){
        int previousValue;
        bool uniqueSolution;

        for (int repetition = 0; repetition < 60; repetition++){
            // get a random row and column index
            int rowNumber = getRandomValue(0,8);
            int columnNumber = getRandomValue(0,8);

            // remove value from matrix (and save it in case more than 1 solution is obtained)
            previousValue = matrix[rowNumber][columnNumber];
            matrix[rowNumber][columnNumber] = 0;
        }
        printBoard();

        uniqueSolution = testUniqueness();
    }

    bool testUniqueness(){
        // returns true if only 1 solution, returns false if at least 2 solutions
        
        
        // test possible solutions
        bool allPossibilitiesAnalyzed = false;
        int solutions = solveBoard();
        // while (solutions < 2 and not allPossibilitiesAnalyzed){
        //     

        // }


        // return condition
        if (solutions == 1) {
            return true;
        } else {
            return false;
        }
    }

    int solveBoard(int maxAllowedSolutions = 0) {
        solvedBoard = matrix; // create a copy of the gameBoard to solve it
        
        // find all empty spaces and get possible values to fill
        emptySpaces = possibleValues();

        // 1- fill positions that have only 1 possible value:
        fillPositionsWithUniqueValues();

        // 2- check if a value makes that a position has no possible values

        return 0;
    }

    void fillPositionsWithUniqueValues() {
        bool uniquePossibleValues = checkForUniquePossibleValues();
        int row;
        int column;
        vector<vector<int>> keysToDelete;
        
        // while there are positions with only 1 possible values
        while (uniquePossibleValues) {
            printPossibleValuesMap(emptySpaces);

            // 1- fill positions
            for (const auto& [key, value] : emptySpaces) {
                if (value.size() == 1) {
                    row = key[0];
                    column = key[1];
                    cout << row << ", " << column << " -> " << value[0] << endl;
                    solvedBoard[row][column] = value[0];
                    cout << "assigned value to solvedBoard" << endl;
                    keysToDelete.emplace(keysToDelete.end(), key);
                    cout << "erased key from empty spaces" << endl;
                }
            }

            for (const auto& key : keysToDelete) {
                emptySpaces.erase(key);
            }


            // 2- recheck possibleValues
            emptySpaces = possibleValues();
            uniquePossibleValues = checkForUniquePossibleValues();
        }
    }

    bool checkForUniquePossibleValues() {
        int counterUniquePossibleValues = 0;
        for (const auto& [key, value] : emptySpaces) {
            // for (int i: value) {
            //     cout << i << " ";
            // }
            // cout << "\n";
            // cout << "vector size = " << value.size() << endl;
            if (value.size() == 1) {
                counterUniquePossibleValues++;
            }
        }

        if (counterUniquePossibleValues > 0) {
            return true;
        } else {
            return false;
        }
    }


    map<vector<int>,vector<int>> possibleValues() {
        map<vector<int>,vector<int>> possibleValuesMap;
        for (int i = 0; i < 9; i++){ // i = row index
            for (int j = 0; j < 9; j++){ // j = column index
                if (matrix[i][j] == 0){
                    possibleValuesMap[{i,j}] = getPossibleValues(i,j);
                }
            }
        }

        return possibleValuesMap;
    }


    vector<int> getPossibleValues( int rowIndex, int columnIndex, bool defaultBoard = true , bool solvedBoard = false, bool testBoard = false){

        if (solvedBoard) {
            board = solvedBoard;
        } else if (testBoard) {
            board = testBoard;
        } else {
            board = matrix;
        }
        vector<int> possibleValues;
        bool rowCheck;
        bool columnCheck;

        for (int i = 1; i < 10; i++) {
            rowCheck = checkValueNotInRow(i, columnIndex);
            columnCheck = checkValueNotInColumn(i, rowIndex);

            if (rowCheck and columnCheck) {
                possibleValues.emplace(possibleValues.end(), i);
            }
        }
        return possibleValues;
    }

    void randomizeBasicArray(){
        std::random_device os_seed;
        const u32 seed = os_seed();

        engine generator( seed );
        std::uniform_int_distribution< u32 > distribute( 1, 9 );

        // reset basicArray to 0s 
        for (int i = 0; i < 9; i++) {
            basicArray[i] = 0; 
        }

        for( int repetition = 0; repetition < 9; ++repetition ) {
            int randomValue;
            bool checkAvailability = false;
            while (not checkAvailability) {
                randomValue = distribute( generator );

                checkAvailability = true;
                for (int i = 0; i < 9; i++) {
                    if (randomValue == basicArray[i]){
                        checkAvailability = false;
                    }
                }
            }

            basicArray[repetition] = randomValue;
        }
    }

    int getRandomValue(int min, int max) {
        std::random_device os_seed;
        const u32 seed = os_seed();

        engine generator( seed );
        std::uniform_int_distribution< u32 > distribute( min, max );

        return distribute( generator );
    }

    void printBasicArray(){
        cout << "basicArray = [";
        for (int i = 0; i < 9; i++) {
            if (i !=0 ) {
                cout << ", ";
            }
            cout << basicArray[i]; 
        }
        cout << "]\n";
    }
};

int main() {
    int gameSize = 9;
    Sudoku* sudoku = new Sudoku(gameSize);
}
