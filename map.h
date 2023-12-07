#ifndef MAP_H
#define MAP_H

#include <ncurses.h>
#include <cstdlib>
#include <random>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <thread>

#include "nonPlayerCharacter.h"
#include "playerCharacter.h"
#include "priorityQueue.h"

/**
 * Class that represents a "map", which is all the terrain/everything that is displayed
 * at a particular coordinate (e.g. game starts at map (0,0))
*/
class map {
private:
    priorityQueue Q;
    int playerThruGate;
    coords mapCoords;
    char terrain[21][80];
    coords paths[100];
    float dist_hiker[21][80];
    float dist_rival[21][80];
    nonPlayerCharacter npcs[9];
    pokemon* npc_pokes[9];
    playerCharacter pc;

    void createRegions();
    void makeBlot(int col, int row, char type, int size_col, int size_row);
    void patchEmptyCells();
    void createBorders();
    void createPath();
    void createPathVertical(int northRow, int southRow);
    void generateCenterMart(int paths[156]);
    void tallyPaths();

    void isThereAPokemon();
    int isTrainerAdjacent(char dir);

    /* Pokemon characters related */
    pokemon generateRandomPokemon();
    pokemon generateWailord();
    pokemon generateKraken();
    pokemon generateZekrom();
    void giveIVs(pokemon *p);
    void giveOthers(pokemon *p);
    void giveMoves(pokemon *p);

    void initializeNpcPokes();
    int isWildEncounter();
    int calcManhattan();

    int isOriginMap;
    int hasSelectedPokemon;

    //Battle/Encounter
    void doWildEncounter();
    void doTrainerBattle(int index);

    void printWildUi(pokemon wild, pokemon mine);
    int calcDamage(int att_level, int def_level, int power, int attack, int defense,
    int move_type, int att_poke_type_one, int att_poke_type_two,
    int def_poke_type_one, int def_poke_type_two);
    double calcCritical(int speed);
    double calcEffectiveness();
    int calcAccuracy();

    int whosFirst(int moveZero_priority, int moveOne_priority,
    int pokeZero_speed, int pokeOne_speed);
    void doAnAttack(pokemon* attacking, pokemon* defending, int flag, int move_index);
    void gameOver();
    pokemon* switchPokemon(); //TODO


    // Bag functions
    int viewBag();
    void viewTms();
    void useTm(int index);
    void learnNewMove(int tm_index, int poke_index);

    void listPokemon();
    void usePotion();
    void useRevive();
    void useFish(int type);


public:

    int row_north_gate;
    int col_west_gate;
    int row_south_gate;
    int col_east_gate;
    int generated;

    // general map terrain
    map();
    map(int mapRow, int mapCol, int north, int east, int south, int west, int pcRow, int pcCol);
    void initializeTerrain();
    int isGenerated();
    void generate(int row, int col);
    void printMap();
    void printRandomPokemon();

    // distance map
    void initializeDistanceMaps();
    float addDistance(char npc, char terrain);
    void generateDistanceMaps();
    void printDistanceMaps();

    // Enter activity
    void enterPokemart();
    void enterPokecenter();
    void enterBuilding();
    void viewPokemon();

    // NPC
    int isSpaceTaken(int row, int col);
    void initializeNpcs();
    void placeNpc(int index);
    void spawnNpcs();
    void giveNewDirection(int index);
    double moveNpc(int ind);

    // PC
    int pcAllowedSpace(int row, int col);
    void spawnPc(int row, int col);
    void movePc(char dir);
    int pcOnGate();
    int gatePcIsOn();
    void printPokemonSelection();

    // Fishing
    int canFishHere();
    void cantFishHere();

    void printFishingUi();

    void doFishing();
    void promptCatch();
    void catchSomething();

    void doRobberFish();
    void doPipeBomb();
    void doSeaGem();
    void giveTm();

    void waterBattle(int boss);


    // Game Queue
    void initializeQueue();
    void doATurn(char c);
    void doTheInput(char ch);

    int getPcRow();
    int getPcCol();
    void setPcRow(int v);
    void setPcCol(int v);

    // Secret hack (to present 1.10 to class)
    void doHack();

};

#endif