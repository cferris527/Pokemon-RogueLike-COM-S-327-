#ifndef POKEDEX_H
#define POKEDEX_H

#include <string>

class csvSource {
public:
    int src; // src is 0 for /share, src is 1 for $HOME, src is 2 for not found

    csvSource() {};
};

class attack_move {
public:
    int id;
    std::string identifier;
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;

    attack_move() {};
};

class pokemon {
public:
    int id;
    std::string name;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;

    /* Not loaded in initial CSV */
    int level;

    /* IV values */
    int hp;
    int attack;
    int defense;
    int spAtt;
    int spDef;
    int speed;

    /* Other values */
    int gender; // 0 = female, 1 = male

    int hp_max; // Max HP pokemon can have
    int hp_cur; // HP pokemon currently has
    int is_dead;

    int type_one;
    int type_two;

    /* Moves */
    attack_move move_one;
    attack_move move_two;

    pokemon() {};
};

class pokemon_move {
public:
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;

    pokemon_move() {};
};

class pokemon_species {
public:
    int id;
    std::string identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;

    pokemon_species() {};
};

class experience {
public:
    int growth_rate_id;
    int level;
    int experience_needed;

    experience() {};
};

class type_name {
public:
    int type_id;
    int local_language_id;
    std::string name;

    type_name() {};
};

class pokemon_stat {
public:
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;

    pokemon_stat() {};
};

class stat {
public:
    int id;
    int damage_class_id;
    std::string identifier;
    int is_battle_only;
    int game_index;

    stat() {};
};

class pokemon_type {
public:
    int pokemon_id;
    int type_id;
    int slot;

    pokemon_type() {};
};

class type_efficacy {
public:
    int damage_type_id;
    int target_type_id;
    int damage_factor;

    type_efficacy() {};
};

#endif