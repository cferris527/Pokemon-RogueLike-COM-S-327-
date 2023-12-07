#include "game.h"

game::game() {}

/**
 * Generates starting map and intializes all maps to not being generated.
*/
void game::start() {

    srand(time(NULL));  //RNG init
    initscr(); // Initialize ncurses
    cbreak();  // Disable line buffering, making characters available immediately
    noecho();  // Don't display the character on the screen

    for (int i = 0; i < 401; ++i) {
        for (int j = 0; j < 401; ++j) {
            allMaps[i][j] = map();
        }
    }

    currentMap = coords(200,200); //current map set to center (201,201)
    allMaps[currentMap.getRow()][currentMap.getCol()]
     = map(currentMap.getRow(), currentMap.getCol(),-1,-1,-1,-1,-1,-1);

    /* Colors for Curses */
    start_color();
    init_color(1, 178, 255, 178); // Lime
    init_pair(1, 1, COLOR_BLACK); 
    init_color(2, 42, 184, 30); // Dark Green
    init_pair(2, 2, COLOR_BLACK);
    init_color(3, 204, 204, 204); // Gray
    init_pair(3, 3, COLOR_BLACK);
    init_color(4, 255, 0, 0);
    init_pair(4, 4, COLOR_BLACK); // Red
    init_color(5, 255, 0, 255);
    init_pair(5, 5, COLOR_BLACK); // Pink
    init_color(6, 0, 255, 255);
    init_pair(6, 6, COLOR_BLACK); // Cyan
    init_color(8, 255, 236, 0);
    init_pair(8, 8, COLOR_BLACK); // Yellow
    init_color(9, 255, 153, 0);
    init_pair(9, 9, COLOR_BLACK); // Orange
    init_color(10, 254, 159, 188);
    init_pair(10, 10, COLOR_BLACK); // Rose
    init_color(11, 142, 98, 39);
    init_pair(11, 11, COLOR_BLACK); // Brown

    chooseStarterPokemon();
}

void game::end()
{
    erase();
    printw("And that's why you'll never be the very best. Git gud");
    printw("\n\n\n\n");
    printw("Press any key to exit");
    getch();
    endwin();
}

void game::chooseStarterPokemon()
{
    allMaps[currentMap.getRow()][currentMap.getCol()].printPokemonSelection();
}

/**
 * Prints map that player is currently on
*/
void game::printCurrentMap() {
    allMaps[currentMap.getRow()][currentMap.getCol()].printMap();
}

/**
 * Prints distance maps (testing purposes only)
*/
void game::printCurrentDistanceMaps() {
    allMaps[currentMap.getRow()][currentMap.getCol()].printDistanceMaps();
}

/**
 * Performs a player turn on the current map being displayed.
*/
void game::doTurn(char c) {
    allMaps[currentMap.getRow()][currentMap.getCol()].doATurn(c);
}

void game::getFly() {
    erase();
    printw("To fly:\n");
    printw("1. Type your x coordinate, press ENTER\n");
    printw("2. Type your y coordinate, press ENTER\n");
    printw("Type \"<\" and press ENTER to QUIT flying\n");

    noraw();
    echo();

    //Get first #
    int ch_x;
    int ch_y;
    std::string inputStringX;
    std::string inputStringY;
    int xCoord = 0;
    int yCoord = 0;

    while (1) {
        ch_x = getch();

        if (ch_x == '\n') {
            break;
        }
        else if (isdigit(ch_x)) {
            inputStringX.push_back(ch_x);
            addch(ch_x);
            //refresh();
        }
    }
    while (1) {
        ch_y = getch();

        if (ch_y == '\n') {
            break;
        }
        else if (isdigit(ch_y)) {
            inputStringY.push_back(ch_y);
            addch(ch_y);
            //refresh();
        }
    }
    xCoord = std::stoi(inputStringX);
    yCoord = std::stoi(inputStringY);

    noecho();
    printw("x: %d | y: %d", xCoord, yCoord);
    refresh();
    getch();
    erase();
    fly(xCoord, yCoord);



    //reset stuff to normal
    raw();
}

void game::fly(int row, int col) {

}

/**
 * Returns position of gate of adjacent maps (if they exist, -1 if not)
*/
int game::checkForAdjacentGate(char dir) {

    int tempRow = currentMap.getRow();
    int tempCol = currentMap.getCol();

    if (dir == 'n') {
        tempRow--;
        if (allMaps[tempRow][tempCol].isGenerated()) {
            return allMaps[tempRow][tempCol].row_south_gate;
        }
    }
    else if (dir == 'e') {
        tempCol++;
        if (allMaps[tempRow][tempCol].isGenerated()) {
            return allMaps[tempRow][tempCol].col_west_gate;
        }
    }
    else if (dir == 's') {
        tempRow++;
        if (allMaps[tempRow][tempCol].isGenerated()) {
            return allMaps[tempRow][tempCol].row_north_gate;
        }
    }
    else if (dir == 'w') {
        tempCol--;
        if (allMaps[tempRow][tempCol].isGenerated()) {
            return allMaps[tempRow][tempCol].col_east_gate;
        }
    }
    return -1;
}

void game::checkIfOnGate() {
    
    // if player character is on '!' gate
    if(allMaps[currentMap.getRow()][currentMap.getCol()].pcOnGate()) {
        
        // PC coordinates
        int tempRow = allMaps[currentMap.getRow()][currentMap.getCol()].getPcRow();
        int tempCol = allMaps[currentMap.getRow()][currentMap.getCol()].getPcCol();

        // direction to move
        int dir = allMaps[currentMap.getRow()][currentMap.getCol()].gatePcIsOn();

        //Current map coordinates (ex: 200,200 on first map)
        int mapRow = currentMap.getRow();
        int mapCol = currentMap.getCol();


        int north, east, south, west;
        // N, E, S, W
        if (dir == 1) {
            tempRow = 19;
            mapRow--;
            currentMap = coords(currentMap.getRow() - 1, currentMap.getCol());
        }
        else if (dir == 2) {
            tempCol = 1;
            mapCol++;
            currentMap = coords(currentMap.getRow(), currentMap.getCol() + 1);
        }
        else if (dir == 3) {
            tempRow = 1;
            mapRow++;
            currentMap = coords(currentMap.getRow() + 1, currentMap.getCol());
        }
        else if (dir == 4) {
            tempCol = 78;
            mapCol--;
            currentMap = coords(currentMap.getRow(), currentMap.getCol() - 1);
        }


        //if new current map map not generated, generate it
        if (!allMaps[currentMap.getRow()][currentMap.getCol()].generated) {
            north = checkForAdjacentGate('n');
            east = checkForAdjacentGate('e');
            south = checkForAdjacentGate('s');
            west = checkForAdjacentGate('w');


            allMaps[currentMap.getRow()][currentMap.getCol()] = 
            map(currentMap.getRow(), currentMap.getCol(), north, east, south, west, tempRow, tempCol);
        }
        //printw("%d, %d\n", tempRow, tempCol);
        //getch();
        //allMaps[currentMap.getRow()][currentMap.getCol()].setPcRow(tempRow);
        //allMaps[currentMap.getRow()][currentMap.getCol()].setPcCol(tempCol);
        printCurrentMap();
        //refresh();

    }
    return;
}