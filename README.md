# About

Name: Connor Ferris 

I took this class the fall 2023 semester at Iowa State.

It was taught by Dr. Jeremy Sheaffer.

Throughout the semester, we worked on a Pokemon RogueLike game project, initially written in C, and later ported to C++.

This is version 1.10, after the final assignment of the semester.

# How the game works

## Core mechanics

The game is all displayed to the terminal using the ncurses library. 

You start out with one level one Pokemon, selected at random when you first launch the game.

You start out at map (0,0), in the center of a map array that is 401 x 401 big.

On each map, there are different terrain types, tall grass, short grass, pathways, water, rocks, trees, and exits.

There are also NPCs on every map. Some of them move randomly, some of them don't move, and some of them will pathfind to the player.

The game is turn based. Every time you move, all other NPCs move. They won't move unless you've made a move/input.

When an NPC is adjacent to you, a battle is triggered. There is also a slight chance that a battle with a wild Pokemon is triggered when you walk in tall grass.

The game ends when all your pokemon are knocked out/out of HP.

## Battles

In battles, you can do the following:
- Fight - You can choose one of your Pokemon's two attacks to lower the opposing Pokemon's HP.
- Bag - You can use an item in your bag (e.g. Revive, Potion, Poke Ball)
- Run - Run from the battle, only if it is a wild encounter. Running chance increases/decreases based on your defending Pokemon's speed compared to the wild Pokemon.
- Switch - Switch out the current Pokemon you have in battle for another one in your party.

In trainer battles, you battle the Pokemon that a trainer has. A trainer can have anywhere from 1-6 Pokemon. 

In wild battles, you battle a single wild Pokemon, that you may also catch if you have empty slots in your party.

Opponent pokemon levels are within the range of Manhattan distance from the origin map (0,0), so the farther you go from the origin map, the higher levels you will encounter.

Higher leveled Pokemon have higher stats, HP, and their attacks do more damage.

Normal Pokemon rules of STAB, Critical Hits, missed attacks, etc. all apply.

## Fishing

For our last assignment/update to the game, we could either make a new project, or add something of our own new to this game. I chose the latter, and implemented a fishing minigame with some new mechanics!

You can enter fishing mode if you are adjacent to water. You can cast your rod as many times as you like until you decide to exit.

For each case of your rod, the following situations may occur, with a certain percent chance of happening:

- 40%: Fish up one of three fish items that gets added to your bag.
  - Small Fry: Restores 10 HP
  - Flopper: Restores 50 HP
  - Shield Fish: Restores Pokemon to max HP

- 25%: Fish up a standard item, a Potion, Poke Ball, or Revive.

- 10%: Encounter the robber fish. THe Robber fish will ignore you if you have less than 3 fish/bag items. However, if you have more than this (which is likely), the robber fish will steal 1-3 of the items in your bag.

- 5%: Fish up a pipe bomb, which explodes and damages all Pokemon in your party for 10% of their max HP.

- 5%: Fish up one of two Blessed Sea Gems:
  - Emerald: The first Pokemon in your party has all their base stats increased.
  - Diamond: All your Pokemon in your party are revived if knocked out, and restored to full health (same effect as Pokemon Center)

- 5%: Add a Technical Machine to your TM case. Your TM case can accessed from your bag, and you can hold up to six TMs at once. A technical machine stores a random attack, and you can make one of your Pokemon forget one of their attacks to learn the attack listed on the TM. Once you teach one of your Pokemon the move on the TM, the TM is destroyed and can no longer be used. This also frees a slot in your TM case.

- 5%: Fish up water walking boots. You can now walk on water, which was previously not possible. If you fish up the boots more than once, nothing happens.

- 4%: A battle is initiated with a Wailord Boss. A wild encounter is started with a Wailord that is 10 levels higher than the current Manhattan value (i.e if tall grass pokemon are level 1, Boss is level 10). You cannot catch a Wailord Boss, and you cannot run from the battle. 

- 1%: An exceedingly rare battle is started with the KRAKEN. The KRAKEN is level 75. The Kraken is an extremely fast version of the Pokemon Malamar. The Kraken knows two moves: flail (does 0 damage), and WRATH OF THE OCEAN GOD, which does insane damage and will surely one hit kill your Pokemon. This encounter will certainly end your game and serves as something to discourage players from fishing too much.

## Controls

- 1-8: Move the PC (player) a certain direction
- b: Access bag
- i: View Pokemon in your party with their stats/attacks/current HP
- o: Fish
- ">": Enter Pokemon Center or PokeMart
- "<": Exit
- "+": Secret hack that turns NPC movement off and gives you a level 50 Zekrom (for testing purposes)
- Q: Quit the game

# Important Notes

Dr Sheaffer released his code after every assignment was due, and students were allowed to build off his code for the next assignment. I elected NOT to do this for any assignment! All code included in this repository is 100% mine, made from scratch, you name it.

As state earlier, this code started in C, but was later ported to C++.

There are many unomptimized routines in my code. It would take me some time to optimize my code (maybe I'll do it eventually?) My code quality definitely improved over time, but some of my early code is quite frankly a mess. Some routines I would improve if I had to write this again:

- Map generation (buildings spawn in odd places, algorithm is damn near unreadable, segfaults happen on rare occasion)

- Moving from map to map (segfaults way more common than I would like)

- Flying from map to map. I never had this working right, so I didn't include it.

- Overall organization. Map.cpp is 3500+ lines and it should be split among multiple files.

If you want to run this game, you'll need 9 Pokemon related .CSVs, the game initially accessed these stored on a /share folder on Iowa State's computer science Pyrite server. You can tweak the file paths in database.h.

I really enjoyed working on this project, I think it would be fun to make my own game from scratch.
