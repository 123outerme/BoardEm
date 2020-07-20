#include "CoSprite/csGraphics.h"
#include "CoSprite/csInput.h"
#include "CoSprite/csUtility.h"
//#include "CoSprite/csCurl.h"
//#include "CoSprite/csMap.h"

#include "beGameState.h"
#include "games/Conqueror.h"
//#include "games/Corporation.h"
//#include "games/SnakesLadders.h"

//defines
#define TURNFRAME_QUIT 1
#define TURNFRAME_CONTINUE 0

#define calcWaitTime(x) x == 0 ? 0 : 1000.0 / x

//game functions
int gameLoop(beGameState* gamestate);

cInputState takeTurn(beGameState* gamestate, int playerIndex);
cDoublePt windowCoordToCameraCoord(cDoublePt pt, cCamera camera);

//test functions
int testTurn(beBoard* board, bePlayer* player, cInputState input);
void testUpdate(beBoard* board);
int testWinCon(beBoard* board);
void testBonus(beBoard* board);

//global vars
bool debug;
int framecap = 120;

int main(int argc, char* argv[])
{
    debug = false;
    if (argc > 0)
    {
        if (strcmp(argv[0], "-d") == 0 || strcmp(argv[0], "--debug"))
            debug = true;
    }
    initCoSprite(NULL, "Board Em!", 960, 480, "./assets/Px437_ITT_BIOS_X.ttf", 24, 5, (SDL_Color) {255, 28, 198, 0xFF}, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    //initialize players
    int playerCount = 2;
    bePlayer players[playerCount];
    char* playerNames[2] = {"One", "Two"};
    for(int i = 0; i < playerCount; i++)
    {
        beInitPlayerEmpty(&(players[i]));
        players[i].name = playerNames[i];
    }

    //init the correct board
    beBoard board;
    beConstructGameBoard(&board, players, playerCount, "Conqueror");

    /*
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
        beInitCell(&(cells[0]), points, 3, &tempCtr, (SDL_Color) {0xFF, 0x00, 0x00, 0xFF});
    }


    //initialize board (needs players + cells)
    beInitBoard(&board, players, 2, cells, 1, "./assets/cb.bmp", 20 * 48, 48 * 10, NULL);
    //*/

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
    initCCamera(&testCam, (cDoubleRect) {0, 0, 36, 20}, 1.0, 0.0);
    cScene testScene;
    initCScene(&testScene, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, &testCam, NULL, 0, NULL, 0, (cResource*[1]) {&(gamestate->board->boardResource)}, 1, NULL, 0);
    gamestate->scene = &testScene;
    startTime = SDL_GetTicks();

    while(!quit)
    {
        for(int i = 0; i < gamestate->board->numPlayers; i++)
        {
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
    //in the future: If online and not your turn, let the user look at the map but not change anything
    //also have a separate section for bots being simulated locally
    bool quit = false;
    cInputState state;
    int framerate = 0, sleepFor = 0;
    double targetTime = calcWaitTime(framecap);
    Uint32 lastFrame = SDL_GetTicks();

    while(!quit)
    {
        //test board draw
        drawCScene(gamestate->scene, true, true, &framerate);
        state = cGetInputState(true);

        //intercept, check for generic key options like pausing, quitting, etc.
        if (state.quitInput)
            quit = true;

        if (state.isClick)
        {
            //store the click coords (in px)
            cDoublePt clickPt = (cDoublePt) {state.click.x, state.click.y};

            clickPt = windowCoordToCameraCoord(clickPt, *(gamestate->scene->camera));

            printf("click on: {%f, %f}\n", clickPt.x, clickPt.y);
            //printf("click vs window: {%f, %f}\n", state.click.x * gamestate->scene->camera->rect.w / global.windowW, state.click.y * gamestate->scene->camera->rect.h / global.windowH);
        }

        // WASD camera movement
        if (state.keyStates[SDL_SCANCODE_A])
            gamestate->scene->camera->rect.x -= .1;

        if (state.keyStates[SDL_SCANCODE_D])
            gamestate->scene->camera->rect.x += .1;

        if (state.keyStates[SDL_SCANCODE_W])
            gamestate->scene->camera->rect.y -= .1;

        if (state.keyStates[SDL_SCANCODE_S])
            gamestate->scene->camera->rect.y += .1;

        //Q / E rotation
        if (state.keyStates[SDL_SCANCODE_Q])
            gamestate->scene->camera->degrees -= 1;

        if (state.keyStates[SDL_SCANCODE_E])
            gamestate->scene->camera->degrees += 1;

        //Z / C zoom
        if (state.keyStates[SDL_SCANCODE_Z])
            gamestate->scene->camera->zoom -= .01;

        if (state.keyStates[SDL_SCANCODE_C])
            gamestate->scene->camera->zoom += .01;

        //F1 game board refresh
        if (state.keyStates[SDL_SCANCODE_F1])
        {
            int playerCount = gamestate->board->numPlayers;
            bePlayer* tempPlayers = calloc(playerCount, sizeof(bePlayer));
            for(int i = 0; i < playerCount; i++)
                tempPlayers[i] = gamestate->board->players[i];

            beDestroyBoard(gamestate->board);

            beConstructGameBoard(gamestate->board, tempPlayers, playerCount, "Conqueror");
            free(tempPlayers);
        }

        //F12 framerate
        if (state.keyStates[SDL_SCANCODE_F12])
            printf("Framerate: %d\n", framerate);


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
        if ((sleepFor = targetTime - (SDL_GetTicks() - lastFrame)) > 0)
            SDL_Delay(sleepFor);  //FPS limiter; rests for (16 - time spent) ms per frame, effectively making each frame run for ~16 ms, or 60 FPS
        lastFrame = SDL_GetTicks();
    }

    return state;
}

cDoublePt windowCoordToCameraCoord(cDoublePt pt, cCamera camera)
{
    //add back the offsets
    pt.x += (camera.rect.x * global.windowW / camera.rect.w);
    pt.y += (camera.rect.y * global.windowH / camera.rect.h);

    //rotate the point back around
    pt = rotatePoint(pt, (cDoublePt) {global.windowW / 2, global.windowH / 2}, -1.0 * camera.degrees);

    //un-zoom the points relative to the center of the window
    pt.x = (pt.x - global.windowW / 2.0) / camera.zoom + global.windowW / 2.0;
    pt.y = (pt.y - global.windowH / 2.0) / camera.zoom + global.windowH / 2.0;



    //convert from px to camera scaling units
    pt.x *= camera.rect.w / global.windowW;
    pt.y *= camera.rect.h / global.windowH;

    return pt;
}

int testTurn(beBoard* board, bePlayer* player, cInputState input)
{
    printf(/*"testing turn for player %s\n"*/ "", player->name, board->cellsSize);
    //waitForKey(true);
    //nothing
    if (input.keyStates[SDL_SCANCODE_RETURN])
        return 1;

    return 0;
}

void testUpdate(beBoard* board)
{
    int s = board->cellsSize;
    printf("testing update scores\n", s);
    //nothing
}

int testWinCon(beBoard* board)
{
    int s = board->cellsSize;
    static int i = 0;
    printf("testing win con\n", s);
    return i++;
}

void testBonus(beBoard* board)
{
    int s = board->cellsSize;
    printf("testing bonus from GO\n", s);
    //nothing
}
