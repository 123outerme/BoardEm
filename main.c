#include "CoSprite/csGraphics.h"
#include "CoSprite/csInput.h"
#include "CoSprite/csUtility.h"
//#include "CoSprite/csCurl.h"
//#include "CoSprite/csMap.h"

#include "beGameState.h"


//game functions
int gameLoop(beGameState* gamestate);


//test functions
void testTurn(beBoard* board, bePlayer* player);
void testUpdate(beBoard* board);
int testWinCon(beBoard* board);
void testBonus(beBoard* board);


int main(int argc, char* argv[])
{
    initCoSprite(NULL, "Hello World", 1080, 960, "./assets/Px437_ITT_BIOS_X.ttf", 24, 5, (SDL_Color) {255, 28, 198, 0xFF}, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    //initialize players
    int playerCount = 2;
    bePlayer players[playerCount];
    for(int i = 0; i < playerCount; i++)
    {
        beInitPlayerEmpty(&(players[i]));
    }

    //initialize cells
    int pointCount = 3;
    cDoublePt* points = calloc(pointCount, sizeof(cDoublePt));
    points[0] = (cDoublePt) {1, 1};
    points[1] = (cDoublePt) {2, 2};
    points[2] = (cDoublePt) {2, 1};

    int cellCount = 1;
    beCell cells[cellCount];
    beInitCell(&(cells[0]), points, 3, NULL, 0);


    //initialize board (needs players + cells)
    beBoard board;
    beInitBoard(&board, players, 2, NULL, cells, 1, 0, 0, NULL);

    //initialize ruleset
    beRuleset rules;
    beInitRuleset(&rules, &testTurn, &testUpdate, &testWinCon, &testBonus);

    //initialize gamestate (needs ruleset + board)
    beGameState gamestate;
    beInitGameState(&gamestate, &rules, &board);

    //start game loop
    gameLoop(&gamestate);

    //de-initialize

    //temp pause
    //waitForKey(true);

    beDestroyGameState(&gamestate);
    closeCoSprite();
    return 0;
}

int gameLoop(beGameState* gamestate)
{
    bool quit = false;
    int returnCode = 0;
    while(!quit)
    {
        for(int i = 0; i < gamestate->board->numPlayers; i++)
        {
            gamestate->ruleset->playerTurn(gamestate->board, &(gamestate->board->players[i]));
            //for all players, allow players to perform their turn

            //check gamestate to update and/or draw scores
            gamestate->ruleset->updateScores(gamestate->board);

            //check for a winner
            if (gamestate->ruleset->checkWin(gamestate->board) != 0)
            {
                printf("quitting\n");
                quit = true;
                break;
            }
        }
    }
    return returnCode;
}

void testTurn(beBoard* board, bePlayer* player)
{
    printf("testing turn\n");
    //nothing
}

void testUpdate(beBoard* board)
{
    printf("testing update scores\n");
    //nothing
}

int testWinCon(beBoard* board)
{
    printf("testing win con\n");
    return 1;
}

void testBonus(beBoard* board)
{
    printf("testing bonus from GO\n");
    //nothing
}
