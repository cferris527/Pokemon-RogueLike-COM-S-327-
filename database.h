#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <climits>

#include "pokedex.h"


/**
 * Searches for the database in /share and $HOME 
 * Returns 1 if database successfully located, 0 if not
*/
csvSource locatePokedex() 
{
    csvSource *s = new csvSource();
    std::ifstream csvFile;
    
    csvFile.open("/share/cs327/pokedex/pokedex/data/csv/pokemon.csv");

    if (csvFile.is_open()) {
        s->src = 0;
        return *s;
    } 
    else {
        csvFile.open("$HOME/.poke327/pokedex/pokedex/data/csv/pokemon.csv");
        if (csvFile.is_open()) {
            s->src = 1;
            return *s;
        } 
    }
    s->src = 2;
    return *s; // Database not located
}

pokemon* loadPokemon(int src) {
    pokemon* pokemonArray = new pokemon[1093];

    std::ifstream csvFile;
    std::string line;
    std::vector<std::vector<std::string>> data;

    if (!src) {
        csvFile.open("/share/cs327/pokedex/pokedex/data/csv/pokemon.csv");
    }
    else {
        csvFile.open("$HOME/.poke327/pokedex/pokedex/data/csv/pokemon.csv");
    }

    std::getline(csvFile, line); // ignore first line

    int i = 0;
    while (std::getline(csvFile, line)) {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> entries;

        while (std::getline(ss, field, ',')) {
            entries.push_back(field);
        }

        pokemon *p = new pokemon();
        //std::cerr << "<" << entries[0] << ">" << std::endl;
        
        p->id = std::stoi(entries[0]);
        p->name = entries[1];
        p->species_id = std::stoi(entries[2]);
        p->height = std::stoi(entries[3]);
        p->weight = std::stoi(entries[4]);
        p->base_experience = std::stoi(entries[5]);
        p->order = std::stoi(entries[6]);
        p->is_default = std::stoi(entries[7]);

        pokemonArray[i] = *p;
        ++i;
    }

    return pokemonArray;
}

void printPokemon(pokemon pokemonArray[1093]) {
    for (int i = 0; i < 1092; ++i) {
        std::cerr << 
        pokemonArray[i].id << "," <<
        pokemonArray[i].name << "," <<
        pokemonArray[i].species_id << "," <<
        pokemonArray[i].height << "," <<
        pokemonArray[i].weight << "," <<
        pokemonArray[i].base_experience << "," <<
        pokemonArray[i].order << ","<<
        pokemonArray[i].is_default <<
        std::endl;
    }
}

attack_move* loadMoves(int src) {

    attack_move* moveArray = new attack_move[845];

    std::ifstream csvFile;
    std::string line;
    std::vector<std::vector<std::string>> data;

    if (!src) {
        csvFile.open("/share/cs327/pokedex/pokedex/data/csv/moves.csv");
    }
    else {
        csvFile.open("$HOME/.poke327/pokedex/pokedex/data/csv/moves.csv");
    }

    std::getline(csvFile, line); // ignore first line

    int i = 0;
    while (std::getline(csvFile, line)) {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> entries;

        while (std::getline(ss, field, ',')) {
            entries.push_back(field);
        }

        attack_move *p = new attack_move();
        //std::cerr << "<" << entries[0] << ">" << std::endl;
        
        p->id = std::stoi(entries[0]);
        p->identifier = entries[1];

        if (entries[2] == "") { p->generation_id = INT_MAX;}
        else p->generation_id = std::stoi(entries[2]);

        if (entries[3] == "") { p->type_id = INT_MAX;}
        else p->type_id = std::stoi(entries[3]);

        if (entries[4] == "") { p->power = INT_MAX;}
        else p->power = std::stoi(entries[4]);

        if (entries[5] == "") { p->pp = INT_MAX;}
        else p->pp = std::stoi(entries[5]);

        if (entries[6] == "") { p->accuracy = INT_MAX;}
        else p->accuracy = std::stoi(entries[6]);

        if (entries[7] == "") { p->priority = INT_MAX;}
        else p->priority = std::stoi(entries[7]);

        if (entries[8] == "") { p->target_id = INT_MAX;}
        else p->target_id = std::stoi(entries[8]);

        if (entries[9] == "") { p->damage_class_id = INT_MAX;}
        else p->damage_class_id = std::stoi(entries[9]);

        if (entries[10] == "") { p->class_id = INT_MAX;}
        else p->class_id = std::stoi(entries[10]);

        if (entries[11] == "") { p->effect_id = INT_MAX;}
        else p->effect_id = std::stoi(entries[11]);

        if (entries[12] == "") { p->effect_chance = INT_MAX;}
        else p->effect_chance = std::stoi(entries[12]);

        if (entries[13] == "") { p->contest_type_id = INT_MAX;}
        else p->contest_type_id = std::stoi(entries[13]);

        if (entries[14] == "") { p->contest_effect_id = INT_MAX;}
        else p->contest_effect_id = std::stoi(entries[14]);

        if (entries[15] == "") { p->super_contest_effect_id = INT_MAX;}
        else p->super_contest_effect_id = std::stoi(entries[15]);

        moveArray[i] = *p;
        ++i;
    }

    return moveArray;
}

void printMoves(attack_move moveArray[845]) {
    for (int i = 0; i < 844; ++i) {

        std::string generation_id;
        if (moveArray[i].generation_id == INT_MAX) {generation_id = "";}
        else generation_id = std::to_string(moveArray[i].generation_id);

        std::string type_id;
        if (moveArray[i].type_id == INT_MAX) {type_id = "";}
        else type_id = std::to_string(moveArray[i].type_id);

        std::string power;
        if (moveArray[i].power == INT_MAX) {power = "";}
        else power = std::to_string(moveArray[i].power);

        std::string pp;
        if (moveArray[i].pp == INT_MAX) {pp = "";}
        else pp = std::to_string(moveArray[i].pp);

        std::string accuracy;
        if (moveArray[i].accuracy == INT_MAX) {accuracy = "";}
        else accuracy = std::to_string(moveArray[i].accuracy);

        std::string priority;
        if (moveArray[i].priority == INT_MAX) {priority = "";}
        else priority = std::to_string(moveArray[i].priority);

        std::string target_id;
        if (moveArray[i].target_id == INT_MAX) {target_id = "";}
        else target_id = std::to_string(moveArray[i].target_id);

        std::string damage_class_id;
        if (moveArray[i].damage_class_id == INT_MAX) {damage_class_id = "";}
        else damage_class_id = std::to_string(moveArray[i].damage_class_id);

        std::string class_id;
        if (moveArray[i].class_id == INT_MAX) {class_id = "";}
        else class_id = std::to_string(moveArray[i].class_id);

        std::string effect_id;
        if (moveArray[i].effect_id == INT_MAX) {effect_id = "";}
        else effect_id = std::to_string(moveArray[i].effect_id);

        std::string effect_chance;
        if (moveArray[i].effect_chance == INT_MAX) {effect_chance = "";}
        else effect_chance = std::to_string(moveArray[i].effect_chance);

        std::string contest_type_id;
        if (moveArray[i].contest_type_id == INT_MAX) {contest_type_id = "";}
        else contest_type_id = std::to_string(moveArray[i].contest_type_id);

        std::string contest_effect_id;
        if (moveArray[i].contest_effect_id == INT_MAX) {contest_effect_id = "";}
        else contest_effect_id = std::to_string(moveArray[i].contest_effect_id);

        std::string super_contest_effect_id;
        if (moveArray[i].super_contest_effect_id == INT_MAX) {super_contest_effect_id = "";}
        else super_contest_effect_id = std::to_string(moveArray[i].super_contest_effect_id);

        std::cout << 
        moveArray[i].id << "," <<
        moveArray[i].identifier << "," <<
        generation_id << "," <<
        type_id << "," <<
        power << "," <<
        pp << "," <<
        accuracy << "," <<
        priority << "," <<
        target_id << "," <<
        damage_class_id << "," <<
        class_id << "," <<
        effect_id << "," <<
        effect_chance << "," <<
        contest_type_id << "," <<
        contest_effect_id << "," <<
        super_contest_effect_id <<
        std::endl;

    }
}

pokemon_move* loadPokemonMoves(int src) {

    pokemon_move* pokemonMoveArray = new pokemon_move[528240];

    std::ifstream csvFile;
    std::string line;
    std::vector<std::vector<std::string>> data;

    if (!src) {
        csvFile.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_moves.csv");
    }
    else {
        csvFile.open("$HOME/.poke327/pokedex/pokedex/data/csv/pokemon_moves.csv");
    }

    std::getline(csvFile, line); // ignore first line

    int i = 0;
    while (std::getline(csvFile, line)) {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> entries;

        while (std::getline(ss, field, ',')) {
            entries.push_back(field);
        }

        pokemon_move *p = new pokemon_move();

        if (entries[0] == "") { p->pokemon_id = INT_MAX;}
        else p->pokemon_id = std::stoi(entries[0]);

        if (entries[1] == "") { p->version_group_id = INT_MAX;}
        else p->version_group_id = std::stoi(entries[1]);

        if (entries[2] == "") { p->move_id = INT_MAX;}
        else p->move_id = std::stoi(entries[2]);

        if (entries[3] == "") { p->pokemon_move_method_id = INT_MAX;}
        else p->pokemon_move_method_id = std::stoi(entries[3]);

        if (entries[4] == "") { p->level = INT_MAX;}
        else p->level = std::stoi(entries[4]);

        if (entries[5] == "") { p->order = INT_MAX;}
        else p->order = std::stoi(entries[5]);

        pokemonMoveArray[i] = *p;
        ++i;
    }

    return pokemonMoveArray;
}

void printPokemonMoves(pokemon_move pokemonMoveArray[528239]) {
    for (int i = 0; i < 528238; ++i) {

        std::string pokemon_id;
        if (pokemonMoveArray[i].pokemon_id == INT_MAX) {pokemon_id = "";}
        else pokemon_id = std::to_string(pokemonMoveArray[i].pokemon_id);

        std::string version_group_id;
        if (pokemonMoveArray[i].version_group_id == INT_MAX) {version_group_id = "";}
        else version_group_id = std::to_string(pokemonMoveArray[i].version_group_id);

        std::string move_id;
        if (pokemonMoveArray[i].move_id == INT_MAX) {move_id = "";}
        else move_id = std::to_string(pokemonMoveArray[i].move_id);

        std::string pokemon_move_method_id;
        if (pokemonMoveArray[i].pokemon_move_method_id == INT_MAX) {pokemon_move_method_id = "";}
        else pokemon_move_method_id = std::to_string(pokemonMoveArray[i].pokemon_move_method_id);

        std::string level;
        if (pokemonMoveArray[i].level == INT_MAX) {level = "";}
        else level = std::to_string(pokemonMoveArray[i].level);

        std::string order;
        if (pokemonMoveArray[i].order == INT_MAX) {order = "";}
        else order = std::to_string(pokemonMoveArray[i].order);

        std::cout << 
        pokemon_id << "," <<
        version_group_id << "," <<
        move_id << "," <<
        pokemon_move_method_id << "," <<
        level << "," <<
        order <<
        std::endl;

    }
}

pokemon_species* loadPokemonSpecies(int src) {

    pokemon_species* pokemonSpeciesArray = new pokemon_species[899];

    std::ifstream csvFile;
    std::string line;
    std::vector<std::vector<std::string>> data;

    if (!src) {
        csvFile.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_species.csv");
    }
    else {
        csvFile.open("$HOME/.poke327/pokedex/pokedex/data/csv/pokemon_species.csv");
    }

    std::getline(csvFile, line); // ignore first line

    int i = 0;
    while (std::getline(csvFile, line)) {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> entries;

        while (std::getline(ss, field, ',')) {
            entries.push_back(field);
        }

        pokemon_species *p = new pokemon_species();

        if (entries[0] == "") { p->id = INT_MAX;}
        else p->id = std::stoi(entries[0]);

        p->identifier = entries[1];

        if (entries[2] == "") { p->generation_id = INT_MAX;}
        else p->generation_id = std::stoi(entries[2]);

        if (entries[3] == "") { p->evolves_from_species_id = INT_MAX;}
        else p->evolves_from_species_id = std::stoi(entries[3]);

        if (entries[4] == "") { p->evolution_chain_id = INT_MAX;}
        else p->evolution_chain_id = std::stoi(entries[4]);

        if (entries[5] == "") { p->color_id = INT_MAX;}
        else p->color_id = std::stoi(entries[5]);

        if (entries[6] == "") { p->shape_id = INT_MAX;}
        else p->shape_id = std::stoi(entries[6]);

        if (entries[7] == "") { p->habitat_id = INT_MAX;}
        else p->habitat_id = std::stoi(entries[7]);

        if (entries[8] == "") { p->gender_rate = INT_MAX;}
        else p->gender_rate = std::stoi(entries[8]);

        if (entries[9] == "") { p->capture_rate = INT_MAX;}
        else p->capture_rate = std::stoi(entries[9]);

        if (entries[10] == "") { p->base_happiness = INT_MAX;}
        else p->base_happiness = std::stoi(entries[10]);

        if (entries[11] == "") { p->is_baby = INT_MAX;}
        else p->is_baby = std::stoi(entries[11]);

        if (entries[12] == "") { p->hatch_counter = INT_MAX;}
        else p->hatch_counter = std::stoi(entries[12]);

        if (entries[13] == "") { p->has_gender_differences = INT_MAX;}
        else p->has_gender_differences = std::stoi(entries[13]);

        if (entries[14] == "") { p->growth_rate_id = INT_MAX;}
        else p->growth_rate_id = std::stoi(entries[14]);

        if (entries[15] == "") { p->forms_switchable = INT_MAX;}
        else p->forms_switchable = std::stoi(entries[15]);

        if (entries[16] == "") { p->is_legendary = INT_MAX;}
        else p->is_legendary = std::stoi(entries[16]);

        if (entries[17] == "") { p->is_mythical = INT_MAX;}
        else p->is_mythical = std::stoi(entries[17]);

        if (entries[18] == "") { p->order = INT_MAX;}
        else p->order = std::stoi(entries[18]);

        if (entries[19] == "") { p->conquest_order = INT_MAX;}
        else p->conquest_order = std::stoi(entries[19]);

        pokemonSpeciesArray[i] = *p;
        ++i;
    }

    return pokemonSpeciesArray;
}

void printPokemonSpecies(pokemon_species pokemonSpeciesArray[899]) {
    for (int i = 0; i < 898; ++i) {

        std::string id;
        if (pokemonSpeciesArray[i].id == INT_MAX) {id = "";}
        else id = std::to_string(pokemonSpeciesArray[i].id);

        std::string generation_id;
        if (pokemonSpeciesArray[i].generation_id == INT_MAX) {generation_id = "";}
        else generation_id = std::to_string(pokemonSpeciesArray[i].generation_id);

        std::string evolves_from_species_id;
        if (pokemonSpeciesArray[i].evolves_from_species_id == INT_MAX) {evolves_from_species_id = "";}
        else evolves_from_species_id = std::to_string(pokemonSpeciesArray[i].evolves_from_species_id);
        
        std::string evolution_chain_id;
        if (pokemonSpeciesArray[i].evolution_chain_id == INT_MAX) {evolution_chain_id = "";}
        else evolution_chain_id = std::to_string(pokemonSpeciesArray[i].evolution_chain_id);

        std::string color_id;
        if (pokemonSpeciesArray[i].color_id == INT_MAX) {color_id = "";}
        else color_id = std::to_string(pokemonSpeciesArray[i].color_id);

        std::string shape_id;
        if (pokemonSpeciesArray[i].shape_id == INT_MAX) {shape_id = "";}
        else shape_id = std::to_string(pokemonSpeciesArray[i].shape_id);

        std::string habitat_id;
        if (pokemonSpeciesArray[i].habitat_id == INT_MAX) {habitat_id = "";}
        else habitat_id = std::to_string(pokemonSpeciesArray[i].habitat_id);

        std::string gender_rate;
        if (pokemonSpeciesArray[i].gender_rate == INT_MAX) {gender_rate = "";}
        else gender_rate = std::to_string(pokemonSpeciesArray[i].gender_rate);

        std::string capture_rate;
        if (pokemonSpeciesArray[i].capture_rate == INT_MAX) {capture_rate = "";}
        else capture_rate = std::to_string(pokemonSpeciesArray[i].capture_rate);

        std::string base_happiness;
        if (pokemonSpeciesArray[i].base_happiness == INT_MAX) {base_happiness = "";}
        else base_happiness = std::to_string(pokemonSpeciesArray[i].base_happiness);

        std::string is_baby;
        if (pokemonSpeciesArray[i].is_baby == INT_MAX) {is_baby = "";}
        else is_baby = std::to_string(pokemonSpeciesArray[i].is_baby);

        std::string hatch_counter;
        if (pokemonSpeciesArray[i].hatch_counter == INT_MAX) {hatch_counter = "";}
        else hatch_counter = std::to_string(pokemonSpeciesArray[i].hatch_counter);

        std::string has_gender_differences;
        if (pokemonSpeciesArray[i].has_gender_differences == INT_MAX) {has_gender_differences = "";}
        else has_gender_differences = std::to_string(pokemonSpeciesArray[i].has_gender_differences);

        std::string growth_rate_id;
        if (pokemonSpeciesArray[i].growth_rate_id == INT_MAX) {growth_rate_id = "";}
        else growth_rate_id = std::to_string(pokemonSpeciesArray[i].growth_rate_id);

        std::string forms_switchable;
        if (pokemonSpeciesArray[i].forms_switchable == INT_MAX) {forms_switchable = "";}
        else forms_switchable = std::to_string(pokemonSpeciesArray[i].forms_switchable);

        std::string is_legendary;
        if (pokemonSpeciesArray[i].is_legendary == INT_MAX) {is_legendary = "";}
        else is_legendary = std::to_string(pokemonSpeciesArray[i].is_legendary);

        std::string is_mythical;
        if (pokemonSpeciesArray[i].is_mythical == INT_MAX) {is_mythical = "";}
        else is_mythical = std::to_string(pokemonSpeciesArray[i].is_mythical);

        std::string order;
        if (pokemonSpeciesArray[i].order == INT_MAX) {order = "";}
        else order = std::to_string(pokemonSpeciesArray[i].order);

        std::string conquest_order;
        if (pokemonSpeciesArray[i].conquest_order == INT_MAX) {conquest_order = "";}
        else conquest_order = std::to_string(pokemonSpeciesArray[i].conquest_order);

        std::cout << 
        id << "," <<
        pokemonSpeciesArray[i].identifier << "," <<
        generation_id << "," <<
        evolves_from_species_id << "," <<
        evolution_chain_id << "," <<
        color_id << "," <<
        shape_id << "," <<
        habitat_id << "," <<
        gender_rate << "," <<
        capture_rate << "," <<
        base_happiness << "," <<
        is_baby << "," <<
        hatch_counter << "," <<
        has_gender_differences << "," <<
        growth_rate_id << "," <<
        forms_switchable << "," <<
        is_legendary << "," <<
        is_mythical << "," <<
        order << "," <<
        conquest_order <<
        std::endl;

    }
}

experience* loadExperience(int src) {

    experience* expArray = new experience[601];

    std::ifstream csvFile;
    std::string line;
    std::vector<std::vector<std::string>> data;

    if (!src) {
        csvFile.open("/share/cs327/pokedex/pokedex/data/csv/experience.csv");
    }
    else {
        csvFile.open("$HOME/.poke327/pokedex/pokedex/data/csv/experience.csv");
    }

    std::getline(csvFile, line); // ignore first line

    int i = 0;
    while (std::getline(csvFile, line)) {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> entries;

        while (std::getline(ss, field, ',')) {
            entries.push_back(field);
        }

        experience *p = new experience();

        if (entries[0] == "") { p->growth_rate_id = INT_MAX;}
        else p->growth_rate_id = std::stoi(entries[0]);

        if (entries[1] == "") { p->level = INT_MAX;}
        else p->level = std::stoi(entries[1]);

        if (entries[2] == "") { p->experience_needed = INT_MAX;}
        else p->experience_needed = std::stoi(entries[2]);

        expArray[i] = *p;
        ++i;
    }

    return expArray;
}

void printExperience(experience expArray[601]) {
    for (int i = 0; i < 600; ++i) {

        std::string growth_rate_id;
        if (expArray[i].growth_rate_id == INT_MAX) {growth_rate_id = "";}
        else growth_rate_id = std::to_string(expArray[i].growth_rate_id);

        std::string level;
        if (expArray[i].level == INT_MAX) {level = "";}
        else level = std::to_string(expArray[i].level);

        std::string experience_needed;
        if (expArray[i].experience_needed == INT_MAX) {experience_needed = "";}
        else experience_needed = std::to_string(expArray[i].experience_needed);

        std::cout << 
        growth_rate_id << "," <<
        level << "," <<
        experience_needed <<
        std::endl;

    }
}

type_name* loadtypeName(int src) {

    type_name* typeNameArray = new type_name[21];

    std::ifstream csvFile;
    std::string line;
    std::vector<std::vector<std::string>> data;

    if (!src) {
        csvFile.open("/share/cs327/pokedex/pokedex/data/csv/type_names.csv");
    }
    else {
        csvFile.open("$HOME/.poke327/pokedex/pokedex/data/csv/type_names.csv");
    }

    std::getline(csvFile, line); // ignore first line

    int i = 0;
    while (std::getline(csvFile, line)) {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> entries;

        while (std::getline(ss, field, ',')) {
            entries.push_back(field);
        }

        type_name *p = new type_name();

        // if line is English
        if (entries[1] == "9") {
            if (entries[0] == "") { p->type_id = INT_MAX;}
            else p->type_id = std::stoi(entries[0]);

            if (entries[1] == "") { p->local_language_id = INT_MAX;}
            else p->local_language_id = std::stoi(entries[1]);

            p->name = entries[2];

            typeNameArray[i] = *p;
            ++i;
        }
    }

    return typeNameArray;
}

void printTypeName(type_name typeNameArray[21]) {
    for (int i = 0; i < 20; ++i) {

        std::string type_id;
        if (typeNameArray[i].type_id == INT_MAX) {type_id = "";}
        else type_id = std::to_string(typeNameArray[i].type_id);

        std::string local_language_id;
        if (typeNameArray[i].local_language_id == INT_MAX) {local_language_id = "";}
        else local_language_id = std::to_string(typeNameArray[i].local_language_id);

        std::cout << 
        type_id << "," <<
        local_language_id << "," <<
        typeNameArray[i].name <<
        std::endl;

    }
}

pokemon_stat* loadPokemonStats(int src) {

    pokemon_stat* pokemonStatArray = new pokemon_stat[6553];

    std::ifstream csvFile;
    std::string line;
    std::vector<std::vector<std::string>> data;

    if (!src) {
        csvFile.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_stats.csv");
    }
    else {
        csvFile.open("$HOME/.poke327/pokedex/pokedex/data/csv/pokemon_stats.csv");
    }

    std::getline(csvFile, line); // ignore first line

    int i = 0;
    while (std::getline(csvFile, line)) {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> entries;

        while (std::getline(ss, field, ',')) {
            entries.push_back(field);
        }

        pokemon_stat *p = new pokemon_stat();

        if (entries[0] == "") { p->pokemon_id = INT_MAX;}
        else p->pokemon_id = std::stoi(entries[0]);

        if (entries[1] == "") { p->stat_id = INT_MAX;}
        else p->stat_id = std::stoi(entries[1]);

        if (entries[2] == "") { p->base_stat = INT_MAX;}
        else p->base_stat = std::stoi(entries[2]);

        if (entries[3] == "") { p->effort = INT_MAX;}
        else p->effort = std::stoi(entries[3]);

        pokemonStatArray[i] = *p;
        ++i;
    }

    return pokemonStatArray;
}

void printPokemonStats(pokemon_stat pokemonStatArray[6553]) {
    for (int i = 0; i < 6552; ++i) {

        std::string pokemon_id;
        if (pokemonStatArray[i].pokemon_id == INT_MAX) {pokemon_id = "";}
        else pokemon_id = std::to_string(pokemonStatArray[i].pokemon_id);

        std::string stat_id;
        if (pokemonStatArray[i].stat_id == INT_MAX) {stat_id = "";}
        else stat_id = std::to_string(pokemonStatArray[i].stat_id);

        std::string base_stat;
        if (pokemonStatArray[i].base_stat == INT_MAX) {base_stat = "";}
        else base_stat = std::to_string(pokemonStatArray[i].base_stat);

        std::string effort;
        if (pokemonStatArray[i].effort == INT_MAX) {effort = "";}
        else effort = std::to_string(pokemonStatArray[i].effort);

        std::cout << 
        pokemon_id << "," <<
        stat_id << "," <<
        base_stat << "," <<
        effort << 
        std::endl;

    }
}

stat* loadStats(int src) {

    stat* statArray = new stat[9];

    std::ifstream csvFile;
    std::string line;
    std::vector<std::vector<std::string>> data;

    if (!src) {
        csvFile.open("/share/cs327/pokedex/pokedex/data/csv/stats.csv");
    }
    else {
        csvFile.open("$HOME/.poke327/pokedex/pokedex/data/csv/stats.csv");
    }

    std::getline(csvFile, line); // ignore first line

    int i = 0;
    while (std::getline(csvFile, line)) {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> entries;

        while (std::getline(ss, field, ',')) {
            entries.push_back(field);
        }
        stat *p = new stat();
        if (entries.size() >= 5) {
        
        if (entries[0] == "") { p->id = INT_MAX;}
        else p->id = std::stoi(entries[0]);

        if (entries[1] == "") { p->damage_class_id = INT_MAX;}
        else p->damage_class_id = std::stoi(entries[1]);

        p->identifier = entries[2];

        if (entries[3] == "") { p->is_battle_only = INT_MAX;}
        else p->is_battle_only = std::stoi(entries[3]);

        if (entries[4] == "") { p->game_index = INT_MAX;}
        else p->game_index = std::stoi(entries[4]);
        }
        else {
            if (entries[0] == "") { p->id = INT_MAX;}
            else p->id = std::stoi(entries[0]);

            if (entries[1] == "") { p->damage_class_id = INT_MAX;}
            else p->damage_class_id = std::stoi(entries[1]);

            p->identifier = entries[2];

            if (entries[3] == "") { p->is_battle_only = INT_MAX;}
            else p->is_battle_only = std::stoi(entries[3]);

            p->game_index = INT_MAX;
        }
        statArray[i] = *p;
        ++i;
    }

    return statArray;
}

void printStats(stat statArray[9]) {
    for (int i = 0; i < 8; ++i) {

        std::string id;
        if (statArray[i].id == INT_MAX) {id = "";}
        else id = std::to_string(statArray[i].id);

        std::string damage_class_id;
        if (statArray[i].damage_class_id == INT_MAX) {damage_class_id = "";}
        else damage_class_id = std::to_string(statArray[i].damage_class_id);

        std::string is_battle_only;
        if (statArray[i].is_battle_only == INT_MAX) {is_battle_only = "";}
        else is_battle_only = std::to_string(statArray[i].is_battle_only);

        std::string game_index;
        if (statArray[i].game_index == INT_MAX) {game_index = "";}
        else game_index = std::to_string(statArray[i].game_index);

        std::cout << 
        id << "," <<
        damage_class_id << "," <<
        statArray[i].identifier << "," <<
        is_battle_only << "," <<
        game_index << 
        std::endl;

    }
}

pokemon_type* loadPokemonTypes(int src) {

    pokemon_type* pokemonTypesArray = new pokemon_type[1676];

    std::ifstream csvFile;
    std::string line;
    std::vector<std::vector<std::string>> data;

    if (!src) {
        csvFile.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_types.csv");
    }
    else {
        csvFile.open("$HOME/.poke327/pokedex/pokedex/data/csv/pokemon_types.csv");
    }

    std::getline(csvFile, line); // ignore first line

    int i = 0;
    while (std::getline(csvFile, line)) {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> entries;

        while (std::getline(ss, field, ',')) {
            entries.push_back(field);
        }

        pokemon_type *p = new pokemon_type();

        if (entries[0] == "") { p->pokemon_id = INT_MAX;}
        else p->pokemon_id = std::stoi(entries[0]);

        if (entries[1] == "") { p->type_id = INT_MAX;}
        else p->type_id = std::stoi(entries[1]);

        if (entries[2] == "") { p->slot = INT_MAX;}
        else p->slot = std::stoi(entries[2]);

        pokemonTypesArray[i] = *p;
        ++i;
    }

    return pokemonTypesArray;
}

void printPokemonTypes(pokemon_type pokemonTypesArray[1676]) {
    for (int i = 0; i < 1675; ++i) {

        std::string pokemon_id;
        if (pokemonTypesArray[i].pokemon_id == INT_MAX) {pokemon_id = "";}
        else pokemon_id = std::to_string(pokemonTypesArray[i].pokemon_id);

        std::string type_id;
        if (pokemonTypesArray[i].type_id == INT_MAX) {type_id = "";}
        else type_id = std::to_string(pokemonTypesArray[i].type_id);

        std::string slot;
        if (pokemonTypesArray[i].slot == INT_MAX) {slot = "";}
        else slot = std::to_string(pokemonTypesArray[i].slot);

        std::cout << 
        pokemon_id << "," <<
        type_id << "," <<
        slot << 
        std::endl;

    }
}

type_efficacy* loadTypeEfficacies(int src) {

    type_efficacy* typeEfficacyArray = new type_efficacy[325];

    std::ifstream csvFile;
    std::string line;
    std::vector<std::vector<std::string>> data;

    if (!src) {
        csvFile.open("/share/cs327/pokedex/pokedex/data/csv/type_efficacy.csv");
    }
    else {
        csvFile.open("$HOME/.poke327/pokedex/pokedex/data/csv/type_efficacy.csv");
    }

    std::getline(csvFile, line); // ignore first line

    int i = 0;
    while (std::getline(csvFile, line)) {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> entries;

        while (std::getline(ss, field, ',')) {
            entries.push_back(field);
        }

        type_efficacy *t = new type_efficacy();

        if (entries[0] == "") { t->damage_type_id = INT_MAX;}
        else t->damage_type_id = std::stoi(entries[0]);

        if (entries[1] == "") { t->target_type_id = INT_MAX;}
        else t->target_type_id = std::stoi(entries[1]);

        if (entries[2] == "") { t->damage_factor = INT_MAX;}
        else t->damage_factor = std::stoi(entries[2]);

        typeEfficacyArray[i] = *t;
        ++i;
    }

    return typeEfficacyArray;
}

#endif