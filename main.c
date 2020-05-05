#include "CoSprite/csGraphics.h"
#include "CoSprite/csInput.h"
#include "CoSprite/csUtility.h"
//#include "CoSprite/csCurl.h"
//#include "CoSprite/csMap.h"

#include "beGameState.h"
//#include "Games/Conqueror.h"
//#include "Games/Corporation.h"
//#include "Games/SnakesLadders.h"

#define TURNFRAME_QUIT 1
#define TURNFRAME_CONTINUE 0

//game functions
int gameLoop(beGameState* gamestate);

cInputState takeTurn(beGameState* gamestate, int playerIndex);

//test functions
int testTurn(beBoard* board, bePlayer* player, cInputState input);
void testUpdate(beBoard* board);
int testWinCon(beBoard* board);
void testBonus(beBoard* board);


int main(int argc, char* argv[])
{
    initCoSprite(NULL, "Hello World", 960, 480, "./assets/Px437_ITT_BIOS_X.ttf", 24, 5, (SDL_Color) {255, 28, 198, 0xFF}, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    //initialize players
    int playerCount = 2;
    bePlayer players[playerCount];
    char* playerNames[2] = {"One", "Two"};
    for(int i = 0; i < playerCount; i++)
    {
        beInitPlayerEmpty(&(players[i]));
        players[i].name = playerNames[i];
    }

    //initialize cells
    int pointCount = 3;
    cDoublePt* points = calloc(pointCount, sizeof(cDoublePt));
    points[0] = (cDoublePt) {1, 1};
    points[1] = (cDoublePt) {2, 2};
    points[2] = (cDoublePt) {2, 1};

    int cellCount = 1;
    beCell cells[cellCount];
    {
        cDoublePt tempCtr = {1.75, 1.33};
        beInitCell(&(cells[0]), points, 3, NULL, 0, &tempCtr, (SDL_Color) {0xFF, 0x00, 0x00, 0xFF});
    }


    //initialize board (needs players + cells)
    beBoard board;
    beInitBoard(&board, players, 2, cells, 1, "./assets/cb.bmp", 20 * 48, 48 * 10, NULL);

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

    /*
    //testing cGetInputState
    bool quit = false;
    cInputState state;
    while(!quit)
    {
        state = cGetInputState(true);
        if (state.isClick)
        {
            printf("coords (%d, %d)\n", state.click.x, state.click.y);
            quit = true;
        }
        if (state.keyStates[SDL_SCANCODE_F] && state.keyStates[SDL_SCANCODE_G])
        {
            printf("printfs in chat fellas\n");
            quit = true;
        }

        if (state.quitInput)
            quit = true;
    }
    */

    closeCoSprite();
    return 0;
}

int gameLoop(beGameState* gamestate)
{
    bool quit = false;
    int returnCode = 0;

    //test init; not sure how I should handle drawing the board
    cCamera testCam;
    initCCamera(&testCam, (cDoubleRect) {0, 0, 18, 10}, 1.0, 0.0);
    cScene testScene;
    initCScene(&testScene, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, &testCam, NULL, 0, NULL, 0, (cResource*[1]) {&(gamestate->board->boardResource)}, 1, NULL, 0);

    while(!quit)
    {
        for(int i = 0; i < gamestate->board->numPlayers; i++)
        {
            //test board draw
            drawCScene(&testScene, true, true);

            //for all players, allow players to perform their turn
            cInputState state = takeTurn(gamestate, i);
            if (state.quitInput)
            {
                //force quit; shut down online (if any), pass host (if necessary)
                printf("force quitting\n");
                quit = true;
                break;
            }
            //gamestate->ruleset->playerTurnFrame(gamestate->board, &(gamestate->board->players[i]));

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

cInputState takeTurn(beGameState* gamestate, int playerIndex)
{
    bool quit = false;
    cInputState state;
    while(!quit)
    {
        state = cGetInputState(true);

        //intercept, check for generic key options like pausing, quitting, etc.
        if (state.quitInput)
            quit = true;


        //pass to customized turn stuff
        if (!(gamestate->board->players[playerIndex].isAI) && gamestate->board->players[playerIndex].isLocal)
        {  //if it's the local human player's turn
            int code = gamestate->ruleset->playerTurnFrame(gamestate->board, &(gamestate->board->players[playerIndex]), state);
            if (code == TURNFRAME_QUIT)
            {
                printf("--pressed Enter\n");
                quit = true;
            }
        }
        else
        {
            if (!(gamestate->board->players[playerIndex].isLocal))
            {
                //wait for online turn to be over
            }
            else
            {
                if (gamestate->board->players[playerIndex].isAI)
                {
                    //calculate AI moves
                }
            }
        }
    }

    return state;
}

int testTurn(beBoard* board, bePlayer* player, cInputState input)
{
    printf("testing turn for player %s\n", player->name);
    //waitForKey(true);
    //nothing
    if (input.keyStates[SDL_SCANCODE_RETURN])
        return 1;

    return 0;
}

void testUpdate(beBoard* board)
{
    printf("testing update scores\n");
    //nothing
}

int testWinCon(beBoard* board)
{
    static int i = 0;
    printf("testing win con\n");
    return i++;
}

void testBonus(beBoard* board)
{
    printf("testing bonus from GO\n");
    //nothing
}
