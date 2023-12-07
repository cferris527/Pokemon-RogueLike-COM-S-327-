#ifndef CHARACTER_H
#define CHARACTER_H

#include "coords.h"
#include "pokedex.h"

/**
 * Class used to represent a character.
 * Player Character and Non Player Character classes extend from this one.
*/
class character {
protected:
    coords pos;
public:
    character() {};

    character(int row, int col)
    {
        pos = coords(row, col);
    }

    int getRow() {return pos.getRow();}
    int getCol() {return pos.getCol();}
    void setRow(int v) {pos.setRow(v);}
    void setCol(int v) {pos.setCol(v);}
};

#endif