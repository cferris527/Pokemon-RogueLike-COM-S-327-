#ifndef GAME_H
#define GAME_H

#include "map.h"

/**
 * Class that represents the game that is being played. Contains all maps
 * that are accessable to the player.
*/
class game {
public:
    map allMaps[401][401];
    coords currentMap;

    game();
    void start();
    void end();

    void chooseStarterPokemon();

    void printCurrentMap();
    void printCurrentDistanceMaps();

    void doTurn(char c);

    void getFly();
    void fly(int row, int col);

    int checkForAdjacentGate(char dir);

    void checkIfOnGate();


};

#endif