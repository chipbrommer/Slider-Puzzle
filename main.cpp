/*
This program will solve a slider puzzle. The user can choose to enter a puzzle,
input one from a file, pick a premade one, or randomly generate one. The
program will then ask the user how they would like to solve the puzzle using 1
of 4 possible methods.
 
By: Chip Brommer
Contact: fredrick.brommer@usm.edu
Date: October 20, 2018
*/

#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <algorithm>
#include <stack>
#include <queue>
#include <chrono>
#include <list>
#include <vector>

using namespace std;

//Variable to hold the length of the puzzle
#define PUZZLE_LENGTH 9

//This is the struct defined to be used in out list when the search is being performed
struct State
{
    int currentState[PUZZLE_LENGTH];    //Holds the current array (puzzle)
    int spaceLocation;                  //Holds the location of the empty slot
    int misplacedTiles;                 //Holds the number of misplaced tiles
    int manhattanDistance;              //Holds the number of manhattan distance
    int moveCount;                      //Holds the number of times the space has moved
    
    State* parent;                      //Holds the parent of the node for tracing path
};

// Comparison object to be used to order the heap for Misplaced tles
struct comp
{
    bool operator()(const State* lhs, const State* rhs) const
    {
        return (lhs->misplacedTiles) > (rhs->misplacedTiles);
    }
};

// Comparison object to be used to order the heap for Manhattan Distance
struct mdcomp
{
    bool operator()(const State* lhs, const State* rhs) const
    {
        return (lhs->manhattanDistance) > (rhs->manhattanDistance);
    }
};

//Function Definitions
void welcomeMessage(fstream&);
void puzzleMainMenu(fstream&);
void mainMenuSwitch(int, fstream&);
void howToSolveMenu(int[], fstream&);
void solveMenuSwitch(int, int[], fstream&);
void puzzleEntry(fstream&);
void puzzleLoader(fstream&);
void premadePuzzles(fstream&);
void premadePuzzleSwitch(int, fstream&);
void puzzleGenerator(fstream&);
void programCredits(fstream&);
void exitProgram(fstream&);
void breadthFirstSearch(int[], fstream&);
void depthFirstSearch(int[], fstream&);
void misplacedTilesSearch(int[], fstream&);
void manhattenDistanceSearch(int[], fstream&);
void puzzlePrinter(int[], fstream&);
bool puzzleInputVerifier(int, int, fstream&);
bool integerVerify(int);
bool puzzleSolved(int);
int findEmptySpace(int[]);
void pressToContinue(fstream&);
bool moveUp(int);
bool moveRight(int);
bool moveDown(int);
bool moveLeft(int);
bool listSearch(int);
int countMisplacedTiles(int[], fstream&);
void printPath(State*, int, fstream&);
void tilesPrintPath(State*, int, fstream&);
void distancePrintPath(State*, int, fstream&);
bool isSolvable(int[]);
int getInvCount(int[]);
void useSameArray(int[], fstream&);
int getManhattanDistance(int[], fstream&);
void getRowAndColumn(int[], int, int&, int&);
int arrayToIntConverter(int[]);
void puzzleInvalidMenuSwitch(int, int[], fstream&);
void puzzleInvalidMenu(fstream&, int[]);

//Holds the ideal state of the puzzle
int solvedPuzzle[PUZZLE_LENGTH] = {1,2,3,4,5,6,7,8,0};

//Number to hold the user input number and initializing to 9
int checkArray[PUZZLE_LENGTH] = {9,9,9,9,9,9,9,9,9};

//Creating the linked list.
list <int> puzzleList;

//----------------------------------------------------------------------
//newState - Functional to allocate a new node
//----------------------------------------------------------------------
State* newState(int puzzleArray[], int space, int moveCount, int newSpace, State* parent)
{
    //Creating a new state
    State* state = new State;
    
    //Setting the pointer for the path to the root
    state->parent = parent;
    
    //Copy the state from the parent state to the new state
    memcpy(state->currentState, puzzleArray, sizeof state->currentState);
    
    //Moving the empty space
    swap(state->currentState[space], state->currentState[newSpace]);
    
    //Setting the number of misplaced tiles or manhattenDistance
    state->misplacedTiles = INT_MAX;
    
    //Set the number of moves
    state->moveCount = moveCount;
    
    //Updating the location of the new space location
    state->spaceLocation = newSpace;
    
    return state;
}

//----------------------------------------------------------------------
//Main - main function
//----------------------------------------------------------------------
int main(int argc, const char * argv[])
{
    //Creating new txt file for console output
    fstream outputFile;
    outputFile.open("AI_SliderPuzzle_Output.txt", fstream::out);
    
    //Time based random number generator for the program.
    //This is in main so it only gets called once and starts upon program run.
    srand((unsigned int) time(NULL));
    
    //Welcome message
    welcomeMessage(outputFile);
    
    //Calling the main menu
    puzzleMainMenu(outputFile);
}

//----------------------------------------------------------------------
//welcomeMessage - This is the welcome message for the program
//----------------------------------------------------------------------
void welcomeMessage(fstream& outputFile)
{
    cout << "\n\n\t\tWelcome to Chips Slider Puzzle\n";
    cout << "\t-------------------------------------\n\n";
    
    outputFile << "\n\n\t\tWelcome to Chips Slider Puzzle\n";
    outputFile << "\t-------------------------------------\n\n";
}

//----------------------------------------------------------------------
//puzzleMenu - This is the main menu for the program
//----------------------------------------------------------------------
void puzzleMainMenu(fstream& outputFile)
{
    //variable to hold the user selection
    int mainMenuSelection = 0;
    
    //Menu with options
    cout << "\tMenu:\n";
    cout << "\t\t1: Enter a puzzle." << endl;
    cout << "\t\t2: Use a user created puzzle." << endl;
    cout << "\t\t3: Use a pre-made puzzle." << endl;
    cout << "\t\t4: Use a generated puzzle." << endl;
    cout << "\t\t5: Credits." << endl;
    cout << "\t\t6: Exit Program." << endl;
    cout << "\n\t\t"; //This is a spacer and tab line for the user input so it aligns with the menu
    
    //Printing menu to output file
    outputFile << "\tMenu:\n";
    outputFile << "\t\t1: Enter a puzzle." << endl;
    outputFile << "\t\t2: Use a user created puzzle." << endl;
    outputFile << "\t\t3: Use a pre-made puzzle." << endl;
    outputFile << "\t\t4: Use a generated puzzle." << endl;
    outputFile << "\t\t5: Credits." << endl;
    outputFile << "\t\t6: Exit Program." << endl;
    outputFile << "\n\t\t"; //This is a spacer and tab line for the user input so it aligns with the menu
    
    //User enters their selection here.
    cout << "Selection: ";
    cin >> mainMenuSelection;
    
    //Verifies input is an integer. IF NOT, requests new input
    while(!integerVerify(mainMenuSelection))
    {
        cout << "Please Enter a Selection: ";
        cin >> mainMenuSelection;
    }
    
    //Spacer Line
    cout << endl;
    
    //Displaying the user selection to the output file
    outputFile << "User Selection: " << mainMenuSelection << endl << endl;
    
    //Calling the switch for the main menu selection
    mainMenuSwitch(mainMenuSelection, outputFile);
}

//---------------------------------------------------------------------------------------
//mainMenuSwitch - This function uses a switch-case for the user entered selection.
//---------------------------------------------------------------------------------------
void mainMenuSwitch(int mainMenuSwitch, fstream& outputFile)
{
    //Setting up the switch for the user selection
    switch (mainMenuSwitch)
    {
        case 1://Enter a puzzle
        {
            puzzleEntry(outputFile);
            break;
        }
        case 2://Load a user created puzzle
        {
            puzzleLoader(outputFile);
            break;
        }
        case 3://Load a premade puzzle
        {
            premadePuzzles(outputFile);
            break;
        }
        case 4://Generate a puzzle
        {
            puzzleGenerator(outputFile);
            break;
        }
        case 5://Program credits
        {
            programCredits(outputFile);
            break;
        }
        case 6://Exit the program
        {
            exitProgram(outputFile);
            break;
        }
        default://Invalid selection
        {
            cout << "\t::Invalid Selection::\n";
            puzzleMainMenu(outputFile);
            break;
        }
    }
}

//---------------------------------------------------------------------------------------
//howToSolveMenu - This will ask the user how they prefer to solve the puzzle
//---------------------------------------------------------------------------------------
void howToSolveMenu(int puzzleArray[], fstream& outputFile)
{
    //Variable to hold the users selection
    int solveMenuSelection = 0;
    
    //Printing the menu
    cout << "Please select a method to solve the puzzle.\n\n";
    cout << "\tSolving Methdods:\n";
    cout << "\t\t1: Breadth First Search.\n";
    cout << "\t\t2: Depth First Search.\n";
    cout << "\t\t3: A* using Number of Misplaced Tiles .\n";
    cout << "\t\t4: A* using Manhatten Distance.\n";
    cout << "\n\t\t"; //This is a spacer and tab line for the user input so it aligns with the menu
    
    //User enters their selection here.
    cout << "Selection: ";
    cin >> solveMenuSelection;
    
    //Verifies input is an integer. IF NOT, requests new input
    while(!integerVerify(solveMenuSelection))
    {
        cout << "Please Enter a Selection: ";
        cin >> solveMenuSelection;
    }
    
    //Spacer Line
    cout << endl;
    
    //Displaying the user selection to the output file
    outputFile << "User Selection: " << solveMenuSelection << endl << endl;
    
    //Calling the switch for the main menu selection
    solveMenuSwitch(solveMenuSelection, puzzleArray, outputFile);
}

//---------------------------------------------------------------------------------------
//solveMenuSwitch - This function uses a switch case to verify the user selection and
//                  call the appropriate search function.
//---------------------------------------------------------------------------------------
void solveMenuSwitch(int solveMenuSelection, int puzzleArray[], fstream& outputFile)
{
    //Setting up the switch for the user selection
    switch (solveMenuSelection)
    {
        case 1://Breadth First Search
        {
            breadthFirstSearch(puzzleArray, outputFile);
            break;
        }
        case 2://Depth First Search
        {
            depthFirstSearch(puzzleArray, outputFile);
            break;
        }
        case 3://A* using the number of misplaced tiles.
        {
            misplacedTilesSearch(puzzleArray, outputFile);
            break;
        }
        case 4://A* using the “Manhattan Distance”
        {
            manhattenDistanceSearch(puzzleArray, outputFile);
            break;
        }
        default://Invalid selection
        {
            cout << "\t::Invalid Selection::\n";
            howToSolveMenu(puzzleArray, outputFile);
            break;
        }
    }
}

//---------------------------------------------------------------------------------------
//puzzleEntry - This function allows the user the enter a puzzle one number at a time.
//---------------------------------------------------------------------------------------
void puzzleEntry(fstream& outputFile)
{
    //Tells the number verifier what selector this is.
    int option = 0;
    
    //Array of Int numbers for the user to input their selection
    int puzzleArray[PUZZLE_LENGTH];
    
    //Variable to hold the users input
    int userInput;
    
    //Resetting the checkArray to '9' in each slot for use in our numberVerifier
    for(int x = 0; x < PUZZLE_LENGTH; x++)
    {
        checkArray[x] = 9;
    }
    
    //Displaying the way the puzzle looks to the user
    cout << "This is the look of the slider puzzle.\n\n";
    cout << "\t\t_\t_\t_\t_\t_\t_\t_\n";
    cout << "\t\t|\t1\t|\t2\t|\t3\t|\n";
    cout << "\t\t|\t-\t-\t-\t-\t-\t|\n";
    cout << "\t\t|\t4\t|\t5\t|\t6\t|\n";
    cout << "\t\t|\t-\t-\t-\t-\t-\t|\n";
    cout << "\t\t|\t7\t|\t8\t|\t9\t|\n";
    cout << "\t\t-\t-\t-\t-\t-\t-\t-\n\n";
    outputFile << "This is the look of the slider puzzle.\n\n";
    outputFile << "\t\t_\t_\t_\t_\t_\t_\t_\n";
    outputFile << "\t\t|\t1\t|\t2\t|\t3\t|\n";
    outputFile << "\t\t|\t-\t-\t-\t-\t-\t|\n";
    outputFile << "\t\t|\t4\t|\t5\t|\t6\t|\n";
    outputFile << "\t\t|\t-\t-\t-\t-\t-\t|\n";
    outputFile << "\t\t|\t7\t|\t8\t|\t9\t|\n";
    outputFile << "\t\t-\t-\t-\t-\t-\t-\t-\n\n";
    
    //Asking for user input.
    cout << "Please enter a number as requested.\n";
    cout << "Acceptable numbers are 1-8, and 0 for\n";
    cout << "the empty slot in the puzzle.\n\n";
    cout << "OR enter 99 to return to the main menu.\n\n";
    outputFile << "Please enter a number as requested.\n";
    outputFile << "Acceptable numbers are 1-8, and 0 for\n";
    outputFile << "the empty slot in the puzzle.\n\n";
    outputFile << "OR enter 99 to return to the main menu.\n\n";
    
    //For loop to recieve the user selection.
    for (int i = 1; i < 10; i++)
    {
        cout << "Please enter a number for slot " << i << ": ";
        outputFile << "Please enter a number for slot " << i << ": ";
        cin >> userInput;
        outputFile << userInput << endl;
        cout << endl;
        
        //Verifies input is an integer. IF NOT, requests new input
        while(!integerVerify(userInput))
        {
            cout << "Please Enter a Number: ";
            cin >> userInput;
        }
        
        //Checking if the userInput is within scope and not previously used.
        if(puzzleInputVerifier(userInput, option, outputFile))
        {
            puzzleArray[i-1] = userInput;
        }
        
        //If numberVerifier returns false, decrements i so the array slot gets called again.
        else
        {
            i--;
        }
        
        //Clearing the input line
        cin.clear();
        cin.clear();
    }
    
    //Displaying the user input puzzle
    cout << "This is your puzzle to be solved:\n";
    outputFile << "This is your puzzle to be solved:\n";
    puzzlePrinter(puzzleArray, outputFile);
    
    //If the generated puzzle is not solvable, try again
    if(!isSolvable(puzzleArray))
    {
        cout << "The input puzzle is not solvable.\n";
        cout << "Would you like to attempt to solve or input another puzzle.\n\n";
        outputFile << "The input puzzle is not solvable.\n";
        outputFile << "Would you like to attempt to solve or input another puzzle.\n\n";
        
        //Calling the invalid menu
        puzzleInvalidMenu(outputFile, puzzleArray);
    }
    
    //Calling the solving options
    howToSolveMenu(puzzleArray, outputFile);
}

//---------------------------------------------------------------------------------------
//puzzleLoader - This function will load in a user created puzzle from a txt file to be solved.
//---------------------------------------------------------------------------------------
void puzzleLoader(fstream& outputFile)
{
    //String to hold the name of the file to be loaded
    string fileName = " ";
    
    //Puzzle array
    int puzzleArray[PUZZLE_LENGTH];
    
    string exitLoader = "EXIT";
    string exitLoaderLower = "exit";
    
    //This allows the user to select a specific file they would like to open
    //Useful incase they would prefer multiple mazes.
    cout << "Enter the name of the file you want to open.\n";
    cout << "This file must be a .txt format file and must\n";
    cout << "only have one number per line.\n";
    cout << "Or enter 'EXIT' to return to the main menu.\n";
    cout << "Enter Here: ";
    cin >> fileName;                         //recieving file name from user
    cout << endl;
    
    //Checking if "EXIT" was typed to return to the main menu
    if (fileName.find(exitLoader) != string::npos)
    {
        puzzleMainMenu(outputFile);
    }
    
    //Checking if "exit" was typed to return to the main menu
    if (fileName.find(exitLoaderLower) != string::npos)
    {
        puzzleMainMenu(outputFile);
    }
    
    fstream puzzleFile;                      //Creating the input file
    puzzleFile.open(fileName + ".txt");       //Opening the specified file by user + .txt
    
    //If the file did open, reads in the file and stores it into the mazeArray
    if(puzzleFile.is_open())
    {
        cout << "File Opened successfully!\n\n";
        cout << "Reading data from file...\n\n";
        
        //Notifying output file of the file opened.
        outputFile << "Output for puzzle file: " << fileName << ".txt\n";
        
        //Storing the file contents into the puzzleArray
        for(int i = 0; i < PUZZLE_LENGTH; ++i)
        {
            puzzleFile >> puzzleArray[i];
        }
        
        //Printing the file to the user
        puzzlePrinter(puzzleArray, outputFile);
        
        //If the input puzzle is not solvable, notify the user and send back to main menu
        if(!isSolvable(puzzleArray))
        {
            cout << "The input puzzle is not solvable.\n";
            cout << "Re-routing back to main menu.\n\n";
            outputFile << "The input puzzle is not solvable.\n";
            outputFile << "Re-routing back to main menu.\n\n";
            puzzleGenerator(outputFile);
        }
        
        //Closing the input file
        puzzleFile.close();
        
        //Calling the solve menu
        howToSolveMenu(puzzleArray, outputFile);
    }
    else    //File was not found, did not open a file.
    {
        cout << "\t---File name supplied was not found---.\n";
        cout << "\t ---Check your files and try again---.\n";
        cout << endl << endl;
        puzzleMainMenu(outputFile);
    }
    
}

//---------------------------------------------------------------------------------------
//premadePuzzles - This function will print a list of premade puzzles to the user and
//                 allow the user to select one puzzle for solving.
//---------------------------------------------------------------------------------------
void premadePuzzles(fstream& outputFile)
{
    //variable to hold the user selection
    int puzzleSelection = 0;
    
    //Menu with options
    cout << "\tPuzzle:\n";
    cout << "\t\t1: 1 2 3 " << endl;
    cout << "\t\t   4 5 6 " << endl;
    cout << "\t\t   7 8 0 " << endl << endl;
    cout << "\t\t2: 2 4 6 " << endl;
    cout << "\t\t   8 0 1 " << endl;
    cout << "\t\t   3 5 7 " << endl << endl;
    cout << "\t\t3: 0 8 1 " << endl;
    cout << "\t\t   7 2 6 " << endl;
    cout << "\t\t   3 5 4 " << endl << endl;
    cout << "\t\t4: 2 3 4 " << endl;
    cout << "\t\t   8 0 7 " << endl;
    cout << "\t\t   6 5 1 " << endl << endl;
    cout << "\t\t5: RETURN TO MAIN MENU" << endl << endl;
    cout << "\n\t\t"; //This is a spacer and tab line for the user input so it aligns with the menu
    
    //Menu with options
    outputFile << "\tPuzzle:\n";
    outputFile << "\t\t1: 0 2 3 " << endl;
    outputFile << "\t\t   4 5 6 " << endl;
    outputFile << "\t\t   7 8 1 " << endl << endl;
    outputFile << "\t\t2: 2 4 6 " << endl;
    outputFile << "\t\t   8 0 1 " << endl;
    outputFile << "\t\t   3 5 7 " << endl << endl;
    outputFile << "\t\t3: 0 8 1 " << endl;
    outputFile << "\t\t   7 2 6 " << endl;
    outputFile << "\t\t   3 5 4 " << endl << endl;
    outputFile << "\t\t4: 2 3 4 " << endl;
    outputFile << "\t\t   8 0 7 " << endl;
    outputFile << "\t\t   6 5 1 " << endl << endl;
    outputFile << "\t\t5: RETURN TO MAIN MENU" << endl << endl;
    outputFile << "\n\t\t"; //This is a spacer and tab line for the user input so it aligns with the menu
    
    //User enters their selection here.
    cout << "Selection: ";
    cin >> puzzleSelection;
    
    //Verifies input is an integer. IF NOT, requests new input
    while(!integerVerify(puzzleSelection))
    {
        cout << "Please Enter a Selection: ";
        cin >> puzzleSelection;
    }
    
    //Spacer Line
    cout << endl;
    
    //Displaying the user selection to the output file
    outputFile << "User Selection: " << puzzleSelection << endl << endl;
    
    //Calling the switch for the main menu selection
    premadePuzzleSwitch(puzzleSelection, outputFile);
}

//---------------------------------------------------------------------------------------
//solveMenuSwitch - This function uses a switch case to verify the user selection and
//                  call the appropriate search function.
//---------------------------------------------------------------------------------------
void premadePuzzleSwitch(int solveMenuSelection, fstream& outputFile)
{
    int puzzleArray[PUZZLE_LENGTH];
    
    //Setting up the switch for the user selection
    switch (solveMenuSelection)
    {
        case 1:// 1 2 3 4 5 6 7 8 0
        {
            puzzleArray[0] = 1;
            puzzleArray[1] = 2;
            puzzleArray[2] = 3;
            puzzleArray[3] = 4;
            puzzleArray[4] = 5;
            puzzleArray[5] = 6;
            puzzleArray[6] = 7;
            puzzleArray[7] = 8;
            puzzleArray[8] = 0;
            break;
        }
        case 2:// 2 4 6 8 0 1 3 5 7
        {
            puzzleArray[0] = 2;
            puzzleArray[1] = 4;
            puzzleArray[2] = 6;
            puzzleArray[3] = 8;
            puzzleArray[4] = 0;
            puzzleArray[5] = 1;
            puzzleArray[6] = 3;
            puzzleArray[7] = 5;
            puzzleArray[8] = 7;
            break;
        }
        case 3:// 0 8 1 7 2 6 3 5 4
        {
            puzzleArray[0] = 0;
            puzzleArray[1] = 8;
            puzzleArray[2] = 1;
            puzzleArray[3] = 7;
            puzzleArray[4] = 2;
            puzzleArray[5] = 6;
            puzzleArray[6] = 3;
            puzzleArray[7] = 5;
            puzzleArray[8] = 4;
            break;
        }
        case 4:// 2 3 4 8 0 7 6 5 1
        {
            puzzleArray[0] = 2;
            puzzleArray[1] = 3;
            puzzleArray[2] = 4;
            puzzleArray[3] = 8;
            puzzleArray[4] = 0;
            puzzleArray[5] = 7;
            puzzleArray[6] = 6;
            puzzleArray[7] = 5;
            puzzleArray[8] = 1;
            break;
        }
        case 5: //Return to main menu
        {
            puzzleMainMenu(outputFile);
        }
        default://Invalid selection
        {
            cout << "\t::Invalid Selection::\n";
            premadePuzzles(outputFile);
            break;
        }
    }
    
    //Printing the maze to the user
    cout << "This is your selected puzzle:\n";
    outputFile << "This is your selected puzzle:\n";
    puzzlePrinter(puzzleArray, outputFile);
    
    //Calling the solving options
    howToSolveMenu(puzzleArray, outputFile);
}

//---------------------------------------------------------------------------------------
//puzzleGenerator() - This function randomly generates a puzzle for the user to have solved.
//---------------------------------------------------------------------------------------
void puzzleGenerator(fstream& outputFile)
{
    //Setting the option to 1
    int option = 1;
    
    //Variable to hold a random number
    int randomNumber = 0;
    
    //Resetting the checkArray to '9' in each slot for use in our numberVerifier
    for(int x = 0; x < PUZZLE_LENGTH; x++)
    {
        checkArray[x] = 9;
    }
    
    //Array to hold the puzzle
    int puzzleArray[PUZZLE_LENGTH] = {9,9,9,9,9,9,9,9,9};
    
    //Notifiying user of random number being generated.
    cout << "Randomly generating a puzzle...\n";
    outputFile << "Randomly generating a puzzle...\n";
    
    //For loop to fill each slot of the array
    for(int i = 0; i < PUZZLE_LENGTH; i++)
    {
        //Generates random number between 0 and 8
        randomNumber = rand() % (8 - 0) + 0;

        //Checking if the number is between 0-8 and that it hasnt been used previously.
        while(!puzzleInputVerifier(randomNumber, option, outputFile))
        {
            //Generating another random number
            randomNumber = rand() % (9 - 0) + 0;
        }
        
        //Setting the location in the array to the randomNumber
        puzzleArray[i] = randomNumber;
    }
    
    //If the generated puzzle is not solvable, recall the puzzle generator
    //and try again.
    if(!isSolvable(puzzleArray))
    {
        puzzleGenerator(outputFile);
    }
    
    //Printing the generated puzzle
    cout << "This is your generated puzzle:\n";
    puzzlePrinter(puzzleArray, outputFile);
    
    //Sending the generated puzzle to the solve menu
    howToSolveMenu(puzzleArray, outputFile);
}

//---------------------------------------------------------------------------------------
//programCredits - This function holds the credits for the program
//---------------------------------------------------------------------------------------
void programCredits(fstream& outputFile)
{
    //Credits to user display
    cout << "This program was designed and programmed\n";
    cout << "for CSC 412 - Intro to Artificial Intelligence\n";
    cout << "with Bryant Walley\n\n";
    cout << "By: Chip Brommer\n";
    cout << "Contact: fredrick.brommer@usm.edu\n";
    cout << "Date: October 20, 2018\n\n";
    
    //Credits to outputFile
    outputFile << "This program was designed and programmed\n";
    outputFile << "for CSC 412 - Intro to Artificial Intelligence\n";
    outputFile << "with Bryant Walley\n\n";
    outputFile << "By: Chip Brommer\n";
    outputFile << "Contact: fredrick.brommer@usm.edu\n";
    outputFile << "Date: October 20, 2018\n\n";
    
    //Call back to main menu
    puzzleMainMenu(outputFile);
}

//---------------------------------------------------------------------------------------
//exitProgram - This function exits the program
//---------------------------------------------------------------------------------------
void exitProgram(fstream& outputFile)
{
    cout << "\nProgram Close.\n";
    outputFile << "\nProgram Close.\n";
    
    //Closing the output file.
    outputFile.close();
    
    exit(0);        //Same as return 0, but causes immediate exit.
}

//---------------------------------------------------------------------------------------
//puzzleInputVerifier - This is a bool to check if a number entered by the user is used
//---------------------------------------------------------------------------------------
bool puzzleInputVerifier(int userInput, int option, fstream& outputFile)
{
    //Checking if 99 was entered to return to the main menu
    if (userInput == 99)
    {
        puzzleMainMenu(outputFile);
    }
    
    //Making sure the user input is between 0-8
    if (userInput < 0 || userInput > 8)
    {
        cout << "Input is not within the scope of the puzzle.\n";
        return false;
    }
    
    //Checking to see if the number has been entered previously
    for(int i = 0; i < PUZZLE_LENGTH; i++)
    {
        //If the number has been already entered, return false
        if(checkArray[i] == userInput )
        {
            //If the option is 1, it will skip this line.
            //Only the random number generator == 1
            if(option != 1)
            {
                cout << "Number " << userInput << " has already been entered.\n";
            }
            return false;
        }
    }
    
    //Checking each allocation of the array for the userInput amount
    for(int j = 0; j < PUZZLE_LENGTH; j++)
    {
        //If the slot of the array == 9, then it has not been used and will
        //be set to the input number from the user and breaks out of the loop.
        if(checkArray[j] == 9)
        {
            checkArray[j] = userInput;
            return true;
        }
    }
    
    return false;
}

//---------------------------------------------------------------------------------------
//IntegerVerify - This function verifies
//---------------------------------------------------------------------------------------
bool integerVerify(int userInput)
{
    //This while loop ensures the user enters ONLY an integer. Anything else will
    //be kicked out.
    while(1)
    {
        if(cin.fail())
        {
            cin.clear();
            
            //If the input is not within numberic limits, throw error
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            cout << "Please only enter a number.\n";
            return false;
        }
        if(!cin.fail())
            break;
    }
    
    return true;
}

//---------------------------------------------------------------------------------------
//breadthFirstSearch - Breadth First Search algorithim to solve the puzzle
//---------------------------------------------------------------------------------------
void breadthFirstSearch(int puzzleArray[], fstream& outputFile)
{
    //Starting timer
    auto start = chrono::system_clock::now();
    
    //Saving the starting puzzle
    int startingPuzzle[PUZZLE_LENGTH];
    for(int q = 0; q < PUZZLE_LENGTH; q++)
    {
        startingPuzzle[q] = puzzleArray[q];
    }
    
    //Variable to count the total number of nodes created
    int totalNodeCount = 0;
    
    //Creating the stack
    queue<State*> BFSqueue;
    
    //Variable to hold the return value from the function that finds the empty space
    int startingEmptySpaceLocation = findEmptySpace(puzzleArray);
    
    //Creating a newNode with the necessary information
    State* startingState = newState(puzzleArray, startingEmptySpaceLocation, 0, startingEmptySpaceLocation, NULL);
    
    //Pushing the startingState struct onto the queue
    BFSqueue.push(startingState);
    
    //While the stack is not empty, run this
    while(!BFSqueue.empty())
    {
        //Setting the working puzzle to the puzzle that is on the top of the stack
        State* tempPuzzle = BFSqueue.front();
        
        //If the current state we are checking is not equal to the goal state
        //pop off the front cell in the queue and queue its possible moves.
        BFSqueue.pop();
        totalNodeCount++;
        
        //Checking if the current state of the puzzle is the ideal state (SOLVED)
        if(puzzleSolved(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Deleting the queue
            while (!BFSqueue.empty())
            {
                BFSqueue.pop();
                totalNodeCount++;
            }
            
            //Stopping timer
            auto end = chrono::system_clock::now();
            
            //Getting the total elapsed time: End time - Start time = Total time elapsed.
            chrono::duration<double> elapsedTime = (end - start);
            
            //Notifying user of the time it took to complete the sorting.
            cout << "\nBreadth First Search Elapsed time: " << elapsedTime.count() << "s\n\n";
            outputFile << "\nBreadth First Search Elapsed time: " << elapsedTime.count() << "s\n\n";
            
            //Printing the total nodes count.
            cout << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
            outputFile << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
            
            //Printing the move count.
            cout << "\t\tTotal Moves: " << tempPuzzle->moveCount << endl << endl;
            outputFile << "\t\tTotal Moves: " << tempPuzzle->moveCount << endl << endl;
            
            //Printing the final puzzle
            puzzlePrinter(tempPuzzle->currentState, outputFile);
            
            //Asking the user if they want to see each move that was taken
            char decision = NULL;
            cout << "\tWould you like to see all the moves used?\n";
            cout << "\tEnter Y for yes or N for no.\n";
            cout << "\tDecision:";
            cin >> decision;
            cout << endl << endl;
            
            //Verifies input is Y or N. IF NOT, requests new input
            while(decision != 'Y' && decision != 'y' && decision != 'N' && decision != 'n' )
            {
                cout << "\nPlease enter a correct selection.\n";
                cout << "Enter Here:  ";
                cin >> decision;
            }
            
            //If the user enters "y" or "Y" then call the printPath function
            if(decision == 'y' || decision == 'Y')
            {
                outputFile << "Printing each move from start to finish.\n\n";
                printPath(tempPuzzle, tempPuzzle->moveCount, outputFile);
                cout << endl << endl;
            }
            
            //Clearing the list
            puzzleList.clear();
            
            //Asking the user if they want to use the same array for a different search, if they do not
            //this function will send them back to the main menu.
            useSameArray(startingPuzzle, outputFile);
        }
        
        //Checking if the space can move up, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveUp(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation - 3, tempPuzzle);
            
            //Pushing the tempPuzzle onto the stack
            BFSqueue.push(child);
        }
        
        //Checking if the space can move right, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveRight(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation + 1, tempPuzzle);
            
            //Pushing the tempPuzzle onto the stack
            BFSqueue.push(child);
        }
        
        //Checking if the space can move down, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveDown(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation + 3, tempPuzzle);
            
            //Pushing the tempPuzzle onto the stack
            BFSqueue.push(child);
        }
        
        //Checking if the space can move left, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveLeft(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation - 1, tempPuzzle);
            
            //Pushing the tempPuzzle onto the stack
            BFSqueue.push(child);
        }
        
        //Adding the tempPuzzle to the List
        puzzleList.push_front(arrayToIntConverter(tempPuzzle->currentState));
    }
    
    //NO SOLUTION FOUND
    cout << "\nNo Solution Found." << endl;
    
    //Stopping timer
    auto end = chrono::system_clock::now();
    
    //Getting the total elapsed time: End time - Start time = Total time elapsed.
    chrono::duration<double> elapsedTime = (end - start);
    
    //Notifying user of the time it took to complete the sorting.
    cout << "\nBreadth First Search Elapsed time: " << elapsedTime.count() << "s\n\n";
    outputFile << "\nBreadth First Search Elapsed time: " << elapsedTime.count() << "s\n\n";
    
    //Printing the total nodes count.
    cout << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
    outputFile << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
    
    //Clearing the list
    puzzleList.clear();
    
    //Asking the user if they want to use the same array for a different search, if they do not
    //this function will send them back to the main menu.
    useSameArray(startingPuzzle, outputFile);
}

//---------------------------------------------------------------------------------------
//depthFirstSearch - Depth First Search algorithim to solve the puzzle
//---------------------------------------------------------------------------------------
void depthFirstSearch(int puzzleArray[], fstream& outputFile)
{
    //Starting timer
    auto start = chrono::system_clock::now();
    
    //Saving the starting puzzle
    int startingPuzzle[PUZZLE_LENGTH];
    for(int q = 0; q < PUZZLE_LENGTH; q++)
    {
        startingPuzzle[q] = puzzleArray[q];
    }
    
    //Variable to count the total number of nodes created
    int totalNodeCount = 0;
    
    //Creating the stack
    stack<State*> DFSstack;
    
    //Variable to hold the return value from the function that finds the empty space
    int startingEmptySpaceLocation = findEmptySpace(puzzleArray);
    
    //Creating a newNode with the necessary information
    State* startingState = newState(puzzleArray, startingEmptySpaceLocation, 0, startingEmptySpaceLocation, NULL);
    
    //Pushing the startingState struct onto the queue
    DFSstack.push(startingState);
    
    //While the stack is not empty, run this
    while(!DFSstack.empty())
    {
        //Setting the working puzzle to the puzzle that is on the top of the stack
        State* tempPuzzle = DFSstack.top();
        
        //If the current state we are checking is not equal to the goal state
        //pop off the front cell in the queue and queue its possible moves.
        DFSstack.pop();
        totalNodeCount++;
        
        //Checking if the current state of the puzzle is the ideal state (SOLVED)
        if(puzzleSolved(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Deleting the stack
            while (!DFSstack.empty())
            {
                DFSstack.pop();
                totalNodeCount++;
            }
            
            //Stopping timer
            auto end = chrono::system_clock::now();
            
            //Getting the total elapsed time: End time - Start time = Total time elapsed.
            chrono::duration<double> elapsedTime = (end - start);
            
            //Notifying user of the time it took to complete the sorting.
            cout << "\nDepth First Search Elapsed time: " << elapsedTime.count() << "s\n\n";
            outputFile << "\nDepth First Search Elapsed time: " << elapsedTime.count() << "s\n\n";
            
            //Printing the total nodes count.
            cout << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
            outputFile << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
            
            //Printing the move count.
            cout << "\t\tTotal Moves: " << tempPuzzle->moveCount << endl << endl;
            outputFile << "\t\tTotal Moves: " << tempPuzzle->moveCount << endl << endl;
            
            //Printing the final puzzle
            puzzlePrinter(tempPuzzle->currentState, outputFile);
            
            //Asking the user if they want to see each move that was taken
            char decision = NULL;
            cout << "\tWould you like to see all the moves used?\n";
            cout << "\tEnter Y for yes or N for no.\n";
            cout << "\tDecision:";
            cin >> decision;
            cout << endl << endl;
            
            //Verifies input is Y or N. IF NOT, requests new input
            while(decision != 'Y' && decision != 'y' && decision != 'N' && decision != 'n' )
            {
                cout << "\nPlease enter a correct selection.\n";
                cout << "Enter Here:  ";
                cin >> decision;
            }
            
            //If the user enters "y" or "Y" then call the printPath function
            if(decision == 'y' || decision == 'Y')
            {
                outputFile << "Printing each move from start to finish.\n\n";
                printPath(tempPuzzle, tempPuzzle->moveCount, outputFile);
                cout << endl << endl;
            }
            
            //Clearing the list
            puzzleList.clear();
            
            //Asking the user if they want to use the same array for a different search, if they do not
            //this function will send them back to the main menu.
            useSameArray(startingPuzzle, outputFile);
        }
        
        //Checking if the space can move up, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveUp(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation - 3, tempPuzzle);
            
            //Pushing the tempPuzzle onto the stack
            DFSstack.push(child);
        }
        
        //Checking if the space can move right, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveRight(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation + 1, tempPuzzle);
            
            //Pushing the tempPuzzle onto the stack
            DFSstack.push(child);
        }
        
        //Checking if the space can move down, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveDown(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation + 3, tempPuzzle);
            
            //Pushing the tempPuzzle onto the stack
            DFSstack.push(child);
        }
        
        //Checking if the space can move left, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveLeft(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation - 1, tempPuzzle);
            
            //Pushing the tempPuzzle onto the stack
            DFSstack.push(child);
        }
        
        //Adding the tempPuzzle to the List
        puzzleList.push_front(arrayToIntConverter(tempPuzzle->currentState));
    }
    
    //NO SOLUTION FOUND
    cout << "\nNo Solution Found." << endl;
    
    //Stopping timer
    auto end = chrono::system_clock::now();
    
    //Getting the total elapsed time: End time - Start time = Total time elapsed.
    chrono::duration<double> elapsedTime = (end - start);
    
    //Notifying user of the time it took to complete the sorting.
    cout << "\nDepth First Search Elapsed time: " << elapsedTime.count() << "s\n\n";
    outputFile << "\nDepth First Search Elapsed time: " << elapsedTime.count() << "s\n\n";
    
    //Printing the total nodes count.
    cout << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
    outputFile << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
    
    //Clearing the list
    puzzleList.clear();
    
    //Asking the user if they want to use the same array for a different search, if they do not
    //this function will send them back to the main menu.
    useSameArray(startingPuzzle, outputFile);
}

//---------------------------------------------------------------------------------------
//misplacedTilesSearch - A* search algorithm using the misplaced tiles method
//---------------------------------------------------------------------------------------
void misplacedTilesSearch(int puzzleArray[], fstream& outputFile)
{
    
    //Starting timer
    auto start = chrono::system_clock::now();
    
    //Variable to count the total number of nodes created
    int totalNodeCount = 0;
    
    //Saving the starting puzzle
    int startingPuzzle[PUZZLE_LENGTH];
    for(int q = 0; q < PUZZLE_LENGTH; q++)
    {
        startingPuzzle[q] = puzzleArray[q];
    }
    
    //Creating the priority queue
    priority_queue<State*, vector<State*>, comp> pQueue;
    
    //Variable to hold the return value from the function that finds the empty space
    int startingEmptySpaceLocation = findEmptySpace(puzzleArray);
    
    //Creating a newNode with the necessary information
    State* startingState = newState(puzzleArray, startingEmptySpaceLocation, 0, startingEmptySpaceLocation, NULL);
    
    //Setting the number of misplacedTiles in the startingState to the return value
    //of the function that counts the number of misplaced tiles
    startingState->misplacedTiles = countMisplacedTiles(puzzleArray, outputFile);
    
    //Pushing the startingState struct onto the queue
    pQueue.push(startingState);
    
    //Finds a live state with the least number of misplaced tiles,
    //Adds its children to the list of live states to check, then deletes it from the list
    while(!pQueue.empty())
    {
        //Creating a temporary state to hold the state with the least number of misplaced tiles from the
        //top of the queue
        State* leastTiles = pQueue.top();
        
        //Popping the top off the queue and incrementing the totalNodeCount
        pQueue.pop();
        totalNodeCount++;
        
        //If all the tiles are in the correct location, we have reached the solved puzzle
        if (puzzleSolved(arrayToIntConverter(leastTiles->currentState)))
        {
            //Clearing the Priority Queue
            while (!pQueue.empty())
            {
                pQueue.pop();
                totalNodeCount++;
            }
            
            //Stopping timer
            auto end = chrono::system_clock::now();
            
            //Getting the total elapsed time: End time - Start time = Total time elapsed.
            chrono::duration<double> elapsedTime = (end - start);
            
            //Notifying user of the time it took to complete the sorting.
            cout << "\nMisplaced Tiles Search Elapsed time: " << elapsedTime.count() << "s\n\n";
            outputFile << "\nMispalced Tiles Search Elapsed time: " << elapsedTime.count() << "s\n\n";
            
            //Printing the total nodes count.
            cout << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
            outputFile << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
            
            //Printing the move count.
            cout << "\t\tTotal Moves: " << leastTiles->moveCount << endl << endl;
            outputFile << "\t\tTotal Moves: " << leastTiles->moveCount << endl << endl;
            
            //Printing the final puzzle
            puzzlePrinter(leastTiles->currentState, outputFile);
            
            //Asking the user if they want to see each move that was taken
            char decision = NULL;
            cout << "\tWould you like to see all the moves used?\n";
            cout << "\tEnter Y for Yes or N for No.\n";
            cout << "\tDecision:";
            cin >> decision;
            cout << endl << endl;
            
            //Verifies input is Y or N. IF NOT, requests new input
            while(decision != 'Y' && decision != 'y' && decision != 'N' && decision != 'n' )
            {
                cout << "\nPlease enter a correct selection.\n";
                cout << "Enter Here:  ";
                cin >> decision;
            }
            
            //If the user enters "y" or "Y" then call the printPath function
            if(decision == 'y' || decision == 'Y')
            {
                outputFile << "Printing each move from start to finish.\n\n";
                tilesPrintPath(leastTiles, leastTiles->moveCount, outputFile);
                cout << endl << endl;
            }
            
            //Clearing the list
            puzzleList.clear();
            
            //Asking the user if they want to use the same array for a different search, if they do not
            //this function will send them back to the main menu.
            useSameArray(startingPuzzle, outputFile);
        }
        
        //-----------ELSE--------------
        
        //Checking if the space can move up, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveUp(leastTiles->spaceLocation) && !listSearch(arrayToIntConverter(leastTiles->currentState)))
        {
            //Creating the new state
            State* child = newState(leastTiles->currentState, leastTiles->spaceLocation, leastTiles->moveCount + 1, leastTiles->spaceLocation - 3, leastTiles);
            
            //Calculate the number of misplaced tiles for the child and store it in the state
            child->misplacedTiles = countMisplacedTiles(child->currentState, outputFile);
            
            //Pushing the tempPuzzle onto the stack
            pQueue.push(child);
        }
        
        //Checking if the space can move right, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveRight(leastTiles->spaceLocation) && !listSearch(arrayToIntConverter(leastTiles->currentState)))
        {
            //Creating the new state
            State* child = newState(leastTiles->currentState, leastTiles->spaceLocation, leastTiles->moveCount + 1, leastTiles->spaceLocation + 1, leastTiles);
            
            //Calculate the number of misplaced tiles for the child and store it in the state
            child->misplacedTiles = countMisplacedTiles(child->currentState, outputFile);
            
            //Pushing the tempPuzzle onto the stack
            pQueue.push(child);
        }
        
        //Checking if the space can move down, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveDown(leastTiles->spaceLocation) && !listSearch(arrayToIntConverter(leastTiles->currentState)))
        {
            //Creating the new state
            State* child = newState(leastTiles->currentState, leastTiles->spaceLocation, leastTiles->moveCount + 1, leastTiles->spaceLocation + 3, leastTiles);
            
            //Calculate the number of misplaced tiles for the child and store it in the state
            child->misplacedTiles = countMisplacedTiles(child->currentState, outputFile);
            
            //Pushing the tempPuzzle onto the stack
            pQueue.push(child);
        }
        
        //Checking if the space can move left, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveLeft(leastTiles->spaceLocation) && !listSearch(arrayToIntConverter(leastTiles->currentState)))
        {
            //Creating the new state
            State* child = newState(leastTiles->currentState, leastTiles->spaceLocation, leastTiles->moveCount + 1, leastTiles->spaceLocation - 1, leastTiles);
            
            //Calculate the number of misplaced tiles for the child and store it in the state
            child->misplacedTiles = countMisplacedTiles(child->currentState, outputFile);
            
            //Pushing the tempPuzzle onto the stack
            pQueue.push(child);
        }
        
        //Adding the tempPuzzle to the List
        puzzleList.push_front(arrayToIntConverter(leastTiles->currentState));
    }
    //NO SOLUTION FOUND
    cout << "\nNo Solution Found." << endl;
    
    //Stopping timer
    auto end = chrono::system_clock::now();
    
    //Getting the total elapsed time: End time - Start time = Total time elapsed.
    chrono::duration<double> elapsedTime = (end - start);
    
    //Notifying user of the time it took to complete the sorting.
    cout << "\nMisplaced Tiles Search Elapsed time: " << elapsedTime.count() << "s\n\n";
    outputFile << "\nMispalced Tiles Search Elapsed time: " << elapsedTime.count() << "s\n\n";
    
    //Printing the total nodes count.
    cout << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
    outputFile << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
    
    //Clearing the list
    puzzleList.clear();
    
    //Asking the user if they want to use the same array for a different search, if they do not
    //this function will send them back to the main menu.
    useSameArray(startingPuzzle, outputFile);
}

//---------------------------------------------------------------------------------------
//manhattenDistanceSearch - A* search algorithm using the "Manhatten Distance" method
//---------------------------------------------------------------------------------------
void manhattenDistanceSearch(int puzzleArray[], fstream& outputFile)
{
    //Starting timer
    auto start = chrono::system_clock::now();
    
    //Variable to count the total number of nodes created
    int totalNodeCount = 0;
    
    //Saving the starting puzzle
    int startingPuzzle[PUZZLE_LENGTH];
    for(int q = 0; q < PUZZLE_LENGTH; q++)
    {
        startingPuzzle[q] = puzzleArray[q];
    }
    
    //Creating the priority queue
    priority_queue<State*, vector<State*>, mdcomp> mdQueue;
    
    //Variable to hold the return value from the function that finds the empty space
    int startingEmptySpaceLocation = findEmptySpace(puzzleArray);
    
    //Creating a newNode with the necessary information
    State* startingState = newState(puzzleArray, startingEmptySpaceLocation, 0, startingEmptySpaceLocation, NULL);
    
    //Pushing the startingState struct onto the queue
    mdQueue.push(startingState);
    
    //Finds a live state with the least number of misplaced tiles,
    //Adds its children to the list of live states to check, then deletes it from the list
    while(!mdQueue.empty())
    {
        //Creating a temporary state to hold the state with the least number of misplaced tiles from the
        //top of the queue
        State* tempPuzzle = mdQueue.top();
        
        //Popping the top off the queue and incrementing the totalNodeCount
        mdQueue.pop();
        totalNodeCount++;
        
        //If all the tiles are in the correct location, we have reached the solved puzzle
        if (puzzleSolved(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Clearing the Priority Queue
            while (!mdQueue.empty())
            {
                mdQueue.pop();
                totalNodeCount++;
            }
            
            //Stopping timer
            auto end = chrono::system_clock::now();
            
            //Getting the total elapsed time: End time - Start time = Total time elapsed.
            chrono::duration<double> elapsedTime = (end - start);
            
            //Notifying user of the time it took to complete the sorting.
            cout << "\nManhatten Distance Search Elapsed time: " << elapsedTime.count() << "s\n\n";
            outputFile << "\nManhatten Distance Search Elapsed time: " << elapsedTime.count() << "s\n\n";
            
            //Printing the total nodes count.
            cout << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
            outputFile << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
            
            //Printing the move count.
            cout << "\t\tTotal Moves: " << tempPuzzle->moveCount << endl << endl;
            outputFile << "\t\tTotal Moves: " << tempPuzzle->moveCount << endl << endl;
            
            //Printing the final puzzle
            puzzlePrinter(tempPuzzle->currentState, outputFile);
            
            //Asking the user if they want to see each move that was taken
            char decision = NULL;
            cout << "\tWould you like to see all the moves used?\n";
            cout << "\tEnter Y for Yes or N for No.\n";
            cout << "\tDecision:";
            cin >> decision;
            cout << endl << endl;
            
            //Verifies input is Y or N. IF NOT, requests new input
            while(decision != 'Y' && decision != 'y' && decision != 'N' && decision != 'n' )
            {
                cout << "\nPlease enter a correct selection.\n";
                cout << "Enter Here:  ";
                cin >> decision;
            }
            
            //If the user enters "y" or "Y" then call the printPath function
            if(decision == 'y' || decision == 'Y')
            {
                outputFile << "Printing each move from start to finish.\n\n";
                distancePrintPath(tempPuzzle, tempPuzzle->moveCount, outputFile);
                cout << endl << endl;
            }
            
            //Clearing the list
            puzzleList.clear();
            
            //Asking the user if they want to use the same array for a different search, if they do not
            //this function will send them back to the main menu.
            useSameArray(startingPuzzle, outputFile);
        }
        
        //-----------ELSE--------------
        
        //Checking if the space can move up, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveUp(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation - 3, tempPuzzle);
            
            //Calculate the manhatten distance for the child node
            child->manhattanDistance = getManhattanDistance(child->currentState, outputFile) + child->moveCount;
            
            //Pushing the tempPuzzle onto the stack
            mdQueue.push(child);
        }
        
        //Checking if the space can move right, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveRight(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation + 1, tempPuzzle);
            
            //Calculate the manhatten distance for the child node
            child->manhattanDistance = getManhattanDistance(child->currentState, outputFile) + child->moveCount;
            
            //Pushing the tempPuzzle onto the stack
            mdQueue.push(child);
        }
        
        //Checking if the space can move down, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveDown(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation + 3, tempPuzzle);
            
            //Calculate the manhatten distance for the child node
            child->manhattanDistance = getManhattanDistance(child->currentState, outputFile) + child->moveCount;
            
            //Pushing the tempPuzzle onto the stack
            mdQueue.push(child);
        }
        
        //Checking if the space can move left, if yes, swaps locations, increments the counter,
        //and pop the new puzzle onto the stack
        if (moveLeft(tempPuzzle->spaceLocation) && !listSearch(arrayToIntConverter(tempPuzzle->currentState)))
        {
            //Creating the new state
            State* child = newState(tempPuzzle->currentState, tempPuzzle->spaceLocation, tempPuzzle->moveCount + 1, tempPuzzle->spaceLocation - 1, tempPuzzle);
            
            //Calculate the manhatten distance for the child node
            child->manhattanDistance = getManhattanDistance(child->currentState, outputFile) + child->moveCount;
            
            //Pushing the tempPuzzle onto the stack
            mdQueue.push(child);
        }
        
        //Adding the tempPuzzle to the List
        puzzleList.push_front(arrayToIntConverter(tempPuzzle->currentState));
    }
    //NO SOLUTION FOUND
    cout << "\nNo Solution Found." << endl;
    
    //Stopping timer
    auto end = chrono::system_clock::now();
    
    //Getting the total elapsed time: End time - Start time = Total time elapsed.
    chrono::duration<double> elapsedTime = (end - start);
    
    //Notifying user of the time it took to complete the sorting.
    cout << "\nManhattan Distance Search Elapsed time: " << elapsedTime.count() << "s\n\n";
    outputFile << "\nManhattan Distance Search Elapsed time: " << elapsedTime.count() << "s\n\n";
    
    //Printing the total nodes count.
    cout << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
    outputFile << "\t\tTotal Nodes Created: " << totalNodeCount << endl << endl;
    
    //Clearing the list
    puzzleList.clear();
    
    //Asking the user if they want to use the same array for a different search, if they do not
    //this function will send them back to the main menu.
    useSameArray(startingPuzzle, outputFile);
}

//---------------------------------------------------------------------------------------
//puzzlePrinter - Prints a puzzle to the user and the output file
//---------------------------------------------------------------------------------------
void puzzlePrinter(int puzzleArray[], fstream& outputFile)
{
    cout << "\t\t_\t_\t_\t_\t_\t_\t_\n";
    cout << "\t\t|\t" << puzzleArray[0] << "\t|\t" << puzzleArray[1] << "\t|\t" << puzzleArray[2] << "\t|\n";
    cout << "\t\t|\t-\t-\t-\t-\t-\t|\n";
    cout << "\t\t|\t" << puzzleArray[3] << "\t|\t" << puzzleArray[4] << "\t|\t" << puzzleArray[5] << "\t|\n";
    cout << "\t\t|\t-\t-\t-\t-\t-\t|\n";
    cout << "\t\t|\t" << puzzleArray[6] << "\t|\t" << puzzleArray[7] << "\t|\t" << puzzleArray[8] << "\t|\n";
    cout << "\t\t-\t-\t-\t-\t-\t-\t-\n\n";
    
    outputFile << "\t\t_\t_\t_\t_\t_\t_\t_\n";
    outputFile << "\t\t|\t" << puzzleArray[0] << "\t|\t" << puzzleArray[1] << "\t|\t" << puzzleArray[2] << "\t|\n";
    outputFile << "\t\t|\t-\t-\t-\t-\t-\t|\n";
    outputFile << "\t\t|\t" << puzzleArray[3] << "\t|\t" << puzzleArray[4] << "\t|\t" << puzzleArray[5] << "\t|\n";
    outputFile << "\t\t|\t-\t-\t-\t-\t-\t|\n";
    outputFile << "\t\t|\t" << puzzleArray[6] << "\t|\t" << puzzleArray[7] << "\t|\t" << puzzleArray[8] << "\t|\n";
    outputFile << "\t\t-\t-\t-\t-\t-\t-\t-\n\n";
}

//---------------------------------------------------------------------------------------
//puzzleSolved - This bool checks to see if a puzzle is solved.
//---------------------------------------------------------------------------------------
bool puzzleSolved(int puzzleArray)
{
    int goalArray = 123456780;
    
    //if the two arrays are equal, return true
    if (puzzleArray == goalArray)
    {
        return true;
    }
    
    return false;
}

//---------------------------------------------------------------------------------------
//findEmptySpace - Finds the empty space of the puzzle and returns its location
//---------------------------------------------------------------------------------------
int findEmptySpace(int puzzleArray[])
{
    //Variable to hold the spot of the empty location
    int emptyLocation = 0;
    
    //For loop to check each spot of the array for the empty location '0'
    //When it finds the location that == 0, sets it to our variable and then returns the location
    for(int x = 0; x < PUZZLE_LENGTH; x++)
    {
        if(puzzleArray[x] == 0)
        {
            emptyLocation = x;
        }
    }
    
    return emptyLocation;
}

//------------------------------------------------------------------------------------------------
//pressToContinue - Called at the end of each search function. then forwards to the main menu.
//------------------------------------------------------------------------------------------------
void pressToContinue(fstream& outputFile)
{
    cout << "Press any key to continue...\n";
    cin.get();
    cin.get();
    
    //Forwarding to main menu
    puzzleMainMenu(outputFile);
}
//------------------------------------------------------------------------------------------------
//moveUp - If space is not in location 0, 1, or 2; It can move up (returns true) else returns false.
//------------------------------------------------------------------------------------------------
bool moveUp(int spaceLocation)
{
    return spaceLocation > 2;
}

//------------------------------------------------------------------------------------------------
//moveRight - If space is not in location 2, 5, or 8; It can move right (returns true) else returns false.
//------------------------------------------------------------------------------------------------
bool moveRight(int spaceLocation)
{
    return (spaceLocation != 2 && spaceLocation != 5 && spaceLocation != 8);
}

//------------------------------------------------------------------------------------------------
//moveDown - If space is not in location 6, 7, or 8; It can move up (returns true) else returns false.
//------------------------------------------------------------------------------------------------
bool moveDown(int spaceLocation)
{
    return spaceLocation < 6;
}

//------------------------------------------------------------------------------------------------
//moveLeft - If space is not in location 0, 3, or 6; It can move left (returns true) else returns false.
//------------------------------------------------------------------------------------------------
bool moveLeft(int spaceLocation)
{
    return (spaceLocation != 0 && spaceLocation != 3 && spaceLocation != 6);
}

//------------------------------------------------------------------------------------------------
//listSearch - Checks the linked list to see if the puzzle has been seen before
//------------------------------------------------------------------------------------------------
bool listSearch(int puzzleArray)
{
    //Iterating through the list to check if the two arrays are the same.
    for(int a = 0; a < puzzleList.size(); a++)
    {
        list<int> :: iterator it = puzzleList.begin();
        
        advance(it, a);
        
        //if the two arrays are equal, return true
        if (puzzleArray == *it)
        {
            return true;
        }
    }
    
    //If puzzle is not in the linked list, return false
    return false;
}

//----------------------------------------------------------------------
//countMisplacedTiles - This functon returns the number of misplaced tiles from the current state
//----------------------------------------------------------------------
int countMisplacedTiles(int puzzleArray[], fstream& outputFile)
{
    //Variable to holds the number of misplaced tiles
    int numberMisplacedTiles = 0;
    
    //Checks each allocation of the array to determine how many tiles are out of place.
    for(int i = 0; i < PUZZLE_LENGTH; i++)
    {
        //If the current state tile being check is not equal to the solved puzzle tile
        //incrememnt the count
        if(puzzleArray[i] != solvedPuzzle[i])
        {
            numberMisplacedTiles++;
        }
    }
    
    //Return the number of misplaced tiles.
    return numberMisplacedTiles;
}

//----------------------------------------------------------------------
//printPath - Will print each move taken step by step for misplacedTiles using recursion until reaching the solve state
//----------------------------------------------------------------------
void printPath(State* root, int moveCount, fstream& outputFile)
{
    
    //If reached the final state in the list, return
    if (root == NULL)
    {
        return;
    }
    
    //Recursive call the the path printer
    moveCount--;
    printPath(root->parent, moveCount, outputFile);
    
    //Printing the move with the number of which move it is.
    if (moveCount == -1)
    {
        cout << "\tStarting State:\n";
        outputFile << "\tStarting State:\n";
    }
    else
    {
        cout << "\tStep Number: " << moveCount + 1<< endl;
        outputFile << "\tStep Number: " << moveCount + 1<< endl;
    }
    
    //Printing the current state
    puzzlePrinter(root->currentState, outputFile);
}


//----------------------------------------------------------------------
//printPath - Will print each move taken step by step for misplacedTiles using recursion until reaching the solve state
//----------------------------------------------------------------------
void tilesPrintPath(State* root, int moveCount, fstream& outputFile)
{
    
    //If reached the final state in the list, return
    if (root == NULL)
    {
        return;
    }
    
    //Recursive call the the path printer
    moveCount--;
    tilesPrintPath(root->parent, moveCount, outputFile);
    
    //Printing the move with the number of which move it is.
    if (moveCount == -1)
    {
        cout << "\tStarting State:\n";
        outputFile << "\tStarting State:\n";
        cout << "\tNumber of Misplace Tiles:" << root->misplacedTiles << endl;
        outputFile << "\tNumber of Misplace Tiles:" << root->misplacedTiles << endl;
    }
    else
    {
        cout << "\tStep Number: " << moveCount + 1<< endl;
        outputFile << "\tStep Number: " << moveCount + 1<< endl;
        cout << "\tNumber of Misplace Tiles:" << root->misplacedTiles << endl;
        outputFile << "\tNumber of Misplace Tiles:" << root->misplacedTiles << endl;
    }
    
    //Printing the current state
    puzzlePrinter(root->currentState, outputFile);
}

//----------------------------------------------------------------------
//distancePrintPath - Will print each move taken step by step for manhattanDistance using recursion until reaching the solve state
//----------------------------------------------------------------------
void distancePrintPath(State* root, int moveCount, fstream& outputFile)
{
    
    //If reached the final state in the list, return
    if (root == NULL)
    {
        return;
    }
    
    //Recursive call the the path printer
    moveCount--;
    distancePrintPath(root->parent, moveCount, outputFile);
    
    //Printing the move with the number of which move it is.
    if (moveCount == -1)
    {
        cout << "\tStarting State:\n";
        outputFile << "\tStarting State:\n";
        cout << "\tManhattan Distance:" << root->manhattanDistance << endl;
        outputFile << "\tManhattan Distance:" << root->manhattanDistance << endl;
    }
    else
    {
        cout << "\tStep Number: " << moveCount + 1<< endl;
        outputFile << "\tStep Number: " << moveCount + 1<< endl;
        cout << "\tManhattan Distance:" << root->manhattanDistance << endl;
        outputFile << "\tManhattan Distance:" << root->manhattanDistance << endl;
    }
    
    //Printing the current state
    puzzlePrinter(root->currentState, outputFile);
}

//----------------------------------------------------------------------
//getInvCount - Function to count inversions in given array 'arr[]'
//----------------------------------------------------------------------
int getInvCount(int puzzleArray[])
{
    //Holds the number of inversions
    int inv_count = 0;
    
    
    for (int i = 0; i < PUZZLE_LENGTH - 1; i++)
    {
        for (int j = i + 1; j < PUZZLE_LENGTH; j++)
        {
            // Value 0 is used for empty space
            if (puzzleArray[j] && puzzleArray[i] &&  puzzleArray[i] > puzzleArray[j])
            {
                //Incrementing the inversion count
                inv_count++;
            }
        }
    }
    
    //Returns the number of inversions
    return inv_count;
}

//----------------------------------------------------------------------
//isSolvable - This function returns true if given puzzle is solvable.
//----------------------------------------------------------------------
bool isSolvable(int puzzleArray[])
{
    // Count inversions in given 8 puzzle
    int invCount = getInvCount((int *)puzzleArray);
    
    // return true if inversion count is even.
    return (invCount % 2 == 0);
}

//----------------------------------------------------------------------
//useSameArray - This function asks the user if they would like to use the same starting array for a new search algorithm
//----------------------------------------------------------------------
void useSameArray(int puzzleArray[], fstream& outputFile)
{
    char useAgain = 0;
    
    cout << "Would you like to use the same starting puzzle\n";
    cout << "in a different search?\n";
    cout << "Please enter 'Y' for YES or 'N' for NO.\n";
    cout << "Enter Here: ";
    cin >> useAgain;
    
    //Verifies input is Y or N. IF NOT, requests new input
    while(useAgain != 'Y' && useAgain != 'y' && useAgain != 'N' && useAgain != 'n' )
    {
        cout << "\nPlease enter a correct selection.\n";
        cout << "Enter Here:  ";
        cin >> useAgain;
    }
    
    //If the user input Y for yes, call the solvint options menu.
    if (useAgain == 'y' || useAgain == 'Y')
    {
        cout << endl << endl;
        howToSolveMenu(puzzleArray, outputFile);
    }
    else //Else send to main menu
    {
        cout << endl << endl;
        pressToContinue(outputFile);
    }
}

//----------------------------------------------------------------------
//getManhattenDistance - This function returns the absolute value of how many moves the puzzle is out of place
//----------------------------------------------------------------------
int getManhattanDistance(int puzzleArray[], fstream& outputFile)
{
    // Function variables
    int manhattanDistance = 0;
    
    //Holds the row and column of the searched value in the for loop.
    int puzzleRow = 0;
    int puzzleColumn = 0;
    
    //Holds the row and column of the searched value in the solvedPuzzle
    int solvedRow = 0;
    int solvedColumn = 0;
    
    // For loop used to calculate the Manhattan distance
    for (int x = 0; x < PUZZLE_LENGTH; x++)
    {
        //Setting the location of the searched array row and column
        getRowAndColumn(puzzleArray, x, puzzleRow, puzzleColumn);
        
        //Setting the location of the solvedState row and column
        getRowAndColumn(solvedPuzzle, x, solvedRow, solvedColumn);
        
        //Calculating the distance
        manhattanDistance = manhattanDistance + abs((puzzleRow - solvedRow)) + abs((puzzleColumn - solvedColumn));
        
    }
    
    //Returns the manhattenDistance that was calculated
    return manhattanDistance;
}

//----------------------------------------------------------------------
//getRowandColumn - This function takes the location of a value and returns a row and column.
//----------------------------------------------------------------------
void getRowAndColumn(int puzzleArray[], int location, int& puzzleRow, int& puzzleColumn)
{
    // For loop used to get the index of the array and return the location of the column and row.
    for (int x = 0; x < PUZZLE_LENGTH; x++)
    {
        //If statement to set the value for each row and column once the for loop locatates the correct value
        if (puzzleArray[x] == solvedPuzzle[location])
        {
            switch (x)
            {
                case 0: // Row 0, Column 0
                {
                    puzzleRow = 0;
                    puzzleColumn = 0;
                    break;
                }
                    
                case 1: // Row 0, Column 1
                {
                    puzzleRow = 0;
                    puzzleColumn = 1;
                    break;
                }
                    
                case 2: // Row 0, Column 2
                {
                    puzzleRow = 0;
                    puzzleColumn = 2;
                    break;
                }
                    
                case 3: // Row 1, Column 0
                {
                    puzzleRow = 1;
                    puzzleColumn = 0;
                    break;
                }
                    
                case 4: // Row 1, Column 1
                {
                    puzzleRow = 1;
                    puzzleColumn = 1;
                    break;
                }
                    
                case 5: // Row 1, Column 2
                {
                    puzzleRow = 1;
                    puzzleColumn = 2;
                    
                    break;
                }
                    
                case 6: // Row 2, Column 0
                {
                    puzzleRow = 2;
                    puzzleColumn = 0;
                    break;
                }
                    
                case 7: // Row 2, Column 1
                {
                    puzzleRow = 2;
                    puzzleColumn = 1;
                    break;
                }
                    
                case 8: // Row 2, Column 2
                {
                    puzzleRow = 2;
                    puzzleColumn = 2;
                    break;
                }
                    
                default:
                {
                    break;
                }
            }
        }
    }
}

//----------------------------------------------------------------------
//arrayToIntConverter - Will take a int array and convert it into a single int number.
//                      that will be used with the c++ list.
//----------------------------------------------------------------------
int arrayToIntConverter(int puzzleArray[])
{
    int arrayInt = 0;

    for (int i = 0; i < PUZZLE_LENGTH; i++)
    {
        arrayInt *= 10;
        arrayInt += puzzleArray[i];
    }
    
    return arrayInt;
}

//----------------------------------------------------------------------
//puzzleInvalidMenu - This menu is called if the input puzzle cannot be solved
//----------------------------------------------------------------------
void puzzleInvalidMenu(fstream& outputFile, int puzzleArray[])
{
    int userSelection = 0;
    
    cout << "Please make a selection:" << endl;
    cout << "\t1: Attempt puzzle." << endl;
    cout << "\t2: Input a new puzzle." << endl;
    cout << "\t3: Exit to main menu." << endl << endl;
    
    cin >> userSelection;
    
    //Verifies input is an integer. IF NOT, requests new input
    while(!integerVerify(userSelection))
    {
        cout << "Please Enter a Selection: ";
        cin >> userSelection;
    }
    
    //Spacer Line
    cout << endl;
    
    //calling the switch
    puzzleInvalidMenuSwitch(userSelection, puzzleArray, outputFile);
}

//----------------------------------------------------------------------
//puzzleInvalidMenuSwitch - This is the switch for the invalid menu
//----------------------------------------------------------------------
void puzzleInvalidMenuSwitch(int userSelection, int puzzleArray[], fstream& outputFile)
{
    //Setting up the switch for the user selection
    switch (userSelection)
    {
        case 1://Breadth First Search
        {
            howToSolveMenu(puzzleArray, outputFile);
            break;
        }
        case 2://Depth First Search
        {
            puzzleEntry(outputFile);
            break;
        }
        case 3://A* using the number of misplaced tiles.
        {
            puzzleMainMenu(outputFile);
            break;
        }
        default://Invalid selection
        {
            cout << "\t::Invalid Selection::\n";
            puzzleInvalidMenu(outputFile, puzzleArray);
            break;
        }
    }
}
