#include "map.h"
#include "data.h"

int map::calcManhattan() 
{
  std::random_device rd;
  std::mt19937 gen(rd());

  int x = mapCoords.getRow();
  int y = mapCoords.getCol();

  int dist = abs(200 - x) + abs(200 - y) + 2;
  
  int ohs;

  if (dist < 200) {
    ohs = dist/2;
    std::uniform_int_distribution<> distribution(1, ohs);
    return distribution(gen);
  }
  else {
    ohs = dist - 200;
    std::uniform_int_distribution<> distribution(ohs, 100);
    return distribution(gen);
  }
}

/**
 * Critical hit not calculated here. Everything else is
*/
int map::calcDamage(int att_level, int def_level, int power, int attack, int defense, 
int move_type, int att_poke_type_one, int att_poke_type_two,
int def_poke_type_one, int def_poke_type_two)
{
  typeEfficacyData *te;
  te = typeEfficacyData::getInstance();

  //Adjust attack and defense IVs to value appropriate for level
  attack = (((2 * attack) * att_level) / 100) + 5;
  defense = (((2 * defense) * def_level) / 100) + 5;

  int firstParen = ((((((2 * att_level)/5) + 2) * power) * (attack/defense)) / 50) + 2;

  double random = (double)rand() / RAND_MAX;
  random = 0.85 + random * 0.15;
  //double random = (rand() % 16 + 85) / 100; 
  double stab;
  double type = 1;

  // If same type attack bonus
  if (move_type == att_poke_type_one || move_type == att_poke_type_two) {
    stab = 1.5;
  }
  else {
    stab = 1;
  }

  //Calculate if defending pokemon has weakness/resistance to move type for each pokemon type
  int select_type;
  for (int i = 0; i < 2; ++i) {
    if (i == 0) {select_type = def_poke_type_one;}
    if (i == 1) {select_type = def_poke_type_two;}

    if (i > 0) {continue;} // If type is invalid (for pokemon that only have one type)

    for (int j = 0; j < te->getLength(); ++j)
    {
      if (te->getTypeEfficacy(j).damage_type_id == move_type
      && te->getTypeEfficacy(j).target_type_id == select_type) {

        if (te->getTypeEfficacy(j).damage_factor == 50
        && type == 2) {
          type = 1;
        }

        else if (te->getTypeEfficacy(j).damage_factor == 50
        && type == 1) { 
          type = .5;
        }

        else if (te->getTypeEfficacy(j).damage_factor == 50
        && type == .5) {
          type = .25;
        }

        else if (te->getTypeEfficacy(j).damage_factor == 200
        && type == 2) {
          type = 4;
        }

        else if (te->getTypeEfficacy(j).damage_factor == 200
        && type == 1) {
          type = 2;
        }

        else if (te->getTypeEfficacy(j).damage_factor == 200
        && type == .5) {
          type = 1;
        }

        else if (te->getTypeEfficacy(j).damage_factor == 0) {
          type = 0;
        }
      }
    }
  }
  return firstParen * random * stab * type; // This is before crit
}

int map::whosFirst(int moveZero_priority, int moveOne_priority,int pokeZero_speed, int pokeOne_speed)
{

  if (moveZero_priority > moveOne_priority) {
    return 0;
  }
  else if (moveOne_priority < moveZero_priority) {
    return 1;
  }
  
  // If move priorities are equal:
  if (pokeZero_speed > pokeOne_speed) {
    return 0;
  }
  else if (pokeOne_speed > pokeZero_speed) {
    return 1;
  }

  // If pokemon speeds are equal, pick a random one
  return rand() % 2; // 0-1 
}

double map::calcCritical(int speed) {

  if ((rand() % 256) < (speed / 2)) {
    return 1.5;
  }
  return 1;
}

/**
 * One pokemon performs an attack on another.
*/
void map::doAnAttack(pokemon* attacking, pokemon* defending, int flag, int move_index)
{
  // If flag = 1, PC poke is attacking, else Wild poke is attacking
  if (flag) {printWildUi(*defending, *attacking);}
  else {printWildUi(*attacking, *defending);}

  // move_index = 0 : move_one
  //move index = 1 ; move_two
  attack_move move_selected;
  if (move_index) {move_selected = attacking->move_two;}
  else {move_selected = attacking->move_one;}

  int base_damage = calcDamage(attacking->level, defending->level, move_selected.power, 
  attacking->attack, defending->defense, move_selected.type_id, 
  attacking->type_one, attacking->type_two, defending->type_one, defending->type_two);

  double critical = calcCritical(attacking->speed);

  int total_damage = base_damage * critical;

  printw("%s used %s!\n", attacking->name.c_str(), move_selected.identifier.c_str());

  printw("\nPress any key to continue\n");
  getch();

  // If attack missed

  // If flag = 1, PC poke is attacking,

  if (rand() % 100 < move_selected.accuracy) { // If attack DOESN'T miss
    defending->hp_cur -= total_damage; // Apply damage
    if (defending->hp_cur <= 0) {
      defending->hp_cur = 0; // HP can't go below 0
      defending->is_dead = 1; // Defending pokemon is dead if HP < 0
    }

    if (flag) {
      printWildUi(*defending, *attacking);
    }
    //else Wild poke is attacking
    else {
      printWildUi(*attacking, *defending);
    }
    printw("The attack did %d damage ", total_damage);


    //If critical hit happened, include in text
    if (critical == 1.5) {printw(" (a critical hit!)");}
    printw("to %s.\n", defending->name.c_str());

  }
  else { // If attack missed
    if (flag) {
      printWildUi(*defending, *attacking);
    }
  // else Wild poke is attacking
    else {
      printWildUi(*attacking, *defending);
    }
    printw("The attack missed!\n");
  }

  printw("\nPress any key to continue\n");
  getch();

}

map::map() {
  generated = 0;
  playerThruGate = 0;
  row_north_gate = -1;
  col_west_gate = -1;
  row_south_gate = -1;
  col_east_gate = -1;
}

/**
 * Initializes map. DOES NOT GENERATE TERRAIN YET.
 * north, east, south, west are gate locations
 * to be passed in by game.
*/
map::map(int mapRow, int mapCol, int north, int east, int south, int west, int pcRow, int pcCol) {

    //initialize path coords to -1,-1
    for (int i = 0; i < 100; ++i) {
        paths[i] = coords();
    }
    //also initialize gate locations, adjacent map gates unknown at this point
    row_north_gate = north;
    col_west_gate = west;
    row_south_gate = south;
    col_east_gate = east;

    mapCoords = coords(mapRow, mapCol);

    generate(pcRow, pcCol);
    generated = 1;
    playerThruGate = 0;

    if(mapRow == 200 && mapCol == 200) {
      isOriginMap = 1;
      hasSelectedPokemon = 0;
    }
    else {
      isOriginMap = 0;
      hasSelectedPokemon = 1;
    }
}

/**
 * Sets all terrain chars to 'E'.
*/
void map::initializeTerrain()
{
    for (int i = 0; i < 21; ++i) {
        for (int j = 0; j < 80; ++j) {
            terrain[i][j] = 'E';
        }
    }
}

/**
 * Returns whether or not the map has been generated or not
*/
int map::isGenerated() { return generated;}

int map::getPcRow() {return pc.getRow();}

int map::getPcCol() {return pc.getCol();}

void map::setPcRow(int v) {pc.setRow(v);}

void map::setPcCol(int v) {pc.setCol(v);}

/**
 * Creates different type regions on the map.
 */
void map::createRegions() {

  int randRow, randCol, blotX, blotY;

  char typeList[5];
  typeList[0] = ':';
  typeList[1] = '.';
  typeList[2] = ':';
  typeList[3] = '.';
  typeList[4] = '~';

  for (int i = 0; i < 5; ++i) {
    blotX = rand() % 3 + 12;  // 12-25
    blotY = rand() % 19 + 12; // 12-36
    randRow = rand() % 15 + 2; // 2-16
    randCol = rand() % 73 + 2; // 2-74
    makeBlot(randRow, randCol, typeList[i], blotX, blotY);
  }
}

/*
  Makes (size) blot centered around initial point
*/
void map::makeBlot(int row, int col, char type, int size_row, int size_col) {

  int high_row = size_row/2;
  int low_row = high_row - (high_row * 2);
  int high_col = size_col/2;
  int low_col = high_col - (high_col * 2);
  int chance;
  
  for(int i = low_row; i < high_row; i++){
    for(int j = low_col; j < high_col; j++){

      chance = rand() % 3; // 0-2
      //if row, col is valid and RNG hits (75% chance of success)
      if( (row + i >= 0) && (row + i <= 20) &&
	    (col + j >= 0) && (col + j <= 79)) {

	      terrain[row+i][col+j] = type;

	      if (i == high_row - 1 || i == low_row ||
	        j == high_col - 1 || j == low_row) {

	        if(chance != 0) {
            terrain[row+i][col+j] = 'E';
          }
	      }
      }
    }
  }
}

/**
 * Assigns terrain to any cells that are still 'E'
*/
void map::patchEmptyCells() {

  int tally_grass;
  int tally_clear;
  int tally_water;
  int tally_empty;
  int RNG;
  char fill;
  //scans for closest char in 3x3 radius
  for(int i = 1; i < 20; i++){
    for(int j = 1; j < 79; j++){

      if(terrain[i][j] == 'E'){

	      tally_grass = 0; // :
	      tally_clear = 0; // .
	      tally_water = 0; // ~
	      tally_empty = 0; // E
	      for(int l = -1; l < 2; ++l) {
	        for(int p = -1; p < 2; ++p) {
	          if (terrain[i + l][j + p] == 'E'){tally_empty++;}
	          else if (terrain[i + l][j + p] == ':'){tally_grass++;}
	          else if (terrain[i + l][j + p] == '.'){tally_clear++;}
	          else if (terrain[i + l][j + p] == '~'){tally_water++;}
	        }
	      }

	      RNG = rand() % 40; //0-39
	      fill = '.';
	
	      if(tally_grass > tally_clear){fill = ':';}
	      else {fill = ':';}
	      if(tally_clear >= tally_water){fill = '.';}
	      else {fill = '~';}
	      if(tally_water > tally_clear){fill = '~';}
	      else {fill = '.';}

	      if(RNG == 39 && fill == '.'){fill = '%';}
	      if((RNG == 38 || RNG == 37) && fill == '.'){fill = '^';}
	
	      terrain[i][j] = fill;
	
      }     
    }
  }
}

/*
  Places outside boulders ('=', but printed as '%' later) to create border around terrain.
*/
void map::createBorders()
{
  for (int i = 0; i < 80; ++i) {
    if (i < 21) {
      terrain[i][0] = '=';
      terrain[i][79] = '=';
    }
    terrain[0][i] = '=';
    terrain[20][i] = '=';
  }
}

void map::createPath()
{
  int northRow, westCol, southRow, eastCol;
  //determine whether or not path locations need to be null
  if (col_west_gate == -1){ col_west_gate = rand() % 19 + 1; }
  if (row_south_gate == -1){ row_south_gate = rand() % 77 + 1; }
  if (col_east_gate == -1){ col_east_gate = rand() % 19 + 1; }
  if (row_north_gate == -1){ row_north_gate = rand() % 77 + 1; }

  northRow = row_north_gate;
  westCol = col_west_gate;
  southRow = row_south_gate;
  eastCol = col_east_gate;
  
  int pathLocations[156];
  int location_index = 0;
  //format [col][row]
 
  //N,W,S,E entraces
  terrain[westCol][0] = '!';
  terrain[eastCol][79] = '!';
  terrain[0][northRow] = '!';
  terrain[20][southRow] = '!';

  //initial paths outward from border on W,E sides. 4 deep
  for(int i = 1; i < 5; ++i) {
    terrain[westCol][0 + i] = '#';
    terrain[eastCol][79 - i] = '#';
  }

  //determine whether right path is HIGHER or LOWER than left path
  //if(westCol < eastCol){printf("Lower\n");} //+1
  //if(westCol > eastCol){printf("Higher\n");} //-1

  //build path if lines are equal (do sqaure thing)
  if(westCol == eastCol){ //0

    int diff = abs(westCol - 19);
    int smaller = ((westCol - 2) < diff) ? westCol : diff;

    //maxBound is how much path will go up or down. Random
    //catch possible floating point exception (mod by 0)
    int maxBound;
    if(smaller != 0) {
      maxBound = rand() % smaller + 1;
    }
    else {maxBound = 0;}

    terrain[westCol][rand() % 32 + 5] = '!';
    terrain[westCol][rand() % 35 + 40] = '?';

    int deviate = 0;
    for(int i = 5; i < 75; ++i) {

      //go on deviation path
      if(terrain[westCol][i] == '!') {

	      for(int j = 0; j <= maxBound; ++j) {
	        terrain[westCol + j][i] = '#';
	        pathLocations[location_index] = westCol + j;
	        location_index++;
	        pathLocations[location_index] = i;
	        location_index++;
	      }
	      deviate = maxBound;
      }
      //come off deviation path
      else if(terrain[westCol][i] == '?') {
	      terrain[westCol + deviate][i] = '#';
	      pathLocations[location_index] = westCol + deviate;
	      location_index++;
	      pathLocations[location_index] = i;
	      location_index++;

	      for(int j = maxBound; j >= 0; --j) {
	        terrain[westCol + j][i] = '#';
	        pathLocations[location_index] = westCol + j;
	        location_index++;
	        pathLocations[location_index] = i;
	        location_index++;
	      } 
	      deviate = 0;
      }
      else{
	      terrain[westCol + deviate][i] = '#';
	      pathLocations[location_index] = westCol + deviate;
	      location_index++;
	      pathLocations[location_index] = i;
	      location_index++;
      }
    }
    createPathVertical(northRow, southRow);
    generateCenterMart(pathLocations);
    return;
  }
  //NOW: If W,E exits aren't equal row
  //Must determine whether we need to traverse up/down
  int increment;
  if(westCol < eastCol){increment = 1;} //+1
  if(westCol > eastCol){increment = -1;} //-1

  int deviation = 0;
  int rnd;
  for (int i = 5; i < 75; ++i){

    if(westCol + deviation != eastCol) {

      rnd = rand() % 5;

      if(rnd == 0) {
	      terrain[westCol + deviation][i] = '#';
	      deviation = deviation + increment;
	      terrain[westCol + deviation][i] = '#';
	      pathLocations[location_index] = westCol + deviation;
	      location_index++;
	      pathLocations[location_index] = i;
	      location_index++;
      }
      else {
	      terrain[westCol + deviation][i] = '#';
	      pathLocations[location_index] = westCol + deviation;
	      location_index++;
	      pathLocations[location_index] = i;
	      location_index++;
      }

    }
    else{
      terrain[eastCol][i] = '#';
      pathLocations[location_index] = eastCol;
      location_index++;
      pathLocations[location_index] = i;
      location_index++;
    }
    //catch if upon last iteration paths are still unlinked.

    if(i == 74 && westCol + deviation != eastCol) {

      int j = increment;
      terrain[westCol + j + deviation][i] = '#';
      while (j + westCol + deviation != eastCol) {
	      j = j + increment;
	      terrain[westCol + j + deviation][i] = '#';
      }
    }
    
    
  }
  createPathVertical(northRow, southRow);
  generateCenterMart(pathLocations);
}

/*
  Sub-function of createPath. Forms the North and South Paths from their respective gates.
*/
void map::createPathVertical(int northRow, int southRow){
  bool connected = false;
  int i = 0;
  while(!connected) {
    if(terrain[i + 1][northRow] != '#'){
      terrain[i + 1][northRow] = '#';
      i++;
    }
    else{
      connected = true;
    }
  }
  connected = false;
  i = 20;
  while(!connected) {
    if(terrain[i - 1][southRow] != '#'){
      terrain[i - 1][southRow] = '#';
      --i;
    }
    else{
      connected = true;
    }
  }
}

/*
  Generates Pokemon Center and PokeMart.
*/
void map::generateCenterMart(int paths[156])
{
  int index;
  int col;
  int row;
  bool CenterPlaced = false;
  
  while(!CenterPlaced) {
    index = rand() % 155;

    if(index % 2 == 0) {
      col = paths[index];
      row = paths[index + 1];
      
      if (col > 2 && row < 78) {

	if(terrain[col - 1][row] != '#' && terrain[col - 2][row] != '#' &&
	   terrain[col - 1][row + 1] != '#' && terrain[col - 2][row + 1] != '#')
	  {
	    terrain[col - 1][row] = 'C';
	    terrain[col - 2][row] = 'C';
	    terrain[col - 1][row + 1] = 'C';
	    terrain[col -2][row + 1] = 'C';
	    CenterPlaced = true;
	    paths[col] = 10000;
	    paths[row] = 10000;
	  }
	
      }

      else if (col < 18 && row < 79) {
	if(terrain[col + 1][row - 1] != '#' && terrain[col + 1][row] != '#' &&
	   terrain[col + 2][row - 1] != '#' && terrain[col + 2][row] != '#')
	  {
	    terrain[col + 1][row] = 'C';
	    terrain[col + 2][row] = 'C';
	    terrain[col + 1][row - 1] = 'C';
	    terrain[col + 2][row - 1] = 'C';
	    CenterPlaced = true;
	    paths[col] = 10000;
	    paths[row] = 10000;
	  }
      }
    }
  }

  bool martPlaced = false;
  while(!martPlaced) {
    index = rand() % 155;

    if(index % 2 == 0) {
      col = paths[index];
      row = paths[index + 1];
      
      if (col > 2 && row < 78) {

	if(terrain[col - 1][row] != '#' && terrain[col - 2][row] != '#' &&
	   terrain[col - 1][row + 1] != '#' && terrain[col - 2][row + 1] != '#')
	  {
	    terrain[col - 1][row] = 'M';
	    terrain[col - 2][row] = 'M';
	    terrain[col - 1][row + 1] = 'M';
	    terrain[col -2][row + 1] = 'M';
	    martPlaced = true;
	    paths[col] = 10000;
	    paths[row] = 10000;
	  }
      }

      else if (col < 18 && row < 79) {
	if(terrain[col + 1][row - 1] != '#' && terrain[col + 1][row] != '#' &&
	   terrain[col + 2][row - 1] != '#' && terrain[col + 2][row] != '#')
	  {
	    terrain[col + 1][row] = 'M';
	    terrain[col + 2][row] = 'M';
	    terrain[col + 1][row - 1] = 'M';
	    terrain[col + 2][row - 1] = 'M';
	    martPlaced = true;
	    paths[col] = 10000;
	    paths[row] = 10000;
	  }

      }
    }
  }
}

/*
  Fills a Map's "paths" attribute with all valid paths on the map.
*/
void map::tallyPaths() 
{
  int arrInd = 0;
  coords C = coords(-1, -1);

  for (int i = 0; i < 100; ++i) {
    paths[i] = coords(-1,-1);
  }

  for(int i = 0; i < 21; ++i){
    for(int j = 0; j < 80; ++j){
      if (terrain[i][j] == '#') {
        C.setRow(i);
        C.setCol(j);
        paths[arrInd] = C;
        arrInd++;
      }
    }
  }
}

/**
 * Determines whether or not there is an NPC or PC occupying
 * a given space on the map.
 * Returns 1 (true) if occupied, 0 (false) if not
*/
int map::isSpaceTaken(int row, int col) {

  for(int i = 0; i < 9; ++i) {
    if (npcs[i].getRow() == row && npcs[i].getCol() == col) {
      return 1;
    }
  }
  if (pc.getRow() == row && pc.getCol() == col) {
    return 1;
  }

  return 0;
}

void map::initializeNpcs() 
{
  for (int i = 0; i < 9; ++i) {
    npcs[i] = nonPlayerCharacter(-1,-1,'X','X');
  }
}

void map::initializeNpcPokes()
{
  // for (int i = 0; i < 9; ++i) {
  //   npc_pokes[i] = new pokemon();
  //   pokemon p = generateRandomPokemon();
  //   npc_pokes[i]->name = p.name;
  //   npc_pokes[i]->level = p.level;
  //   npc_pokes[i]->move_one = p.move_one;
  //   npc_pokes[i]->move_two = p.move_two;
  // }

  for (int i = 0; i < 9; ++i) { // For every trainer
    
    npcs[i].setPokemon(generateRandomPokemon(), 0); // First pokemon guaranteed

    for (int j = 1; j < 6; ++j)
    {
      if (rand() % 10 <= 5) { // 60 % chance of adding +1 pokemon until chance doesn't land
        npcs[i].setPokemon(generateRandomPokemon(), j);
      }
      else {
        break; 
      }
    }

  }
}

/**
 * Places NPC at random spot on the map when first
 * generated according to what is allowed 
*/
void map::placeNpc(int index) {

  while (1) {
    int rRow = (rand() % 19) + 1; // 1 - 19, excludes borders
    int rCol = (rand() % 78) + 1; // 1 - 78, excludes borders
  

  //cant place NPC on ^, %, ~ (=,! already excluded) or 
  //cell already occupied by NPC
  if (terrain[rRow][rCol] != '^' && terrain[rRow][rCol] != '%' && 
      terrain[rRow][rCol] != '~' && !isSpaceTaken(rRow, rCol)) {

        npcs[index].setRow(rRow);
        npcs[index].setCol(rCol);
        return;
      }
  }
}

/**
 * Spawns NPCs at random but appropriate locations on the map.
 * NPCs are hard coded/guaranteed for now. Will likely be
 * randomized in the future.
*/
void map::spawnNpcs() {
  initializeNpcs();
  //Hikers: 1, Rivals: 2, Pacers: 1, Wanderers: 2 , Sentries: 1, Explorers: 2 = 9

  npcs[0].setType('h');
  npcs[1].setType('r');
  npcs[2].setType('r');

  npcs[3].setType('p');
  giveNewDirection(3);

  npcs[4].setType('w');
  giveNewDirection(4);

  npcs[5].setType('w');
  giveNewDirection(5);

  npcs[6].setType('s');
  giveNewDirection(6);

  npcs[7].setType('e');
  giveNewDirection(7);

  npcs[8].setType('e');
  giveNewDirection(8);

  for (int i = 0; i < 9; ++i) {
    placeNpc(i);
  }

}

void map::giveNewDirection(int index) {
  int newDir = rand() % 8; // 0-7

  if (newDir == 0) {npcs[index].setDirection('N');}
  else if (newDir == 1) {npcs[index].setDirection('n');}
  else if (newDir == 2) {npcs[index].setDirection('E');}
  else if (newDir == 3) {npcs[index].setDirection('e');}
  else if (newDir == 4) {npcs[index].setDirection('S');}
  else if (newDir == 5) {npcs[index].setDirection('s');}
  else if (newDir == 6) {npcs[index].setDirection('W');}
  else if (newDir == 7) {npcs[index].setDirection('w');}
}

double map::moveNpc(int ind) {

  int nRow = npcs[ind].getRow();
  int nCol = npcs[ind].getCol();
  int cRow, cCol;
  float lowest = INFINITY;
  // Hiker
  if (npcs[ind].getType() == 'h') {
    for (int i = -1; i < 2; ++i) {
      for (int j = -1; j < 2; ++j) {
        cRow = npcs[ind].getRow() + i;
        cCol = npcs[ind].getCol() + j;

        if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79
        && !isSpaceTaken(cRow, cCol)) {
          if (dist_hiker[cRow][cCol] < lowest) {
            nRow = cRow;
            nCol = cCol;
            lowest = dist_hiker[cRow][cCol];
          }
        }
      }
    }
    npcs[ind].setRow(nRow);
    npcs[ind].setCol(nCol);
    if (lowest == INFINITY) {return 100;}
    return lowest + 10;
  }
  // Rival
  else if (npcs[ind].getType() == 'r') {
    for (int i = -1; i < 2; ++i) {
      for (int j = -1; j < 2; ++j) {
        cRow = npcs[ind].getRow() + i;
        cCol = npcs[ind].getCol() + j;

        if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79
        && !isSpaceTaken(cRow, cCol)) {
          if(dist_rival[cRow][cCol] < lowest) {
            nRow = cRow;
            nCol = cCol;
            lowest = dist_rival[cRow][cCol];
          }
        }
      }
    }
    npcs[ind].setRow(nRow);
    npcs[ind].setCol(nCol);
    if (lowest == INFINITY) {return 100;}
    return lowest + 10;
  }
  // Pacer
  else if (npcs[ind].getType() == 'p') {

    if(npcs[ind].getDirection() == 'N') {
      cRow = npcs[ind].getRow() - 1;
      cCol = npcs[ind].getCol();

      if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79
      && terrain[cRow][cCol] != '!' && terrain[cRow][cCol] != '='
      && terrain[cRow][cCol] != '%' && terrain[cRow][cCol] != '~'
      && !isSpaceTaken(cRow, cCol)) {

        npcs[ind].setRow(cRow);
        npcs[ind].setCol(cCol);
        return 10;
      }
      else {
        npcs[ind].setDirection('S');
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'n') {
      cRow = npcs[ind].getRow() - 1;
      cCol = npcs[ind].getCol() + 1;

      if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79
      && terrain[cRow][cCol] != '!' && terrain[cRow][cCol] != '='
      && terrain[cRow][cCol] != '%' && terrain[cRow][cCol] != '~'
      && !isSpaceTaken(cRow, cCol)) {

        npcs[ind].setRow(cRow);
        npcs[ind].setCol(cCol);
        return 10;
      }
      else {
        npcs[ind].setDirection('s');
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'E') {
      cRow = npcs[ind].getRow();
      cCol = npcs[ind].getCol() + 1;

      if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79
      && terrain[cRow][cCol] != '!' && terrain[cRow][cCol] != '='
      && terrain[cRow][cCol] != '%' && terrain[cRow][cCol] != '~'
      && !isSpaceTaken(cRow, cCol)) {

        npcs[ind].setRow(cRow);
        npcs[ind].setCol(cCol);
        return 10;
      }
      else {
        npcs[ind].setDirection('W');
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'e') {
      cRow = npcs[ind].getRow() + 1;
      cCol = npcs[ind].getCol() + 1;

      if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79
      && terrain[cRow][cCol] != '!' && terrain[cRow][cCol] != '='
      && terrain[cRow][cCol] != '%' && terrain[cRow][cCol] != '~'
      && !isSpaceTaken(cRow, cCol)) {

        npcs[ind].setRow(cRow);
        npcs[ind].setCol(cCol);
        return 10;
      }
      else {
        npcs[ind].setDirection('w');
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'S') {
      cRow = npcs[ind].getRow() + 1;
      cCol = npcs[ind].getCol();

      if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79
      && terrain[cRow][cCol] != '!' && terrain[cRow][cCol] != '='
      && terrain[cRow][cCol] != '%' && terrain[cRow][cCol] != '~'
      && !isSpaceTaken(cRow, cCol)) {

        npcs[ind].setRow(cRow);
        npcs[ind].setCol(cCol);
        return 10;
      }
      else {
        npcs[ind].setDirection('N');
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 's') {
      cRow = npcs[ind].getRow() + 1;
      cCol = npcs[ind].getCol() - 1;

      if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79
      && terrain[cRow][cCol] != '!' && terrain[cRow][cCol] != '='
      && terrain[cRow][cCol] != '%' && terrain[cRow][cCol] != '~'
      && !isSpaceTaken(cRow, cCol)) {

        npcs[ind].setRow(cRow);
        npcs[ind].setCol(cCol);
        return 10;
      }
      else {
        npcs[ind].setDirection('n');
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'W') {
      cRow = npcs[ind].getRow();
      cCol = npcs[ind].getCol() - 1;

      if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79
      && terrain[cRow][cCol] != '!' && terrain[cRow][cCol] != '='
      && terrain[cRow][cCol] != '%' && terrain[cRow][cCol] != '~'
      && !isSpaceTaken(cRow, cCol)) {

        npcs[ind].setRow(cRow);
        npcs[ind].setCol(cCol);
        return 10;
      }
      else {
        npcs[ind].setDirection('E');
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'w') {
      cRow = npcs[ind].getRow() - 1;
      cCol = npcs[ind].getCol() - 1;

      if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79
      && terrain[cRow][cCol] != '!' && terrain[cRow][cCol] != '='
      && terrain[cRow][cCol] != '%' && terrain[cRow][cCol] != '~'
      && !isSpaceTaken(cRow, cCol)) {

        npcs[ind].setRow(cRow);
        npcs[ind].setCol(cCol);
        return 10;
      }
      else {
        npcs[ind].setDirection('e');
        return 10;
      }
    }
  }
  // Wanderer
  else if (npcs[ind].getType() == 'w') {
    char cTerra = terrain[npcs[ind].getRow()][npcs[ind].getCol()];

    if (npcs[ind].getDirection() == 'N') {
      if (cTerra == terrain[npcs[ind].getRow() - 1][npcs[ind].getCol()]
      && !isSpaceTaken(npcs[ind].getRow() - 1, npcs[ind].getCol())) {
        npcs[ind].setRow(npcs[ind].getRow() - 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'n') {
      if (cTerra == terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() + 1]
      && !isSpaceTaken(npcs[ind].getRow() - 1, npcs[ind].getCol() + 1)) {
        npcs[ind].setRow(npcs[ind].getRow() - 1);
        npcs[ind].setCol(npcs[ind].getCol() + 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'E') {
      if (cTerra == terrain[npcs[ind].getRow()][npcs[ind].getCol() + 1]
      && !isSpaceTaken(npcs[ind].getRow(), npcs[ind].getCol() + 1)) {
        npcs[ind].setCol(npcs[ind].getCol() + 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'e') {
      if (cTerra == terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() + 1]
      && !isSpaceTaken(npcs[ind].getRow() + 1, npcs[ind].getCol() + 1)) {
        npcs[ind].setRow(npcs[ind].getRow() + 1);
        npcs[ind].setCol(npcs[ind].getCol() + 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'S') {
      if (cTerra == terrain[npcs[ind].getRow() + 1][npcs[ind].getCol()]
      && !isSpaceTaken(npcs[ind].getRow() + 1, npcs[ind].getCol())) {
        npcs[ind].setRow(npcs[ind].getRow() + 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 's') {
      if (cTerra == terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() - 1]
      && !isSpaceTaken(npcs[ind].getRow() + 1, npcs[ind].getCol() - 1)) {
        npcs[ind].setRow(npcs[ind].getRow() + 1);
        npcs[ind].setCol(npcs[ind].getCol() - 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'W') {
      if (cTerra == terrain[npcs[ind].getRow()][npcs[ind].getCol() - 1]
      && !isSpaceTaken(npcs[ind].getRow(), npcs[ind].getCol() - 1)) {
        npcs[ind].setCol(npcs[ind].getCol() - 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'w') {
      if (cTerra == terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() - 1]
      && !isSpaceTaken(npcs[ind].getRow() - 1, npcs[ind].getCol() - 1)) {
        npcs[ind].setRow(npcs[ind].getRow() - 1);
        npcs[ind].setCol(npcs[ind].getCol() - 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
  }
  // Sentry
  else if (npcs[ind].getType() == 's') {
    return 100;
  }
  // Explorer
  else if (npcs[ind].getType() == 'e') {

    if (npcs[ind].getDirection() == 'N') {
      if (terrain[npcs[ind].getRow() - 1][npcs[ind].getCol()] != '~'
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol()] != '='
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol()] != '!'
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol()] != '%'
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol()] != '^'
      && !isSpaceTaken(npcs[ind].getRow() - 1, npcs[ind].getCol())) {

        npcs[ind].setRow(npcs[ind].getRow() - 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'n') {
      if (terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() - 1] != '~'
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() - 1] != '='
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() - 1] != '!'
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() - 1] != '%'
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() - 1] != '^'
      && !isSpaceTaken(npcs[ind].getRow() - 1, npcs[ind].getCol() - 1)) {

        npcs[ind].setRow(npcs[ind].getRow() - 1);
        npcs[ind].setCol(npcs[ind].getCol() - 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'E') {
      if (terrain[npcs[ind].getRow()][npcs[ind].getCol() + 1] != '~'
      && terrain[npcs[ind].getRow()][npcs[ind].getCol() + 1] != '='
      && terrain[npcs[ind].getRow()][npcs[ind].getCol() + 1] != '!'
      && terrain[npcs[ind].getRow()][npcs[ind].getCol() + 1] != '%'
      && terrain[npcs[ind].getRow()][npcs[ind].getCol() + 1] != '^'
      && !isSpaceTaken(npcs[ind].getRow(), npcs[ind].getCol() + 1)) {

        npcs[ind].setCol(npcs[ind].getCol() + 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'e') {
      if (terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() + 1] != '~'
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() + 1] != '='
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() + 1] != '!'
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() + 1] != '%'
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() + 1] != '^'
      && !isSpaceTaken(npcs[ind].getRow() + 1, npcs[ind].getCol() + 1)) {

        npcs[ind].setRow(npcs[ind].getRow() + 1);
        npcs[ind].setCol(npcs[ind].getCol() + 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'S') {
      if (terrain[npcs[ind].getRow() + 1][npcs[ind].getCol()] != '~'
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol()] != '='
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol()] != '!'
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol()] != '%'
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol()] != '^'
      && !isSpaceTaken(npcs[ind].getRow() + 1, npcs[ind].getCol())) {

        npcs[ind].setRow(npcs[ind].getRow() + 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 's') {
      if (terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() - 1] != '~'
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() - 1] != '='
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() - 1] != '!'
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() - 1] != '%'
      && terrain[npcs[ind].getRow() + 1][npcs[ind].getCol() - 1] != '^'
      && !isSpaceTaken(npcs[ind].getRow() + 1, npcs[ind].getCol() - 1)) {

        npcs[ind].setRow(npcs[ind].getRow() + 1);
        npcs[ind].setCol(npcs[ind].getCol() - 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'W') {
      if (terrain[npcs[ind].getRow()][npcs[ind].getCol() - 1] != '~'
      && terrain[npcs[ind].getRow()][npcs[ind].getCol() - 1] != '='
      && terrain[npcs[ind].getRow()][npcs[ind].getCol() - 1] != '!'
      && terrain[npcs[ind].getRow()][npcs[ind].getCol() - 1] != '%'
      && terrain[npcs[ind].getRow()][npcs[ind].getCol() - 1] != '^'
      && !isSpaceTaken(npcs[ind].getRow(), npcs[ind].getCol() - 1)) {

        npcs[ind].setCol(npcs[ind].getCol() - 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
    else if (npcs[ind].getDirection() == 'w') {
      if (terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() - 1] != '~'
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() - 1] != '='
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() - 1] != '!'
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() - 1] != '%'
      && terrain[npcs[ind].getRow() - 1][npcs[ind].getCol() - 1] != '^'
      && !isSpaceTaken(npcs[ind].getRow() - 1, npcs[ind].getCol() - 1)) {

        npcs[ind].setRow(npcs[ind].getRow() - 1);
        npcs[ind].setCol(npcs[ind].getCol() - 1);
        return 10;
      }
      else {
        giveNewDirection(ind);
        return 10;
      }
    }
  }
  return 100;
}


/*
  Returns 0 if PC not allowed on terrain, returns 1 if PC is allowed.
*/
int map::pcAllowedSpace(int row, int col) {

  pcData *pcd;
  pcd = pcData::getInstance();


  if (terrain[row][col] == '=') {return 0;}
  if (terrain[row][col] == '^') {return 0;}
  if (terrain[row][col] == '~') {return pcd->canWalkOnWater;}
  if(terrain[row][col] == '%') {return 0;}

  return 1;
}

/*
  Spawns the PC on a random Path at Map(0,0)
*/
void map::spawnPc(int row, int col) {

  if (row == -1) {
    int index = 0;
    int is_spawned = 0;
    while (!is_spawned) {
      index = rand() % 100;
      if (paths[index].getRow() != -1) {
        pc.setRow(paths[index].getRow());
        pc.setCol(paths[index].getCol());
      }
      is_spawned = 1;
    }
  } else {
    pc.setRow(row);
    pc.setCol(col);
  }

}

void map::movePc(char dir) {
  int newRow = pc.getRow();
  int newCol = pc.getCol();

  if (dir == 'N') {
    newRow--;
  }
  else if (dir == 'n') {
    newRow--;
    newCol++;
  }
  else if (dir == 'E') {
    newCol++;
  }
  else if (dir == 'e') {
    newRow++;
    newCol++;
  }
  else if (dir == 'S') {
    newRow++;
  }
  else if (dir == 's') {
    newRow++;
    newCol--;
  }
  else if (dir == 'W') {
    newCol--;
  }
  else if (dir == 'w') {
    newRow--;
    newCol--;
  }

  if (!isSpaceTaken(newRow, newCol) && pcAllowedSpace(newRow, newCol)) {
    pc.setRow(newRow);
    pc.setCol(newCol);
  }
}
/**
 * Returns 0 if PC is not on a "!" gate. Returns true if PC is on a gate.
 * Used to detect movement between maps.
*/
int map::pcOnGate() {
  if(terrain[pc.getRow()][pc.getCol()] == '!') {
    return 1;
  }
  return 0;
}

/**
 * 1 - North Gate
 * 2 - East Gate
 * 3 - South Gate
 * 4 - West Gate
*/
int map::gatePcIsOn() {
  if (pc.getRow() == col_east_gate) {
    return 2;
  }
  else if (pc.getRow() == col_west_gate) {
    return 4;
  }
  else if (pc.getCol() == row_north_gate) {
    return 1;
  }
  else if (pc.getCol() == row_south_gate) {
    return 3;
  }
  return 0;
}

void map::initializeDistanceMaps() {
  for (int i = 0; i < 21; ++i) {
    for (int j = 0; j < 80; ++j) {
      dist_hiker[i][j] = INFINITY;
      dist_rival[i][j] = INFINITY;    
    }
  }
}

float map::addDistance(char npc, char terrain) {
  if (npc == 'h') { //Hiker
    if (terrain == '=') {return INFINITY;}
    else if (terrain == '^') {return INFINITY;}
    else if (terrain == '#') {return 10;}
    else if (terrain == 'M') {return 50;}
    else if (terrain == 'C') {return 50;}
    else if (terrain == ':') {return 15;}
    else if (terrain == '.') {return 10;}
    else if (terrain == '%') {return 15;}
    else if (terrain == '~') {return INFINITY;}
    else if (terrain == '!') {return INFINITY;}
    else {return INFINITY;}
  }
  else if (npc == 'r') { //Rival
    if (terrain == '=') {return INFINITY;}
    else if (terrain == '^') {return INFINITY;}
    else if (terrain == '#') {return 10;}
    else if (terrain == 'M') {return 50;}
    else if (terrain == 'C') {return 50;}
    else if (terrain == ':') {return 20;}
    else if (terrain == '.') {return 10;}
    else if (terrain == '%') {return INFINITY;}
    else if (terrain == '~') {return INFINITY;}
    else if (terrain == '!') {return INFINITY;}
    else {return INFINITY;}

  }
  else {return INFINITY;}
}

void map::generateDistanceMaps() {
  initializeDistanceMaps();

  int row, col, cRow, cCol;
  float tempDist;
  dist_hiker[pc.getRow()][pc.getCol()] = 0;
  dist_rival[pc.getRow()][pc.getCol()] = 0;

  /* START HIKER DISTANCE MAP*/
  priorityQueue H;
  queue_init(&H);
  queue_insert(&H, pc.getRow(), pc.getCol(), 0);

  for (int i = 0; i < 21; ++i) {
    for (int j = 0; j < 80; ++j) {
      if (i != pc.getRow() || j != pc.getCol()) {
        queue_insert(&H, i, j, INFINITY);
      }
    }
  }

  while (!is_Empty(&H)) {
    queue_remove(&H, &row, &col);

    for (int i = -1; i < 2; ++i) {
      for (int j = -1; j < 2; ++j) {
        cRow = row + i;
        cCol = col + j;

        if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79) {
          tempDist = dist_hiker[row][col] + addDistance('h', terrain[cRow][cCol]);

          if (tempDist < dist_hiker[cRow][cCol]) {
            dist_hiker[cRow][cCol] = tempDist;
            queue_decrease_priority(&H, cRow, cCol, tempDist);
          }
        }
      }
    }
  }
  queue_destroy(&H);
  /* END HIKER DISTANCE MAP*/
  /* START RIVAL DISTANCE MAP*/
  priorityQueue R;
  queue_init(&R);
  queue_insert(&R, pc.getRow(), pc.getCol(), 0);

  for (int i = 0; i < 21; ++i) {
    for (int j = 0; j < 80; ++j) {
      if (i != pc.getRow() || j != pc.getCol()) {
        queue_insert(&R, i, j, INFINITY);
      }
    }
  }

  while (!is_Empty(&R)) {
    queue_remove(&R, &row, &col);

    for (int i = -1; i < 2; ++i) {
      for (int j = -1; j < 2; ++j) {
        cRow = row + i;
        cCol = col + j;

        if (cRow >= 0 && cRow <= 20 && cCol >= 0 && cCol <= 79) {
          tempDist = dist_rival[row][col] + addDistance('r', terrain[cRow][cCol]);

          if (tempDist < dist_rival[cRow][cCol]) {
            dist_rival[cRow][cCol] = tempDist;
            queue_decrease_priority(&R, cRow, cCol, tempDist);
          }
        }
      }
    }
  }
  queue_destroy(&R);
}

void map::printDistanceMaps() {
  int val;
  for (int i = 0; i < 21; ++i) {
    for (int j = 0; j < 80; ++j) {
      val = (int)fmod(dist_hiker[i][j], 100);
      if (dist_hiker[i][j] == INFINITY) {
        printw("   ");
      }
      else if (val == 0) {printw("00 ");}
      else if (val == 5) {printw("05 ");}
      else {
        printw("%d ", val);
      }
    }
    printw("\n");
  }
  printf("\n");

  for (int i = 0; i < 21; ++i) {
    for (int j = 0; j < 80; ++j) {
      val = (int)fmod(dist_rival[i][j], 100);
      if (dist_rival[i][j] == INFINITY) {
        printw("   ");
      }
      else if (val == 0) {printw("00 ");}
      else if (val == 5) {printw("05 ");}
      else {
        printw("%d ", val);
      }
    }
    printw("\n");
  }
  printf("\n");
}

void map::enterPokemart() {
  
  pcData *pcd;
  pcd = pcData::getInstance();
  pcd->restoreItems();
  
  erase();
  printw("\nPokeMart\n");
  printw("Items restored!\n");
  printw("Potion x3\nRevive x3\nPokeball x3\n");
  printw("\nPress \"<\" to exit.\n");
  char ch = getch();
  while (ch != '<') {
    ch = getch();
  }
}

void map::enterPokecenter() {

  pcData *pcd;
  pcd = pcData::getInstance();
  pcd->healAllPokemon();

  erase();
  printw("\nPokemon Center\n");
  printw("All pokemon HP restored!\n");
  printw("\nPress \"<\" to exit.\n");

  char ch = getch();
  while (ch != '<') {
    ch = getch();
  }
}

void map::enterBuilding() {
  if (terrain[pc.getRow()][pc.getCol()] == 'M') {
    enterPokemart();
  }
  else if (terrain[pc.getRow()][pc.getCol()] == 'C') {
    enterPokecenter();
  }
}

/**
 * Set up game queue for the map.
*/
void map::initializeQueue() {
  queue_init(&Q);

  // insert NPCs
  queue_insert(&Q, pc.getRow(), pc.getCol(), 0);
  for (int i = 0; i < 9; ++i) {
    queue_insert(&Q, npcs[i].getRow(), npcs[i].getCol(), 0);
  }
}

/**
 * Executes NPC movement until PC has a turn.
*/
void map::doATurn(char c) {

  pcData *pcd;
  pcd = pcData::getInstance();

  doTheInput(c);
  generateDistanceMaps();
  printMap();

  for (int i = 0; i < 9; ++i)
  {
    if (!npcs[i].is_defeated && pcd->npcsCanMove) {moveNpc(i);}
  }

}

/**
 * Matches user-entered char to appropriate action.
*/
void map::doTheInput(char ch) {

  if ((ch == '7') || (ch == 'y')) {
    movePc('w');
  }
  else if ((ch == '8') || (ch == 'k')) {
    movePc('N');
  }
  else if ((ch == '9') || (ch == 'u')) {
    movePc('n');
  }
  else if ((ch == '6') || (ch == 'l')) {
    movePc('E');
  }
  else if ((ch == '3') || (ch == 'n')) {
    movePc('e');
  }
  else if ((ch == '2') || (ch == 'j')) {
    movePc('S');
  }
  else if ((ch == '1')) {
    movePc('s');
  }
  else if ((ch == '4') || (ch == 'h')) {
    movePc('W');
  }
  else if ((ch == '5') || (ch == '.') || (ch == ' ')) {
    //do nothing
  }
  else if (ch == '>') {
    enterBuilding();
  }
  else if (ch == 'i') {
    viewPokemon();
  }
  else if (ch == 'b') {
    viewBag();
  }
  else if (ch == 'o') {
    if (canFishHere()) {doFishing();}
    else {cantFishHere();}
  }
  else if (ch == 'Q') {
    //ends game
    endwin();
    exit(0);
  }
  else if (ch == '+') {
    doHack();
  }
  refresh();
}

/**
 * Master method for generating terrain. Includes all sub-methods
*/
void map::generate(int row, int col)
{
  initializeTerrain();
  createRegions();
  patchEmptyCells();
  createBorders();
  createPath();

  tallyPaths();
  spawnPc(row, col);
  spawnNpcs();

  generateDistanceMaps();
  initializeQueue();
  initializeNpcPokes();

}

pokemon map::generateRandomPokemon()
{
  pokemon *p = new pokemon();
  pokemonData *d;
  d = pokemonData::getInstance();

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distribution(0, 1092);
  int r = distribution(gen);
  *p = d->getPokemon(r);
  p->level = calcManhattan(); // Change to manhattan
  p->is_dead = 0;

  /*Give moves*/
  giveMoves(p);

  /*Give IVs and other stats*/
  giveIVs(p);
  giveOthers(p);

  return *p;
}

pokemon map::generateWailord()
{
  pokemon *p = new pokemon();
  p->level = calcManhattan() + 10; // 10 levels higher than wild encounter
  p->id = 321;
  p->name = "Wailord - BOSS";
  p->is_dead = 0;
  giveMoves(p);
  giveIVs(p);
  giveOthers(p);

  return *p;
}

pokemon map::generateZekrom()
{
  pokemon *p = new pokemon();
  p->level = 50;
  p->id = 644;
  p->name = "Zekrom";
  p->is_dead = 0;
  giveMoves(p);
  giveIVs(p);
  giveOthers(p);

  return *p;
}

pokemon map::generateKraken()
{
  moveData *attacks;
  attacks = moveData::getInstance();

  pokemon *p = new pokemon();
  p->level = 100;
  p->id = 687; // Malamar looks like a Kraken
  p->name = "THE GREAT KRAKEN OF THE SEA";
  p->is_dead = 0;

  p->move_one.id = 20232023;
  p->move_one.identifier = "SUPREME WRATH OF THE OCEAN";
  p->move_one.accuracy = 90;
  p->move_one.type_id = 17; // Dark type
  p->move_one.power = 1999;
  p->move_one.priority = 1;

  p->move_two = attacks->getMove(174); // Flail

  giveIVs(p);
  giveOthers(p);

  return *p;
}

void map::giveMoves(pokemon *p)
{
  pokemonMoveData *pokeMoves;
  pokeMoves = pokemonMoveData::getInstance();

  moveData *attacks;
  attacks = moveData::getInstance();

  std::vector<int> levelUp_attacks; // Possible move IDs at any level
  std::vector<int> levelUp_levels; // Level move is learned at
  std::vector<int> possible_attacks; // Possible move IDs at current level
  //Find all moves possible at any level
  for (int i = 0; i < 528238; ++i) //range should be 528238
  {
    if ((pokeMoves->getPokemonMove(i).pokemon_id == p->id)
    && (pokeMoves->getPokemonMove(i).pokemon_move_method_id == 1)) { // If pokemon can learn the level up move
      levelUp_attacks.push_back(pokeMoves->getPokemonMove(i).move_id); // Add move to vector
      levelUp_levels.push_back(pokeMoves->getPokemonMove(i).level);
    }
    else if (pokeMoves->getPokemonMove(i).pokemon_id > p->id) {
      break;
    }
  }

  //Remove duplicate attacks 
  std::sort(levelUp_attacks.begin(), levelUp_attacks.end()); // Sort vector
  auto uniqueEnd = std::unique(levelUp_attacks.begin(), levelUp_attacks.end());
  levelUp_attacks.erase(uniqueEnd, levelUp_attacks.end());


  int success = 0;
  while (!success) {

    possible_attacks.clear();

    // Find all moves possible at Pokemon's current level
    for (int i = 0; i < static_cast<int>(levelUp_attacks.size()); ++i) {
      if (p->level >= levelUp_levels[i]) {
        possible_attacks.push_back(levelUp_attacks[i]);
      }
    }

    // If pokemon can learn at least one move:
    if (static_cast<int>(possible_attacks.size()) > 0) {
      success = 1;
    }
    else { // If pokemon can't learn any moves at current level
      p->level = p->level + 1; // Increase level by one and try again
    }
  }
  
  // If pokemon only has 1 move possible
  if (static_cast<int>(possible_attacks.size()) == 1) {
    p->move_one = attacks->getMove(possible_attacks[0] - 1);
    p->move_two.identifier = "---";
    return;
  }

  //If pokemon only has 2 moves possible
  if (static_cast<int>(possible_attacks.size()) == 2) {
    p->move_one = attacks->getMove(possible_attacks[0] - 1);
    p->move_two = attacks->getMove(possible_attacks[1] - 1);
    return;
  }

  //If pokemon has over 2 moves, pick at random.
  if (static_cast<int>(possible_attacks.size()) > 2) {
    int move_1 = rand() % static_cast<int>(possible_attacks.size());
    int move_2 = rand() % static_cast<int>(possible_attacks.size());

    // Cannot be the same move
    while (move_1 == move_2) {
      move_2 = rand() % static_cast<int>(possible_attacks.size());
    }

    p->move_one = attacks->getMove(possible_attacks[move_1] - 1);
    p->move_two = attacks->getMove(possible_attacks[move_2] - 1);
  }
  
}

/**
 * Assigns correct IVs to pokemon
*/
void map::giveIVs(pokemon *p)
{
  pokemonStatData *pokeStats;
  pokeStats = pokemonStatData::getInstance();

  for (int i = 0; i < 6552; ++i) {
    if (pokeStats->getPokemonStat(i).pokemon_id == p->id) { // If pokemon's id matches first IV
      p->hp = pokeStats->getPokemonStat(i).base_stat;
      p->attack = pokeStats->getPokemonStat(i + 1).base_stat;
      p->defense = pokeStats->getPokemonStat(i + 2).base_stat;
      p->spAtt = pokeStats->getPokemonStat(i + 3).base_stat;
      p->spDef = pokeStats->getPokemonStat(i + 4).base_stat;
      p->speed = pokeStats->getPokemonStat(i + 5).base_stat;
      return;
    }
  }
}

/**
 * Assigns non-IV stats to pokemon
*/
void map::giveOthers(pokemon *p)
{
  p->gender = rand() % 2;
  p->hp_max = (((p->hp * 2) * p->level) / 100) + p->level + 10;
  p->hp_cur = p->hp_max;

  //Give types (like water, grass, etc)
  pokemonTypeData *pokeTypes;
  pokeTypes = pokemonTypeData::getInstance();

  p->type_one = -1;
  p->type_two = -1;

  for (int i = 0; i < 1676; ++i) {

    if (pokeTypes->getPokemonType(i).pokemon_id == p->id) {
      if (p->type_one == -1) {
        p->type_one = pokeTypes->getPokemonType(i).type_id;
      }
      else if (p->type_two == -1) {
        p->type_two = pokeTypes->getPokemonType(i).type_id;
        break;
      }
    }
    if (pokeTypes->getPokemonType(i).pokemon_id > p->id) {break;}
  }
}

void map::printRandomPokemon()
{
  pokemon *p = new pokemon();
  // pokemonData *d;
  // d = pokemonData::getInstance();

  // std::random_device rd;
  // std::mt19937 gen(rd());
  // std::uniform_int_distribution<> distribution(0, 1092);
  // int r = distribution(gen);

  *p = generateRandomPokemon(); //d->getPokemon(r);
  p->level = calcManhattan(); // Change to manhattan

  printw("A wild pokemon appears! %s, Lvl %d | Moves: %s, %s\n", p->name.c_str(), p->level, p->move_one.identifier.c_str(), p->move_two.identifier.c_str());
  delete p;
}

int map::isTrainerAdjacent(char dir)
{
  if (dir == 'N') {
    if (isSpaceTaken(pc.getRow() - 1, pc.getCol())) {
      for (int x = 0; x < 9; ++x) {
        if ((npcs[x].getRow() == pc.getRow()-1) && (npcs[x].getCol() == pc.getCol()) && !npcs[x].is_defeated) {
          printw("Trainer's pokemon: %s, Lvl %d | Moves: %s, %s\n", npcs[x].npcPokemon[0].name.c_str(), npcs[x].npcPokemon[0].level,npcs[x].npcPokemon[0].move_one.identifier.c_str(), npcs[x].npcPokemon[0].move_two.identifier.c_str());
          doTrainerBattle(x);
          return 1;
        }
      }
    }
  }
  else if (dir == 'E') {
    if (isSpaceTaken(pc.getRow(), pc.getCol() + 1)) {
      for (int x = 0; x < 9; ++x) {
        if ((npcs[x].getRow() == pc.getRow()) && (npcs[x].getCol() == pc.getCol()+1) && !npcs[x].is_defeated) {
          printw("Trainer's pokemon: %s, Lvl %d | Moves: %s, %s\n", npcs[x].npcPokemon[0].name.c_str(), npcs[x].npcPokemon[0].level,npcs[x].npcPokemon[0].move_one.identifier.c_str(), npcs[x].npcPokemon[0].move_two.identifier.c_str());
          doTrainerBattle(x);
          return 1;
        }
      }
    }
  }
  else if (dir == 'S') {
    if (isSpaceTaken(pc.getRow() + 1, pc.getCol())) {
      for (int x = 0; x < 9; ++x) {
        if ((npcs[x].getRow() == pc.getRow() + 1) && (npcs[x].getCol() == pc.getCol()) && !npcs[x].is_defeated) {
          printw("Trainer's pokemon: %s, Lvl %d | Moves: %s, %s\n", npcs[x].npcPokemon[0].name.c_str(), npcs[x].npcPokemon[0].level,npcs[x].npcPokemon[0].move_one.identifier.c_str(), npcs[x].npcPokemon[0].move_two.identifier.c_str());
          doTrainerBattle(x);
          return 1;
        }
      }
    }
  }
  else if (dir == 'W') {
    if (isSpaceTaken(pc.getRow(), pc.getCol() - 1)) {
      for (int x = 0; x < 9; ++x) {
        if ((npcs[x].getRow() == pc.getRow()) && (npcs[x].getCol() == pc.getCol()-1) && !npcs[x].is_defeated) {
          printw("Trainer's pokemon: %s, Lvl %d | Moves: %s, %s\n", npcs[x].npcPokemon[0].name.c_str(), npcs[x].npcPokemon[0].level,npcs[x].npcPokemon[0].move_one.identifier.c_str(), npcs[x].npcPokemon[0].move_two.identifier.c_str());
          doTrainerBattle(x);
          return 1;
        }
      }
    }
  }

  return 0;
}

int map::isWildEncounter() {

  if(terrain[pc.getRow()][pc.getCol()] != ':') {return 0;}

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distribution(0, 9);
  int r = distribution(gen);

  if (r == 2) {
    printRandomPokemon();
    doWildEncounter();
    return 1;
  }
  return 0;
}

/**
 * Prints Pokemon above console if:
 * 1. The PC encountered a wild pokemon in the Tall Grass
 * 2. The PC is adjacent to a trainer, causing a pokemon battle
 * If neither of these conditions are met, a blank line is printed.
 * Trainer battles take priority over wild encounters.
*/
void map::isThereAPokemon() {

  if(isTrainerAdjacent('N')) {return;}
  if(isTrainerAdjacent('E')) {return;}
  if(isTrainerAdjacent('S')) {return;}
  if(isTrainerAdjacent('W')) {return;}

  if(isWildEncounter()) {return;}
}

void map::printPokemonSelection() {
  erase();
  attron(COLOR_PAIR(1));
  printw("\nPoke327 v.1.10   |   Connor Ferris\n\n");
  attroff(COLOR_PAIR(1));

  attron(COLOR_PAIR(9));
  printw("Before you can begin, you must select a pokemon to start your journey with.\n\n");
  attroff(COLOR_PAIR(9));  

  pokemon first_choice = generateRandomPokemon();
  pokemon second_choice = generateRandomPokemon();
  pokemon third_choice = generateRandomPokemon();
  printw("1. %s, Lvl %d\n", first_choice.name.c_str(), first_choice.level);
  printw("2. %s, Lvl %d\n", second_choice.name.c_str(), second_choice.level);
  printw("3. %s, Lvl %d\n", third_choice.name.c_str(), third_choice.level); 

  attron(COLOR_PAIR(9));
  printw("\nWe understand these pokemon may not be your first choice. But you must start somewhere.\n");  
  attroff(COLOR_PAIR(9));  

  printw("\nPress 1,2, or 3 to add the corresponding pokemon to your team\n");

  pcData *pcd;
  pcd = pcData::getInstance();

  char key = getch();
  string poke;

  while ((key != '1') && (key != '2') && (key != '3')) {
    key = getch();
  }
  if (key == '1') { 
    poke = first_choice.name;
    pcd->addPokemon(first_choice);
  }
  else if (key == '2') {
    poke = second_choice.name;
    pcd->addPokemon(second_choice);
  }
  else if (key == '3') {
    poke = third_choice.name;
    pcd->addPokemon(third_choice);
  }

  printw("I choose you, %s!\n", poke.c_str());
  printw("%s was successfully added to your party.\n", pcd->pcPokemon[0].name.c_str());
  printw("Press any key to exit\n");
  getch();
  hasSelectedPokemon = 1;

}

/**
 * Prints terrain and characters in appropriate places.
*/
void map::printMap() 
{
  erase();
  isThereAPokemon();
  printw("\n");
  for (int i = 0; i < 21; ++i) {
      for (int j = 0; j < 80; ++j) {

        if (terrain[i][j] == '!') {
          attron(COLOR_PAIR(8));
          printw("%c", '#');
          attroff(COLOR_PAIR(8));
        }
        else if (terrain[i][j] == '=') {
          attron(COLOR_PAIR(3));
          printw("%c", '%');
          attroff(COLOR_PAIR(3));
        }
        else if (isSpaceTaken(i,j)) {
          for (int x = 0; x < 9; ++x) {
            if (npcs[x].getRow() == i && npcs[x].getCol() == j) {
              attron(COLOR_PAIR(4));
              printw("%c", npcs[x].getType());
              attroff(COLOR_PAIR(4));
            }
          }
          if (pc.getRow() == i && pc.getCol() == j) {
            attron(COLOR_PAIR(5));
            printw("%c", '@');
            attroff(COLOR_PAIR(5));
          }
        }
        else if (terrain[i][j] == '.') {
          attron(COLOR_PAIR(1));
          printw("%c", terrain[i][j]);
          attroff(COLOR_PAIR(1));
        }
        else if (terrain[i][j] == ':') {
          attron(COLOR_PAIR(2));
          printw("%c", terrain[i][j]);
          attroff(COLOR_PAIR(2));
        }
        else if (terrain[i][j] == '~') {
          attron(COLOR_PAIR(6));
          printw("%c", terrain[i][j]);
          attroff(COLOR_PAIR(6));
        }
        else if (terrain[i][j] == '#') {
          attron(COLOR_PAIR(8));
          printw("%c", terrain[i][j]);
          attroff(COLOR_PAIR(8));
        }
        else if (terrain[i][j] == 'M') {
          attron(COLOR_PAIR(9));
          printw("%c", terrain[i][j]);
          attroff(COLOR_PAIR(9));
        }
        else if (terrain[i][j] == 'C') {
          attron(COLOR_PAIR(10));
          printw("%c", terrain[i][j]);
          attroff(COLOR_PAIR(10));
        }
        else if (terrain[i][j] == '^') {
          attron(COLOR_PAIR(11));
          printw("%c", terrain[i][j]);
          attroff(COLOR_PAIR(11));
        }
        else if (terrain[i][j] == '%') {
          attron(COLOR_PAIR(3));
          printw("%c", terrain[i][j]);
          attroff(COLOR_PAIR(3));
        }
        else {
          printw("%c", terrain[i][j]);
        }
      }
      printw("\n");
  }
  printw("Coordinates: (%d, %d)\n", mapCoords.getRow() - 200, mapCoords.getCol() - 200);
}

/**
 * Displays a list of Pokemon in the PC's party
*/
void map::viewPokemon() 
{
  erase();

  pcData *pcd;
  pcd = pcData::getInstance();

  printw("Pokemon in party:\n");

  for (int i = 0; i < 6; ++i)
  {
    if (pcd->pcPokemon[i].id < 1) {printw("%d: EMPTY SLOT\n-----\n-----\n", (i+1)); continue;}
    printw("%d: %s lvl %d\n", (i+1), pcd->pcPokemon[i].name.c_str(), pcd->pcPokemon[i].level);
    printw("HP: %d/%d | Att: %d | Def: %d | SpAtt: %d | SpDef: %d | Speed: %d\n",
          pcd->pcPokemon[i].hp_cur, pcd->pcPokemon[i].hp_max, pcd->pcPokemon[i].attack, pcd->pcPokemon[i].defense,
          pcd->pcPokemon[i].spAtt, pcd->pcPokemon[i].spDef, pcd->pcPokemon[i].speed);
    printw("Moves: %s  %s\n",pcd->pcPokemon[i].move_one.identifier.c_str(), pcd->pcPokemon[i].move_two.identifier.c_str());
  }
  printw("Press any key to exit");
  getch();

}

/**
 * Access bag/use items
 * Returns 1 if PC is using a poke ball, returns 0 if other item
*/
int map::viewBag()
{
  erase();

  pcData *pcd;
  pcd = pcData::getInstance();

  printw("\nItems in bag:\n\n");
  if (pcd->potions > 0) {printw("1. Potion : x%d\n", pcd->potions);}
  if (pcd->revives > 0) {printw("2. Revive : x%d\n", pcd->revives);}
  if (pcd->poke_balls > 0) {printw("3. Poke Balls : x%d\n", pcd->poke_balls);}
  if (pcd->small_fry > 0) {printw("4. Small Fry : x%d\n", pcd->small_fry);}
  if (pcd->flopper > 0) {printw("5. Flopper : x%d\n", pcd->flopper);}
  if (pcd->shield_fish > 0) {printw("6. Shield Fish : x%d\n", pcd->shield_fish);}
  printw("M. TMs case\n");
  printw("Press corresponding number/key to use item. Press 'b' to exit bag\n");

  char key = 'e';

  while (key != 'b' && key != '1' && key != '2' && key != '3' 
  && key != '4' && key != '5' && key != '6' && key != 'M')
  {
    key = getch();
    if (key == 'b') {return 0;}
    else if (key == '1' && pcd->potions > 0) {usePotion();}
    else if (key == '2' && pcd->revives > 0) {useRevive();} 
    else if (key == '3' && pcd->poke_balls > 0) {return 1;} 
    else if (key == '4' && pcd->small_fry > 0) {useFish(1);}
    else if (key == '5' && pcd->flopper > 0) {useFish(2);}
    else if (key == '6' && pcd->shield_fish > 0) {useFish(3);}
    else if (key == 'M') {viewTms();}
  }
  return 0;
}

void map::viewTms()
{
  erase();
  pcData *pcd;
  pcd = pcData::getInstance();

  printw("\nTMs in case:\n");
  for (int i = 0; i < 6; ++i)
  {
    if (pcd->tm[i].id > 0) {
      printw("%d. #%d: %s\n", i + 1, pcd->tm[i].id, pcd->tm[i].identifier.c_str());
    }
    else {
      printw("%d. -----\n", i + 1);
    }
  }
  printw("Press corresponding number to use TM. Press 'M' to exit TMs case\n");
  
  char key = 'e';

  while (key != 'b' && key != '1' && key != '2' && key != '3' && key != '4'
  && key != '5' && key != '6')
  {
    key = getch();
    if (key == 'M') {return;}
    else if (key == '1' && pcd->tm[0].id > 0) {useTm(0);}
    else if (key == '2' && pcd->tm[1].id > 0) {useTm(1);}
    else if (key == '3' && pcd->tm[2].id > 0) {useTm(2);}
    else if (key == '4' && pcd->tm[3].id > 0) {useTm(3);}
    else if (key == '5' && pcd->tm[4].id > 0) {useTm(4);}
    else if (key == '6' && pcd->tm[5].id > 0) {useTm(5);}
  }
}

void map::useTm(int index)
{
  erase();
  pcData *pcd;
  pcd = pcData::getInstance();

  printw("\nNow using TM #%d: %s\n\n", pcd->tm[index].id, pcd->tm[index].identifier.c_str());

  for (int i = 0; i < 6; i++)
  {
    if (pcd->pcPokemon[i].id > 0) {
      printw("%d. %s ", i + 1, pcd->pcPokemon[i].name.c_str());
      attron(COLOR_PAIR(1));
      printw("ABLE!\n");
      attroff(COLOR_PAIR(1));
    }
  }
  printw("\nPress corresponding number to teach pokemon TM (if able). Press 'M' to exit TMs case\n");

  char key = 'e';
  while (key != '1' && key != '2' && key != '3' && key != '4'
  && key != '5' && key != '6')
  {
    key = getch();
    if (key == 'M') {return;}
    else if (key == '1') { learnNewMove(index, 0);}
    else if (key == '2') { learnNewMove(index, 1);}
    else if (key == '3') { learnNewMove(index, 2);}
    else if (key == '4') { learnNewMove(index, 3);}
    else if (key == '5') { learnNewMove(index, 4);}
    else if (key == '6') { learnNewMove(index, 5);}
  }

  getch();
}

void map::learnNewMove(int tm_index, int poke_index) {
  erase();
  pcData *pcd;
  pcd = pcData::getInstance();
  printw("\nWhat move should %s forget?\n", pcd->pcPokemon[poke_index].name.c_str());
  printw("\n1. %s 2. %s\n", pcd->pcPokemon[poke_index].move_one.identifier.c_str(), pcd->pcPokemon[poke_index].move_two.identifier.c_str());
  printw("Press corresponding number to forget move in place of new move #%d: %s.\n Press 'M' to exit TMs case\n", pcd->tm[tm_index].id, pcd->tm[tm_index].identifier.c_str());

  char key = 'e';
  while (key != '1' && key != '2' && key != 'M') {
    key = getch();
    if (key == 'M') {return;}
    else if (key == '1') {
      printw("%s forgot %s ",pcd->pcPokemon[poke_index].name.c_str(), pcd->pcPokemon[poke_index].move_one.identifier.c_str());
      pcd->pcPokemon[poke_index].move_one = pcd->tm[tm_index];
      printw("and learned %s!\n", pcd->pcPokemon[poke_index].move_one.identifier.c_str());
      printw("TM #%d: %s was destroyed.\n", pcd->tm[tm_index].id, pcd->tm[tm_index].identifier.c_str());
      pcd->tm[tm_index].id = -1;
      printw("\nPress any key to continue\n");
      getch();
    }
    else if (key == '2') {
      printw("%s forgot %s ",pcd->pcPokemon[poke_index].name.c_str(), pcd->pcPokemon[poke_index].move_two.identifier.c_str());
      pcd->pcPokemon[poke_index].move_two = pcd->tm[tm_index];
      printw("and learned %s!\n", pcd->pcPokemon[poke_index].move_two.identifier.c_str());
      printw("TM #%d: %s was destroyed.\n", pcd->tm[tm_index].id, pcd->tm[tm_index].identifier.c_str());
      pcd->tm[tm_index].id = -1;
      printw("\nPress any key to continue\n");
      getch();
    }
  }

}

void map::listPokemon()
{
  pcData *pcd;
  pcd = pcData::getInstance();

  printw("\n");
  for (int i = 0; i < 6; ++i)
  {
    if (pcd->pcPokemon[i].id > -1) // If there is a pokemon in the slot
    {
      printw("%d. %s  HP %d/%d\n", (i+1), pcd->pcPokemon[i].name.c_str(), pcd->pcPokemon[i].hp_cur, pcd->pcPokemon[i].hp_max);
    }
    else { // If slot is empty
      printw("%d. -----\n", (i+1));
    }
  }
}

void map::usePotion()
{
  erase();
  pcData *pcd;
  pcd = pcData::getInstance();

  listPokemon();

  printw("Press corresponding number to use potion. Press 'b' to exit \n");

  char key = 'e';

  while (key != 'b' && key != '1' && key != '2' && key != '3' && key != '4' && key != '5' && key != '6')
  {
    // Clean this up later! This sucks!
    key = getch();
    if (key == 'b') {return;}
    else if (key == '1' && pcd->pcPokemon[0].hp_cur > 0 && (!pcd->pcPokemon[0].is_dead) 
    && pcd->pcPokemon[0].hp_cur != pcd->pcPokemon[0].hp_max && pcd->pcPokemon[0].id > 0) {
      pcd->pcPokemon[0].hp_cur += 20;
      if (pcd->pcPokemon[0].hp_cur > pcd->pcPokemon[0].hp_max) {pcd->pcPokemon[0].hp_cur = pcd->pcPokemon[0].hp_max;}
      pcd->potions--;
      return; 
    }
    else if (key == '2' && pcd->pcPokemon[1].hp_cur > 0 && (!pcd->pcPokemon[1].is_dead) 
    && pcd->pcPokemon[1].hp_cur != pcd->pcPokemon[1].hp_max && pcd->pcPokemon[1].id > 0) {
      pcd->pcPokemon[1].hp_cur += 20;
      if (pcd->pcPokemon[1].hp_cur > pcd->pcPokemon[1].hp_max) {pcd->pcPokemon[1].hp_cur = pcd->pcPokemon[1].hp_max;}
      pcd->potions--;
      return;  
    }
    else if (key == '3' && pcd->pcPokemon[2].hp_cur > 0 && (!pcd->pcPokemon[2].is_dead) 
    && pcd->pcPokemon[2].hp_cur != pcd->pcPokemon[2].hp_max && pcd->pcPokemon[2].id > 0) {
      pcd->pcPokemon[2].hp_cur += 20;
      if (pcd->pcPokemon[2].hp_cur > pcd->pcPokemon[2].hp_max) {pcd->pcPokemon[2].hp_cur = pcd->pcPokemon[2].hp_max;}
      pcd->potions--;
      return; 
    }
    else if (key == '4' && pcd->pcPokemon[3].hp_cur > 0 && (!pcd->pcPokemon[3].is_dead) 
    && pcd->pcPokemon[3].hp_cur != pcd->pcPokemon[3].hp_max && pcd->pcPokemon[3].id > 0) {
      pcd->pcPokemon[3].hp_cur += 20;
      if (pcd->pcPokemon[3].hp_cur > pcd->pcPokemon[3].hp_max) {pcd->pcPokemon[3].hp_cur = pcd->pcPokemon[2].hp_max;}
      pcd->potions--;
      return; 
    }
    else if (key == '5' && pcd->pcPokemon[4].hp_cur > 0 && (!pcd->pcPokemon[4].is_dead) 
    && pcd->pcPokemon[4].hp_cur != pcd->pcPokemon[4].hp_max && pcd->pcPokemon[4].id > 0) {
      pcd->pcPokemon[4].hp_cur += 20;
      if (pcd->pcPokemon[4].hp_cur > pcd->pcPokemon[4].hp_max) {pcd->pcPokemon[4].hp_cur = pcd->pcPokemon[2].hp_max;}
      pcd->potions--;
      return; 
    }
    else if (key == '6' && pcd->pcPokemon[5].hp_cur > 0 && (!pcd->pcPokemon[5].is_dead) 
    && pcd->pcPokemon[5].hp_cur != pcd->pcPokemon[5].hp_max && pcd->pcPokemon[5].id > 0) {
      pcd->pcPokemon[5].hp_cur += 20;
      if (pcd->pcPokemon[5].hp_cur > pcd->pcPokemon[5].hp_max) {pcd->pcPokemon[5].hp_cur = pcd->pcPokemon[5].hp_max;}
      pcd->potions--;
      return; 
    }
  }
}

void map::useFish(int type)
{
  int amount_healed;
  std::string fish;
  if (type == 1) { // Small Fry
    amount_healed = 10;
    fish = "Small Fry";
  } 
  else if (type == 2) { // Flopper
    amount_healed = 50;
    fish = "Flopper";
  }
  else { // Shield Fish
    amount_healed = 10000;
    fish = "Shield Fish";
  }

  erase();
  pcData *pcd;
  pcd = pcData::getInstance();

  listPokemon();
  printw("Press corresponding number to use %s. Press 'b' to exit \n", fish.c_str());
  char key = 'e';

  while (key != 'b' && key != '1' && key != '2' && key != '3' && key != '4' && key != '5' && key != '6')
  {
    // Clean this up later! This sucks!
    key = getch();
    if (key == 'b') {return;}
    else if (key == '1' && pcd->pcPokemon[0].hp_cur > 0 && (!pcd->pcPokemon[0].is_dead) 
    && pcd->pcPokemon[0].hp_cur != pcd->pcPokemon[0].hp_max && pcd->pcPokemon[0].id > 0) {
      pcd->pcPokemon[0].hp_cur += amount_healed;
      if (pcd->pcPokemon[0].hp_cur > pcd->pcPokemon[0].hp_max) {pcd->pcPokemon[0].hp_cur = pcd->pcPokemon[0].hp_max;}

      if (type == 1) {pcd->small_fry--;}
      else if (type == 2) {pcd->flopper--;}
      else {pcd->shield_fish--;}

      return; 
    }
    else if (key == '2' && pcd->pcPokemon[1].hp_cur > 0 && (!pcd->pcPokemon[1].is_dead) 
    && pcd->pcPokemon[1].hp_cur != pcd->pcPokemon[1].hp_max && pcd->pcPokemon[1].id > 0) {
      pcd->pcPokemon[1].hp_cur += amount_healed;
      if (pcd->pcPokemon[1].hp_cur > pcd->pcPokemon[1].hp_max) {pcd->pcPokemon[1].hp_cur = pcd->pcPokemon[1].hp_max;}
      if (type == 1) {pcd->small_fry--;}
      else if (type == 2) {pcd->flopper--;}
      else {pcd->shield_fish--;}
      return;  
    }
    else if (key == '3' && pcd->pcPokemon[2].hp_cur > 0 && (!pcd->pcPokemon[2].is_dead) 
    && pcd->pcPokemon[2].hp_cur != pcd->pcPokemon[2].hp_max && pcd->pcPokemon[2].id > 0) {
      pcd->pcPokemon[2].hp_cur += amount_healed;
      if (pcd->pcPokemon[2].hp_cur > pcd->pcPokemon[2].hp_max) {pcd->pcPokemon[2].hp_cur = pcd->pcPokemon[2].hp_max;}
      if (type == 1) {pcd->small_fry--;}
      else if (type == 2) {pcd->flopper--;}
      else {pcd->shield_fish--;}
      return; 
    }
    else if (key == '4' && pcd->pcPokemon[3].hp_cur > 0 && (!pcd->pcPokemon[3].is_dead) 
    && pcd->pcPokemon[3].hp_cur != pcd->pcPokemon[3].hp_max && pcd->pcPokemon[3].id > 0) {
      pcd->pcPokemon[3].hp_cur += amount_healed;
      if (pcd->pcPokemon[3].hp_cur > pcd->pcPokemon[3].hp_max) {pcd->pcPokemon[3].hp_cur = pcd->pcPokemon[2].hp_max;}
      if (type == 1) {pcd->small_fry--;}
      else if (type == 2) {pcd->flopper--;}
      else {pcd->shield_fish--;}
      return; 
    }
    else if (key == '5' && pcd->pcPokemon[4].hp_cur > 0 && (!pcd->pcPokemon[4].is_dead) 
    && pcd->pcPokemon[4].hp_cur != pcd->pcPokemon[4].hp_max && pcd->pcPokemon[4].id > 0) {
      pcd->pcPokemon[4].hp_cur += amount_healed;
      if (pcd->pcPokemon[4].hp_cur > pcd->pcPokemon[4].hp_max) {pcd->pcPokemon[4].hp_cur = pcd->pcPokemon[2].hp_max;}
      if (type == 1) {pcd->small_fry--;}
      else if (type == 2) {pcd->flopper--;}
      else {pcd->shield_fish--;}
      return; 
    }
    else if (key == '6' && pcd->pcPokemon[5].hp_cur > 0 && (!pcd->pcPokemon[5].is_dead) 
    && pcd->pcPokemon[5].hp_cur != pcd->pcPokemon[5].hp_max && pcd->pcPokemon[5].id > 0) {
      pcd->pcPokemon[5].hp_cur += amount_healed;
      if (pcd->pcPokemon[5].hp_cur > pcd->pcPokemon[5].hp_max) {pcd->pcPokemon[5].hp_cur = pcd->pcPokemon[5].hp_max;}
      if (type == 1) {pcd->small_fry--;}
      else if (type == 2) {pcd->flopper--;}
      else {pcd->shield_fish--;}
      return; 
    }
  }

}

void map::useRevive()
{
  erase();
  pcData *pcd;
  pcd = pcData::getInstance();

  listPokemon();

  printw("Press corresponding number to use revive. Press 'b' to exit \n");

  char key = 'e';

  while (key != 'b' && key != '1' && key != '2' && key != '3' && key != '4' && key != '5' && key != '6')
  {
    key = getch();
    if (key == 'b') {return;}
    else if (key == '1' && pcd->pcPokemon[0].is_dead && pcd->pcPokemon[0].id > -1 && pcd->revives > 0) {
      pcd->pcPokemon[0].hp_cur = pcd->pcPokemon[0].hp_max / 2;
      pcd->pcPokemon[0].is_dead = 0;
      pcd->revives--;
      return;
    }
    else if (key == '2' && pcd->pcPokemon[1].is_dead && pcd->pcPokemon[1].id > -1 && pcd->revives > 0) {
      pcd->pcPokemon[1].hp_cur = pcd->pcPokemon[1].hp_max / 2;
      pcd->pcPokemon[1].is_dead = 0;
      pcd->revives--;
      return;
    }
    else if (key == '3' && pcd->pcPokemon[2].is_dead && pcd->pcPokemon[2].id > -1 && pcd->revives > 0) {
      pcd->pcPokemon[2].hp_cur = pcd->pcPokemon[2].hp_max / 2;
      pcd->pcPokemon[2].is_dead = 0;
      pcd->revives--;
      return;
    }
    else if (key == '4' && pcd->pcPokemon[3].is_dead && pcd->pcPokemon[3].id > -1 && pcd->revives > 0) {
      pcd->pcPokemon[3].hp_cur = pcd->pcPokemon[3].hp_max / 2;
      pcd->pcPokemon[3].is_dead = 0;
      pcd->revives--;
      return;
    }
    else if (key == '5' && pcd->pcPokemon[4].is_dead && pcd->pcPokemon[4].id > -1 && pcd->revives > 0) {
      pcd->pcPokemon[4].hp_cur = pcd->pcPokemon[4].hp_max / 2;
      pcd->pcPokemon[4].is_dead = 0;
      pcd->revives--;
      return;
    }
    else if (key == '6' && pcd->pcPokemon[5].is_dead && pcd->pcPokemon[5].id > -1 && pcd->revives > 0) {
      pcd->pcPokemon[5].hp_cur = pcd->pcPokemon[5].hp_max / 2;
      pcd->pcPokemon[5].is_dead = 0;
      pcd->revives--;
      return;
    }
  }
}

/**
 * Switch pokemon. Can only switch to alive pokemon
*/
pokemon* map::switchPokemon()
{
  erase();
  pcData *pcd;
  pcd = pcData::getInstance();

  listPokemon();
  printw("Select a pokemon that is not knocked out to switch in.\n");

  char key = 'e';

  while (key != '1' && key != '2' && key != '3' && key != '4' && key != '5' && key != '6')
  {
    key = getch();
    if (key == '1') {
      if (!pcd->pcPokemon[0].is_dead) {
        return &pcd->pcPokemon[0];
      }
      else {
      printw("Can't switch to slot 1!\n");
      key = 'e';
      }
    }
    else if (key == '2') {
      if (!pcd->pcPokemon[1].is_dead) {
        return &pcd->pcPokemon[1];
      }
      else {
      printw("Can't switch to slot 2!\n");
      key = 'e';
      }
    }
    else if (key == '3') {
      if (!pcd->pcPokemon[2].is_dead) {
        return &pcd->pcPokemon[2];
      }
      else {
      printw("Can't switch to slot 3!\n");
      key = 'e';
      }
    }
    else if (key == '4') {
      if (!pcd->pcPokemon[3].is_dead) {
        return &pcd->pcPokemon[3];
      }
      else {
      printw("Can't switch to slot 4!\n");
      key = 'e';
      }
    }
    else if (key == '5') {
      if (!pcd->pcPokemon[4].is_dead) {
        return &pcd->pcPokemon[4];
      }
      else {
      printw("Can't switch to slot 5!\n");
      key = 'e';
      }
    }
    else if (key == '6') {
      if (!pcd->pcPokemon[5].is_dead) {
        return &pcd->pcPokemon[5];
      }
      else {
      printw("Can't switch to slot 6!\n");
      key = 'e';
      }
    }
  }
  return &pcd->pcPokemon[0];
}


/**
 * Encountering a pokemon in the tall grass.
 * Can either catch, kill, or run.
*/
void map::doWildEncounter()
{
  erase();
  printw("\nA wild pokemon appears!!!\n");
  printw("\nPress any key to continue\n");
  getch();

  pcData *pcd;
  pcd = pcData::getInstance();

  pokemon wild = generateRandomPokemon();
  pokemon* curPoke = pcd->getFirstPokemon(); // Current pokemon PC is using, at first it will be first pokemon that is out

  printWildUi(wild, *curPoke);

  int battleStatus = 0; 
  // 0 is battle in progress, 1 is fled, 2 is wild pokemon killed, 3 is PC pokemon all killed, 4 is pokemon caught, 5 is pokemon fled

  // 'a' is attack, 'c' is catch, 'r' is run
  char key, attack_key = 'e';
  int runChance, runAttempts = 1;
  attack_move cur_move_used, wild_move_used;
  int turn;

  int cur_move_index; // The choice of move 1/2, 0 and 1 as ints
  int wild_move_index;

  while (battleStatus == 0) {
    printWildUi(wild, *curPoke);
    printw("What will %s do?\n", curPoke->name.c_str());
    printw("Press 'f' to fight, 'b' for bag, 'r' to run, 's' to switch Pokemon\n");
      
    key = getch();
    if (key == 'f') {

      printWildUi(wild, *curPoke);
      printw("Press 1 or 2 to choose attack: 1. %s  2. %s\n", curPoke->move_one.identifier.c_str(), 
      curPoke->move_two.identifier.c_str());

      // Determine what attack to execute
      attack_key = '0'; // Reset attack choice key
      while (attack_key != '1' && attack_key != '2') {attack_key = getch();}
      if (attack_key == '1') {cur_move_index = 0;}
      else if (attack_key == '2' && curPoke->move_two.identifier != "---") {cur_move_index = 1;}
      else {cur_move_index = 0;} // If user selects second move when they only have one move, first move is used

      // Determine what wild pokemon attack to use (random)
      std::string empty = "---";
      if (wild.move_two.identifier == empty) { // If wild only has one move, use it
        wild_move_index = 0;
      }
      else {
        if (rand() % 2) {wild_move_index = 1;}
        else {wild_move_index = 0;}
      }

      // Calculate who attacks first (0 if wild, 1 if PC)
      turn = whosFirst(wild_move_used.priority, cur_move_used.priority,
      wild.speed, curPoke->speed);

      if (turn) { //If PC is first
        doAnAttack(curPoke, &wild, 1, cur_move_index);
        if (wild.is_dead) {battleStatus = 2; continue;} // If we killed wild pokemon end battle

        doAnAttack(&wild, curPoke, 0, wild_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon(); continue;} // If PC has more pokemon
          else {battleStatus = 3; continue;} // If PC has no pokemon left
        }
      }
      else {
        doAnAttack(&wild, curPoke, 0, wild_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon(); continue;} // If PC has more pokemon
          else {battleStatus = 3; continue;} // If PC has no pokemon left
        }

        doAnAttack(curPoke, &wild, 1, cur_move_index);
        if (wild.is_dead) {battleStatus = 2; continue;} // If we killed wild pokemon end battle
      }

      //Reset attack choice key
      attack_key = '0';

    }
    else if (key == 'b') {
      
      if (viewBag()) { // If PC attempts to catch the pokemon
        if (pcd->hasEmptySlot() && pcd->poke_balls > 0) { // If there is room in the party for pokemon and we have a ball
          pcd->addPokemonToParty(wild);
          pcd->poke_balls--;
          battleStatus = 4; continue;
        }
        else {
          battleStatus = 5; continue;
          getch();
        }
      }

      // Opposing pokemon still attacks if bag is accessed

        // Determine what wild pokemon attack to use (random)
        std::string empty = "---";
        if (wild.move_two.identifier == empty) { // If wild only has one move, use it
          wild_move_index = 0;
        }
        else {
          if (rand() % 2) {wild_move_index = 1;}
          else {wild_move_index = 0;}
        }

        doAnAttack(&wild, curPoke, 0, wild_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon();} // If PC has more pokemon
          else {battleStatus = 3; continue;} // If PC has no pokemon left
        }

        getch();

    }
    else if (key == 'r') {

      runChance = (((curPoke->speed * 32) / ((wild.speed / 4) % 256)) + 30) * runAttempts;

      if (rand() % 256 < runChance) {
        printWildUi(wild, *curPoke);
        printw("Got away safely!\n");
        printw("\nPress any key to continue\n");
        getch();

        battleStatus = 1;
      }
      else {
        printWildUi(wild, *curPoke);
        printw("Couldn't get away!\n");
        printw("\nPress any key to continue\n");
        getch();

        // Opposing pokemon still attacks if run attempt fails

        // Determine what wild pokemon attack to use (random)
        std::string empty = "---";
        if (wild.move_two.identifier == empty) { // If wild only has one move, use it
          wild_move_index = 0;
        }
        else {
          if (rand() % 2) {wild_move_index = 1;}
          else {wild_move_index = 0;}
        }

        doAnAttack(&wild, curPoke, 0, wild_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon();} // If PC has more pokemon
          else {battleStatus = 3; continue;} // If PC has no pokemon left
        }
      }

    }
    else if (key == 's') {
      curPoke = switchPokemon();
      printWildUi(wild, *curPoke);
      printw("Go, %s!\n", curPoke->name.c_str());
      printw("\nPress any key to continue\n");
      getch();

      // Opposing pokemon still attacks if pokemon is switched

        // Determine what wild pokemon attack to use (random)
        std::string empty = "---";
        if (wild.move_two.identifier == empty) { // If wild only has one move, use it
          wild_move_index = 0;
        }
        else {
          if (rand() % 2) {wild_move_index = 1;}
          else {wild_move_index = 0;}
        }

        doAnAttack(&wild, curPoke, 0, wild_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (pcd->allPokemonKnockedOut()) {curPoke = switchPokemon();} // If PC has more pokemon
          else {battleStatus = 3; continue;} // If PC has no pokemon left
        }
    }
  }
  erase();

  if (battleStatus == 2) { // If wild pokemon was killed
    printWildUi(wild, *curPoke);
    printw("The opposing %s fainted!\n", wild.name.c_str());
    printw("\nPress any key to exit\n");
    getch();
    erase();
  }
  else if (battleStatus == 3) { //If all PC pokemon were killed
    printWildUi(wild, *curPoke);
    printw("You're all out of pokemon!\n");
    printw("Game over!\n");
    printw("\nPress any key to exit\n");
    getch();
    gameOver(); // Game is over when all pokemon are dead
  }
  else if (battleStatus == 4) { // If wild pokemon was caught
    printWildUi(wild, *curPoke);
    printw("You threw a Poke Ball at the wild %s!\n", wild.name.c_str());
    printw("\nPress any key to continue\n");
    getch();
    printWildUi(wild, *curPoke);
    printw("Wild %s successfully caught!\n", wild.name.c_str());
    printw("%s was added to party.\n", wild.name.c_str());
    printw("\nPress any key to exit\n");
    getch();
    erase();
  }
  else if (battleStatus == 5) { // If wild pokemon fled
    printWildUi(wild, *curPoke);
    printw("You threw a Poke Ball at the wild %s!\n", wild.name.c_str());
    printw("\nPress any key to continue\n");
    getch();
    printWildUi(wild, *curPoke);
    printw("The wild %s broke free from it's Poke Ball!\n", wild.name.c_str());
    printw("\nPress any key to continue\n");
    getch();
    printWildUi(wild, *curPoke);
    printw("The wild %s fled the battle!\n", wild.name.c_str());
    printw("\nPress any key to exit\n");
    getch();
    erase();
  }
  erase();
}

/**
 * Trainer battle with npcs[index]
 * Two endings - PC or NPC is defeated
*/
void map::doTrainerBattle(int index)
{
  erase();
  printw("\nA trainer has challenged you to a battle!\n");
  printw("\nPress any key to continue\n");
  getch();

  pcData *pcd;
  pcd = pcData::getInstance();

  // Get two first pokemon
  pokemon* curPoke = pcd->getFirstPokemon();
  pokemon* npcPoke = &npcs[index].npcPokemon[0];

  printWildUi(*npcPoke, *curPoke);

  int battleStatus = 0;
  // 0 is battle in progress, 1 is PC won, 2 is NPC won

  char key, attack_key;
  attack_move cur_move_used, npc_move_used;
  int turn;
  int cur_move_index, npc_move_index;

  while (battleStatus == 0) {
    printWildUi(*npcPoke, *curPoke);
    printw("What will %s do?\n", curPoke->name.c_str());
    printw("Press 'f' to fight, 'b' for bag, 'r' to run, 's' to switch Pokemon\n");

    key = getch();
    if (key == 'f') {

      printWildUi(*npcPoke, *curPoke);
      printw("Press 1 or 2 to choose attack: 1. %s  2. %s\n", curPoke->move_one.identifier.c_str(), 
      curPoke->move_two.identifier.c_str());

      // Determine what attack to execute
      attack_key = '0'; // Reset attack choice key
      while (attack_key != '1' && attack_key != '2') {attack_key = getch();}
      if (attack_key == '1') {cur_move_index = 0;}
      else if (attack_key == '2' && curPoke->move_two.identifier != "---") {cur_move_index = 1;}
      else {cur_move_index = 0;} // If user selects second move when they only have one move, first move is used

      // Determine what attack npc will use (random)
      std::string empty = "---";
      if (npcPoke->move_two.identifier == empty) { // If wild only has one move, use it
        npc_move_index = 0;
      }
      else {
        if (rand() % 2) {npc_move_index = 1;}
        else {npc_move_index = 0;}
      }

      // Calculate who attacks first (0 if NPC, 1 if PC)
      turn = whosFirst(npc_move_used.priority, cur_move_used.priority,
      npcPoke->speed, curPoke->speed);

      if (turn) { //If PC is first
        doAnAttack(curPoke, npcPoke, 1, cur_move_index);
        if (npcPoke->is_dead) { // If NPC's pokemon in play died
          if(!npcs[index].allPokemonDead()) {npcPoke = npcs[index].getFirstPokemon(); continue;} // If NPC has more pokemon
          else {battleStatus = 1; continue;} // If NPC has no pokemon left
        }

        doAnAttack(npcPoke, curPoke, 0, npc_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon(); continue;} // If PC has more pokemon
          else {battleStatus = 2; continue;} // If PC has no pokemon left
        }
      }
      else {
        doAnAttack(npcPoke, curPoke, 0, npc_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon(); continue;} // If PC has more pokemon
          else {battleStatus = 2; continue;} // If PC has no pokemon left
        }

        doAnAttack(curPoke, npcPoke, 1, cur_move_index);
        if (npcPoke->is_dead) { // If NPC's pokemon in play died
          if(!npcs[index].allPokemonDead()) {npcPoke = npcs[index].getFirstPokemon(); continue;} // If NPC has more pokemon
          else {battleStatus = 1; continue;} // If NPC has no pokemon left
        }
      }

    }
    else if (key == 'b') {
      if (viewBag()) { // If PC attempts to catch the pokemon
        printWildUi(*npcPoke, *curPoke);
        printw("You can't catch another trainer's pokemon!\n");
        printw("\nPress any key to continue\n");
        getch();
      }

      // Opposing pokemon still attacks if bag is accessed

      // Determine what NPC pokemon attack to use (random)
      std::string empty = "---";
      if (npcPoke->move_two.identifier == empty) { // If wild only has one move, use it
        npc_move_index = 0;
      }
      else {
        if (rand() % 2) {npc_move_index = 1;}
        else {npc_move_index = 0;}
      }

      doAnAttack(npcPoke, curPoke, 0, npc_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon(); continue;} // If PC has more pokemon
          else {battleStatus = 2; continue;} // If PC has no pokemon left
      }
      //getch(); <- extra...?

    }
    else if (key == 'r') {
      printWildUi(*npcPoke, *curPoke);
      printw("You can't flee a trainer battle you coward\n");
      printw("\nPress any key to continue\n");
      getch();

      // Opposing pokemon still attacks if opponent is a coward

      // Determine what NPC pokemon attack to use (random)
      std::string empty = "---";
      if (npcPoke->move_two.identifier == empty) { // If wild only has one move, use it
        npc_move_index = 0;
      }
      else {
        if (rand() % 2) {npc_move_index = 1;}
        else {npc_move_index = 0;}
      }

      doAnAttack(npcPoke, curPoke, 0, npc_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon(); continue;} // If PC has more pokemon
          else {battleStatus = 2; continue;} // If PC has no pokemon left
      }
    }
    else if (key == 's') {
      curPoke = switchPokemon();
      printWildUi(*npcPoke, *curPoke);
      printw("Go, %s!\n", curPoke->name.c_str());
      printw("\nPress any key to continue\n");
      getch();

      // Opposing pokemon still attacks if pokemon is switched

      // Determine what NPC pokemon attack to use (random)
      std::string empty = "---";
      if (npcPoke->move_two.identifier == empty) { // If wild only has one move, use it
        npc_move_index = 0;
      }
      else {
        if (rand() % 2) {npc_move_index = 1;}
        else {npc_move_index = 0;}
      }

      doAnAttack(npcPoke, curPoke, 0, npc_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon(); continue;} // If PC has more pokemon
          else {battleStatus = 2; continue;} // If PC has no pokemon left
      }
    }
  }
  erase();

  if (battleStatus == 1) { // If PC won the battle
    npcs[index].is_defeated = 1;
    printWildUi(*npcPoke, *curPoke);
    printw("The trainer is all out of pokemon!\n");
    printw("You won the battle!\n");
    printw("\nPress any key to exit\n");
    getch();
    erase();
  }
  else if (battleStatus == 2) { // If NPC won the battle
    printWildUi(*npcPoke, *curPoke);
    printw("You're all out of pokemon!\n");
    printw("Game over!\n");
    printw("\nPress any key to exit\n");
    getch();
    gameOver();
  }
}

void map::printWildUi(pokemon wild, pokemon mine)
{
  erase();

  std::string w_gender, m_gender;

  if(wild.gender == 1) {w_gender = "M";} else {w_gender = "F";}
  if(mine.gender == 1) {m_gender = "M";} else {m_gender = "F";}

  printw("\n");

  for (int i = 0; i < 80; ++i) {printw("=");} // "==="
  printw("\n");

  // Wild pokemon details
  printw("%s lvl %d %s\n", wild.name.c_str(), wild.level, w_gender.c_str());
  printw("HP: %d/%d\n", wild.hp_cur, wild.hp_max);

  printw("\n\n");

  // PC pokemon details
  printw("%s lvl %d %s\n", mine.name.c_str(), mine.level, m_gender.c_str());
  printw("HP: %d/%d\n", mine.hp_cur, mine.hp_max);

  for (int i = 0; i < 80; ++i) {printw("=");} // "==="
  printw("\n");

}

void map::gameOver()
{
  erase();
  attron(COLOR_PAIR(4));
  printw("\nGAME OVER!\n");
  attroff(COLOR_PAIR(4));

  printw("All your availible pokemon were knocked out!\n");
  printw("\nPress any key to end game\n");
  getch();
  endwin();
  exit(0);
}

/**
 * If PC is adjacent to water in N,W,E,S direction, returns 1
 * Else returns 0
*/
int map::canFishHere()
{
  if (terrain[pc.getRow() + 1][pc.getCol()] == '~') {return 1;}
  else if (terrain[pc.getRow() - 1][pc.getCol()] == '~') {return 1;}
  else if (terrain[pc.getRow()][pc.getCol() + 1] == '~') {return 1;}
  else if (terrain[pc.getRow()][pc.getCol() - 1] == '~') {return 1;}

  return 0;
}

void map::cantFishHere()
{
  erase();
  printw("\n");
  printw("Can't fish here num nutz! Go stand near some water\n\n");

  /* Fish ASCII art*/
  attron(COLOR_PAIR(6));
  printw("   ,\n");
  printw("  /|\n");
  printw(" /_|  ,\n");
  printw("/o  \\/|\n");
  printw("\\<__/\\|\n");
  printw(" \\ |\n");
  printw("  \\|\n");
  attroff(COLOR_PAIR(6));

  printw("\nPress any key to exit\n");
  getch();
}

/* Fishing */
void map::printFishingUi()
{
  erase();
  printw("\n");
  printw("You're fishing...\n\n");

  /* Fishing poke ASCII art*/
  printw("    / .\n");
  printw("   /  .\n");
  printw("  /   .\n");
  printw(" @    .\n");
  printw("/   ");
  attron(COLOR_PAIR(6));
  printw("~~~~~\n");
  attroff(COLOR_PAIR(6));
  printw("\nPress 'e' to try your luck, press 'o' to exit");
}

void map::doFishing()
{
  char key = 'a';
  while (key != 'o') {
    printFishingUi();
    key = getch();
    if (key == 'e') {catchSomething();}
  }
}

/**
 * Robs the player of 1 - 3 items in their bag.
*/
void map::doRobberFish()
{
  pcData *pcd;
  pcd = pcData::getInstance();

  printw("The robber fish approaches!\n");

  /* If PC has not enough items to steal */
  if (pcd->potions + pcd->poke_balls + pcd->revives
  + pcd->small_fry + pcd->flopper + pcd->shield_fish <= 2) {
    printw("The robber fish swam away, because it can smell broke people\n");
    printw("(You don't have enough items to be stolen from)\n");
    return;
  }

  int decreaseCount = rand() % 3 + 1; // 1 - 3 (items taken away)
  int index;

  printw("The robber fish stole from you: \n");

  while(decreaseCount > 0)
  {
    index = rand() % 6; // 0-5

    if (index == 0 && pcd->potions > 0) {
      pcd->potions--;
      printw("-1 potion\n");
      decreaseCount--;
    }
    else if (index == 1 && pcd->poke_balls > 0) {
      pcd->poke_balls--;
      printw("-1 Poke Ball\n");
      decreaseCount--;
    }
    else if (index == 2 && pcd->revives > 0) {
      pcd->revives--;
      printw("-1 revive\n");
      decreaseCount--;
    }
    else if (index == 3 && pcd->small_fry > 0) {
      pcd->small_fry--;
      printw("-1 small fry\n");
      decreaseCount--;
    }
    else if (index == 4 && pcd->flopper > 0) {
      pcd->flopper--;
      printw("-1 flopper\n");
      decreaseCount--;
    }
    else if (index == 5 && pcd->flopper > 0) {
      pcd->flopper--;
      printw("-1 shield fish\n");
      decreaseCount--;
    }
  }
}

/**
 * Damages all PC pokemon for 10% of their base health
*/
void map::doPipeBomb()
{
  pcData *pcd;
  pcd = pcData::getInstance();

  printw("You fished up a pipe bomb! Your pokemon took damage\n");

  for (int i = 0; i < 5; ++i)
  {
    if (pcd->pcPokemon[i].id < 0) { // If pokemon slot is empty
      continue;
    }
    if (pcd->pcPokemon[i].is_dead) { // If pokemon is dead
      continue;
    }

    printw("%d. %s  HP fell from %d/%d to ", i + 1, pcd->pcPokemon[i].name.c_str(), pcd->pcPokemon[i].hp_cur, pcd->pcPokemon[i].hp_max);

    pcd->pcPokemon[i].hp_cur = pcd->pcPokemon[i].hp_cur - (pcd->pcPokemon[i].hp_max / 10);

    if (pcd->pcPokemon[i].hp_cur <= 0) { // If pokemon died from the damage
      pcd->pcPokemon[i].hp_cur = 0;
      pcd->pcPokemon[i].is_dead = 1;
    }

    printw("%d/%d\n", pcd->pcPokemon[i].hp_cur, pcd->pcPokemon[i].hp_max);
  }

  if (pcd->allPokemonKnockedOut()) {
    printw("Oh no! The pipe bomb knocked out your last useable Pokemon!\n");
    printw("Game over!");
    getch();
    gameOver();
  }
}

void map::doSeaGem()
{
  pcData *pcd;
  pcd = pcData::getInstance();

  int rando = rand() % 2; //0-2

  if (rando) {
    printw("You fished up the rare blessed sea ");
    attron(COLOR_PAIR(6));
    printw("DIAMOND");
    attroff(COLOR_PAIR(6));
    printw("!\n");

    pcd->healAllPokemon();
    printw("All your pokemon were restored to full health.");
  }
  else {
    printw("You fished up the rare blessed sea ");
    attron(COLOR_PAIR(1));
    printw("EMERALD");
    attroff(COLOR_PAIR(1));
    printw("!\n");

    printw("Your first pokemon (%s) had their stats increased!\n\n", pcd->pcPokemon[0].name.c_str());

    printw("HP: %d -> ", pcd->pcPokemon[0].hp_max);
    pcd->pcPokemon[0].hp_max += 1;
    pcd->pcPokemon[0].hp_cur += 1;
    printw("%d\n", pcd->pcPokemon[0].hp_max);

    printw("Attack: %d -> ", pcd->pcPokemon[0].attack);
    pcd->pcPokemon[0].attack += 2;
    printw("%d\n", pcd->pcPokemon[0].attack);

    printw("Defense: %d -> ", pcd->pcPokemon[0].defense);
    pcd->pcPokemon[0].defense += 2;
    printw("%d\n", pcd->pcPokemon[0].defense);

    printw("Sp. Attack: %d -> ", pcd->pcPokemon[0].spAtt);
    pcd->pcPokemon[0].spAtt += 2;
    printw("%d\n", pcd->pcPokemon[0].spAtt);

    printw("Sp. Defense: %d -> ", pcd->pcPokemon[0].spDef);
    pcd->pcPokemon[0].spDef += 2;
    printw("%d\n", pcd->pcPokemon[0].spDef);

    printw("Speed: %d -> ", pcd->pcPokemon[0].speed);
    pcd->pcPokemon[0].speed += 2;
    printw("%d\n", pcd->pcPokemon[0].speed);

  }
}

void map::giveTm()
{
  pcData *pcd;
  pcd = pcData::getInstance();

  moveData *attacks;
  attacks = moveData::getInstance();

  int index = rand() % 845;
  
  attack_move newTm = attacks->getMove(index);

  printw("You fished up TM #%d: %s!\n", newTm.id, newTm.identifier.c_str());

  if (pcd->addTm(newTm)) {
    printw("TM #%d was added to your TMs case.\n", newTm.id);
  }
  else {
    printw("TM #%d was not to your TMs case, your TMs case is full.\n", newTm.id);
  }
}

void map::catchSomething()
{
  pcData *pcd;
  pcd = pcData::getInstance();

  erase();
  printw("\nYour rod has been cast, and it moved!\n");
  printw("\nPress any key to continue\n");
  getch();
  erase();
  printw("\n");

  int rando = (rand() % 100) + 1; // 1 - 100
  int rando2;

  // Odds for stuff
  // 40% for fish (varying size)
  // 25% for other items
  // 10% for robber fish
  // 5% for pipe bomb
  // 5% for blessed sea gem
  // 5% for TM
  // 5% for water walking boots
  // 4% for Wailord Boss
  // 1% for Kraken Boss
  
  if (rando <= 40) // Fish
  {
    rando2 = rand() % 7; //0 - 6

    if (rando2 <= 2) {
      pcd->small_fry++;
      printw("You caught a small fry!\n");
      printw("x1 small fry added to bag\n");
    }
    else if (rando <= 5) {
      pcd->flopper++;
      printw("You caught a flopper!\n");
      printw("x1 flopper added to bag\n");
    }
    else {
      pcd->shield_fish++;
      printw("You caught a shield fish!\n");
      printw("x1 shield fish added to bag\n");
    }

  }
  else if (rando <= 65) // Bag item
  {
    rando2 = rand() % 7; //0 - 6

    if (rando2 <= 2) {
      pcd->potions++;
      printw("You fished up a potion!\n");
      printw("+1 potion added to bag\n");
    }
    else if (rando <= 5) {
      pcd->poke_balls++;
      printw("You fished up a Poke Ball!\n");
      printw("+1 Poke Ball added to bag\n");
    }
    else {
      pcd->revives++;
      printw("You fished up a revive!\n");
      printw("+1 revive added to bag\n");
    }
  }
  else if (rando <= 75) // Robber fish
  {
    doRobberFish();
  }
  else if (rando <= 80) // Pipe bomb
  {
    doPipeBomb();
  }
  else if (rando <= 85) // Blessed sea gem
  {
    doSeaGem();
  }
  else if (rando <= 90) // TM
  {
    giveTm();
  }
  else if (rando <= 95) // Water walking boots
  {

    pcd->canWalkOnWater = 1;
    printw("You fished up magical water walking boots!\n");
    printw("You can now walk on water, try it out!\n");
  }
  else if (rando <= 99) // Wailord boss
  {
    printw("A Wailord Boss approaches!\n"); 
    getch();
    waterBattle(1); 
  }
  else if (rando == 100) // Nuclear bomb
  {
    attron(COLOR_PAIR(5));
    printw("THE KRAKEN IS COMING\n");
    attroff(COLOR_PAIR(5));
    getch();
    printw("The great Kraken of the Sea approaches!\n"); 
    getch();
    waterBattle(0);
  }

  printw("\nPress any key to continue\n");
  getch();

}

/**
 * An annoying sea boss fight, either Kraken or Wailord Boss
*/
void map::waterBattle(int boss)
{
  erase();
  if (boss) {printw("\nA wild Wailord - BOSS appears!\n");}
  else {
    printw("\nA wild KRAKEN - BOSS appears!\n");
  }
  printw("\nPress any key to continue\n");
  getch();

  pcData *pcd;
  pcd = pcData::getInstance();
  pokemon wild;
  if (boss) {wild = generateWailord();}
  else {wild = generateKraken();}
  pokemon* curPoke = pcd->getFirstPokemon(); // Current pokemon PC is using, at first it will be first pokemon that is out

  printWildUi(wild, *curPoke);

  int battleStatus = 0; 
  // 0 is battle in progress, 1 is fled, 2 is wild pokemon killed, 3 is PC pokemon all killed, 4 is pokemon caught, 5 is pokemon fled

  // 'a' is attack, 'c' is catch, 'r' is run
  char key, attack_key = 'e';
  attack_move cur_move_used, wild_move_used;
  int turn;

  int cur_move_index; // The choice of move 1/2, 0 and 1 as ints
  int wild_move_index;

  while (battleStatus == 0) {
    printWildUi(wild, *curPoke);
    printw("What will %s do?\n", curPoke->name.c_str());
    printw("Press 'f' to fight, 'b' for bag, 'r' to run, 's' to switch Pokemon\n");
      
    key = getch();
    if (key == 'f') {

      printWildUi(wild, *curPoke);
      printw("Press 1 or 2 to choose attack: 1. %s  2. %s\n", curPoke->move_one.identifier.c_str(), 
      curPoke->move_two.identifier.c_str());

      // Determine what attack to execute
      attack_key = '0'; // Reset attack choice key
      while (attack_key != '1' && attack_key != '2') {attack_key = getch();}
      if (attack_key == '1') {cur_move_index = 0;}
      else if (attack_key == '2' && curPoke->move_two.identifier != "---") {cur_move_index = 1;}
      else {cur_move_index = 0;} // If user selects second move when they only have one move, first move is used

      // Determine what wild pokemon attack to use (random)
      std::string empty = "---";
      if (wild.move_two.identifier == empty) { // If wild only has one move, use it
        wild_move_index = 0;
      }
      else {
        if (rand() % 2) {wild_move_index = 1;}
        else {wild_move_index = 0;}
      }

      // Calculate who attacks first (0 if wild, 1 if PC)
      turn = whosFirst(wild_move_used.priority, cur_move_used.priority,
      wild.speed, curPoke->speed);

      if (turn) { //If PC is first
        doAnAttack(curPoke, &wild, 1, cur_move_index);
        if (wild.is_dead) {battleStatus = 2; continue;} // If we killed wild pokemon end battle

        doAnAttack(&wild, curPoke, 0, wild_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon(); continue;} // If PC has more pokemon
          else {battleStatus = 3; continue;} // If PC has no pokemon left
        }
      }
      else {
        doAnAttack(&wild, curPoke, 0, wild_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon(); continue;} // If PC has more pokemon
          else {battleStatus = 3; continue;} // If PC has no pokemon left
        }

        doAnAttack(curPoke, &wild, 1, cur_move_index);
        if (wild.is_dead) {battleStatus = 2; continue;} // If we killed wild pokemon end battle
      }

      //Reset attack choice key
      attack_key = '0';

    }
    else if (key == 'b') {
      
      if (viewBag()) { // If PC attempts to catch the pokemon
        if (pcd->hasEmptySlot() && pcd->poke_balls > 0) { // If there is room in the party for pokemon and we have a ball
          printw("You can't catch a BOSS pokemon!\n");
        }
        else {
          printw("You can't catch a BOSS pokemon!\n");
        }
      }

      // Opposing pokemon still attacks if bag is accessed

        // Determine what wild pokemon attack to use (random)
        std::string empty = "---";
        if (wild.move_two.identifier == empty) { // If wild only has one move, use it
          wild_move_index = 0;
        }
        else {
          if (rand() % 2) {wild_move_index = 1;}
          else {wild_move_index = 0;}
        }

        doAnAttack(&wild, curPoke, 0, wild_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon();} // If PC has more pokemon
          else {battleStatus = 3; continue;} // If PC has no pokemon left
        }

        getch();

    }
    else if (key == 'r') {

      printw("YOU CANNOT RUN, YOU MUST FACE THE CONSEQUENCES OF YOUR ACTIONS\n");
      getch();

        // Opposing pokemon still attacks if run attempt fails

        // Determine what wild pokemon attack to use (random)
        std::string empty = "---";
        if (wild.move_two.identifier == empty) { // If wild only has one move, use it
          wild_move_index = 0;
        }
        else {
          if (rand() % 2) {wild_move_index = 1;}
          else {wild_move_index = 0;}
        }

        doAnAttack(&wild, curPoke, 0, wild_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (!pcd->allPokemonKnockedOut()) {curPoke = switchPokemon();} // If PC has more pokemon
          else {battleStatus = 3; continue;} // If PC has no pokemon left
        }

    }
    else if (key == 's') {
      curPoke = switchPokemon();
      printWildUi(wild, *curPoke);
      printw("Go, %s!\n", curPoke->name.c_str());
      printw("\nPress any key to continue\n");
      getch();

      // Opposing pokemon still attacks if pokemon is switched

        // Determine what wild pokemon attack to use (random)
        std::string empty = "---";
        if (wild.move_two.identifier == empty) { // If wild only has one move, use it
          wild_move_index = 0;
        }
        else {
          if (rand() % 2) {wild_move_index = 1;}
          else {wild_move_index = 0;}
        }

        doAnAttack(&wild, curPoke, 0, wild_move_index);
        if (curPoke->is_dead) { // If current pokemon in play died
          if (pcd->allPokemonKnockedOut()) {curPoke = switchPokemon();} // If PC has more pokemon
          else {battleStatus = 3; continue;} // If PC has no pokemon left
        }
    }
  }
  erase();

  if (battleStatus == 2) { // If wild pokemon was killed
    printWildUi(wild, *curPoke);
    printw("The opposing %s was defeated!\n", wild.name.c_str());
    printw("\nPress any key to exit\n");
    getch();
    erase();
  }
  else if (battleStatus == 3) { //If all PC pokemon were killed
    printWildUi(wild, *curPoke);
    printw("You're all out of pokemon!\n");
    printw("Game over!\n");
    printw("\nPress any key to exit\n");
    getch();
    gameOver(); // Game is over when all pokemon are dead
  }
  erase();
}

void map::doHack()
{
  erase();

  pcData *pcd;
  pcd = pcData::getInstance();

  pcd->npcsCanMove = 0;

  pcd->pcPokemon[0] = generateZekrom();

  printw("\nSecret hack (for demonstration purposes)\n");
  printw("You now have a level 50 Zekrom.\n");
  printw("NPC movement also toggled off.\n");
  printw("\nPress any key to continue\n");
  getch();
}