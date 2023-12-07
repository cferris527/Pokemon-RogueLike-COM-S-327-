#ifndef NON_PLAYER_CHARACTER_H
#define NON_PLAYER_CHARACTER_H

#include "character.h"

/**
 * Class that represents computer-controlled characters.
*/
class nonPlayerCharacter : public character {
protected:
    char type;
    char direction;
public:
    pokemon npcPokemon[6];
    int is_defeated;

    /* Initialize */
    nonPlayerCharacter() {};
    nonPlayerCharacter(int row, int col, char t, char d) : character(row,col)
    {
        type = t;
        direction = d;
        initializePokemon();
        is_defeated = 0;
    }

    void initializePokemon()
    {
        for (int i = 0; i < 6; ++i)
        {
            npcPokemon[i].id = -1; // Pokemon with id = -1 means not a pokemon in slot
            npcPokemon[i].is_dead = 1;
        }
    }

    /* Pokemon */
    void setPokemon(pokemon p, int index) {npcPokemon[index] = p;}
    int allPokemonDead()
    {
        for (int i = 0; i < 6; ++i)
        {
            if (!npcPokemon[i].is_dead) {return 0;}
        }
        return 1;
    }
    pokemon* getFirstPokemon() 
    {
        for (int i = 0; i < 6; ++i) 
        {
            if (!npcPokemon[i].is_dead) {return &npcPokemon[i];}
        }
        return &npcPokemon[0];
    }

    /* Movement */
    char getType() {return type;}
    char getDirection() {return direction;}
    void setType(char t) {type = t;}
    void setDirection(char d) {direction = d;}

};

#endif