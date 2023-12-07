#ifndef PLAYER_CHARACTER_H
#define PLAYER_CHARACTER_H

#include "character.h"

/**
 * Class that represents character that can be controlled.
*/
class playerCharacter : public character {
protected:
    
public:
    playerCharacter() {}
    playerCharacter(int row, int col) : character(row,col) {}

};

#endif