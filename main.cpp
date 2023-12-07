#include "game.h"

/**
 * Main method for game. Creates a game object to run game off of
*/
int main(int argc, char *argv[]) 
{
    game *G = new game(); // Start game
    G->start();

    /* Loop that runs game unless player hits Q to quit*/
    while (1) {
        G->printCurrentMap();
        char c = getch();

        if (c == 'Q') {
            break;
        }
        else if (c == 'f') {
            //G->getFly();
        }
        else {
            G->doTurn(c);
            G->checkIfOnGate();
            refresh();
        }
        
    }

    G->end(); // End game
    delete G; 

    return 0;
}