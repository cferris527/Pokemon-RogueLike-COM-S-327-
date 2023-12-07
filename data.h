#ifndef DATA_H
#define DATA_H

#include "database.h"


using namespace std;

/*Singleton for Pokemon*/
class pokemonData {
private:
    pokemon* allPokemon;
    static pokemonData *instance;

    pokemonData() {allPokemon = loadPokemon(0);}
    ~pokemonData() {}

public:
    static pokemonData *getInstance() 
    {
        if (!instance) {
            instance = new pokemonData();
        }
        return instance;
    }
    pokemon getPokemon(int i) 
    {
        return allPokemon[i];
    }
};
pokemonData *pokemonData::instance;

/*Singleton for Moves (attack_move)*/
class moveData {
private:
    attack_move *allMoves;
    static moveData *instance;

    moveData() {allMoves = loadMoves(0);}
    ~moveData() {}

public:
    static moveData *getInstance() 
    {
        if (!instance) {
            instance = new moveData();
        }
        return instance;
    }
    attack_move getMove(int i)
    {
        if (i > 844 || i < 0) {
            return allMoves[0];
        }
        return allMoves[i];
    }
};
moveData *moveData::instance;

/*Singleton for Pokemon Moves (pokemon_move)*/
class pokemonMoveData {
private:
    pokemon_move* allPokemonMoves;
    static pokemonMoveData *instance;

    pokemonMoveData() {allPokemonMoves = loadPokemonMoves(0);}
    ~pokemonMoveData() {}

public:
    static pokemonMoveData *getInstance() 
    {
        if (!instance) {
            instance = new pokemonMoveData();
        }
        return instance;
    }
    pokemon_move getPokemonMove(int i)
    {
        if (i > 528238 || i < 0) {
            return allPokemonMoves[0];
        }
        return allPokemonMoves[i];
    }
};
pokemonMoveData *pokemonMoveData::instance;

/*Singleton for Pokemon Species (pokemon_species)*/
class pokemonSpeciesData {
private:
    pokemon_species* allPokemonSpecies;
    static pokemonSpeciesData *instance;

    pokemonSpeciesData() {allPokemonSpecies = loadPokemonSpecies(0);}
    ~pokemonSpeciesData() {}

public:
    static pokemonSpeciesData *getInstance() 
    {
        if (!instance) {
            instance = new pokemonSpeciesData();
        }
        return instance;
    }
};
pokemonSpeciesData *pokemonSpeciesData::instance;

/*Singleton for Experience (experience)*/
class experienceData {
private:
    experience* allExperience;
    static experienceData *instance;

    experienceData() {allExperience = loadExperience(0);}
    ~experienceData() {}

public:
    static experienceData *getInstance() 
    {
        if (!instance) {
            instance = new experienceData();
        }
        return instance;
    }
};
experienceData *experienceData::instance;

/*Singleton for Type Names (type_name)*/
class typeNameData {
private:
    type_name* allTypeNames;
    static typeNameData *instance;

    typeNameData() {allTypeNames = loadtypeName(0);}
    ~typeNameData() {}

public:
    static typeNameData *getInstance() 
    {
        if (!instance) {
            instance = new typeNameData();
        }
        return instance;
    }
};
typeNameData *typeNameData::instance;

/*Singleton for Pokemon Stats (pokemon_stat)*/
class pokemonStatData {
private:
    pokemon_stat* allPokemonStats;
    static pokemonStatData *instance;

    pokemonStatData() {allPokemonStats = loadPokemonStats(0);}
    ~pokemonStatData() {}

public:
    static pokemonStatData *getInstance() 
    {
        if (!instance) {
            instance = new pokemonStatData();
        }
        return instance;
    }
    pokemon_stat getPokemonStat(int i)
    {
        return allPokemonStats[i];
    }
};
pokemonStatData *pokemonStatData::instance;

/*Singleton for Stats (stat)*/
class statData {
private:
    stat* allStats;
    static statData *instance;

    statData() {allStats = loadStats(0);}
    ~statData() {}

public:
    static statData *getInstance() 
    {
        if (!instance) {
            instance = new statData();
        }
        return instance;
    }
};
statData *statData::instance;

/*Singleton for Pokemon Types (pokemon_type)*/
class pokemonTypeData {
private:
    pokemon_type* allPokemonTypes;
    static pokemonTypeData *instance;

    pokemonTypeData() {allPokemonTypes = loadPokemonTypes(0);}
    ~pokemonTypeData() {}

public:
    static pokemonTypeData *getInstance() 
    {
        if (!instance) {
            instance = new pokemonTypeData();
        }
        return instance;
    }
    pokemon_type getPokemonType(int i)
    {
        return allPokemonTypes[i];
    }
};
pokemonTypeData *pokemonTypeData::instance;

class typeEfficacyData {
private:
    type_efficacy* allTypeEfficacies;
    static typeEfficacyData *instance;
    int length = 325;

    typeEfficacyData() {allTypeEfficacies = loadTypeEfficacies(0);}
    ~typeEfficacyData() {}

public:
    static typeEfficacyData *getInstance()
    {
        if (!instance) {
            instance = new typeEfficacyData();
        }
        return instance;
    }
    type_efficacy getTypeEfficacy(int i) {
        return allTypeEfficacies[i];
    }
    int getLength() {return length;}

};
typeEfficacyData *typeEfficacyData::instance;

class pcData {
private:
    static pcData *instance;
    pcData()
    {
        for (int i = 0; i < 6; ++i)
        {
            pcPokemon[i].id = -1; // Pokemon with id = -1 means not a pokemon in slot
            pcPokemon[i].is_dead = 1;

            tm[i].id = -1;
        }
        potions = 3;
        revives = 3;
        poke_balls = 3;
        small_fry = 0;
        flopper = 0;
        shield_fish = 0;

        canWalkOnWater = 0;
        npcsCanMove = 1;
    }
    ~pcData();

public:
    attack_move tm[6];
    pokemon pcPokemon[6];
    int potions;
    int revives;
    int poke_balls;
    int small_fry;
    int flopper;
    int shield_fish;

    int canWalkOnWater;
    int npcsCanMove;


    static pcData *getInstance()
    {
        if (!instance) {
            instance = new pcData();
        }
        return instance;
    }

    /* Returns 1 if pokemon was added to party.
     * Returns 0 if party is full (PC has 6 pokemon) */
    int addPokemon(pokemon p)
    {
        for (int i = 0; i < 6; ++i)
        {
            if (pcPokemon[i].id == -1) {
                pcPokemon[i] = p;
                return 1;
            }
        }
        return 0;
    }
    
    /**
     * Returns first availible pokemon that isn't knocked out (0 hp_cur)
    */
    pokemon* getFirstPokemon()
    {
        for (int i = 0; i < 6; ++i)
        {
            if (pcPokemon[i].hp_cur > 0 && pcPokemon[i].id > 0)
            {
                return &pcPokemon[i];
            }
        }
        // If no pokemon are availible, return empty pokemon with id of -1
        pokemon* p = new pokemon();
        p->id = -1;
        return p;
    }

    /**
     * Returns 1 if all PC's pokemon are dead, 0 if at least 1 is alive
    */
    int allPokemonKnockedOut()
    {
        for (int i = 0; i < 6; ++i)
        {
            if (!pcPokemon[i].is_dead) {return 0;}
        }
        return 1;
    }

    /**
     * Returns 1 if PC has less than 6 pokemon
    */
    int hasEmptySlot()
    {
        for (int i = 0; i < 6; ++i)
        {
            if(pcPokemon[i].id < 0) {return 1;}
        }
        return 0;
    }

    /**
     * Adds pokemon to party at first availible slot
    */
    void addPokemonToParty(pokemon p) 
    {
        for (int i = 0; i < 6; ++i)
        {
            if(pcPokemon[i].id < 0) {
                pcPokemon[i] = p;
                return;
            }
        }
    }

    /**
     * Max heals and revives all pokemon in party (for Pokemon Center)
    */
    void healAllPokemon()
    {
        for (int i = 0; i < 6; ++i)
        {
            if (pcPokemon[i].id > 0) // If pokemon slot isn't empty
            {
                pcPokemon[i].hp_cur = pcPokemon[i].hp_max;
                pcPokemon[i].is_dead = 0;
            }
        }
    }

    /**
     * Restores default number of items (for PokeMart)
    */
    void restoreItems()
    {
        potions = 3;
        revives = 3;
        poke_balls = 3;
    }

    /**
     * Returns 1 if player has room for a TM
     * 0 if all TM slots are full
    */
    int addTm(attack_move m)
    {
        for (int i = 0; i < 6; i++)
        {
            if (tm[i].id < 1)
            {
                tm[i] = m;
                return 1;
            }
        }
        return 0;
    }

};
pcData *pcData::instance;


#endif