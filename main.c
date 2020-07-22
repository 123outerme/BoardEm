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
    initCoSprite(NULL, "Board Em!", 960, 480, "./assets/Px437_ITT_BIOS_X.ttf", 12, 5, (SDL_Color) {255, 28, 198, 0xFF}, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    //initialize players
    int playerCount = 2;
    bePlayer players[playerCount];
    char* playerNames[2] = {"One", "Two"};
    for(int i = 1; i < playerCount; i++)
    {
        beInitPlayerEmpty(&(players[i]));
        players[i].name = playerNames[i];
    }
    //initialize test piece
    bePiece testPiece;
    cSprite pieceSprite;
    initCSprite(&pieceSprite, NULL, "./assets/piece.png", 0, (cDoubleRect) {0, 0, 1, 1}, (cDoubleRect) {0, 0, 24, 24}, NULL, 0.5, SDL_FLIP_NONE, 0, false, NULL, 5);
    beInitPiece(&testPiece, 0, pieceSprite, 0);
    beInitPlayer(&(players[0]), playerNames[0], &testPiece, 1, true, false, true, NULL, NULL);

    //init the correct board
    beBoard board;
    beConstructGameBoard(&board, players, playerCount, "Conqueror");

    //initialize ruleset
    beRuleset rules;
    beInitRuleset(&rules, NULL, &testTurn, &testUpdate, &testWinCon, &testBonus, NULL);

    //initialize gamestate (needs ruleset + board)
    beGameState gamestate;
    beInitGameState(&gamestate, &rules, &board);

    //start game loop
    gameLoop(&gamestate);

    //temp pause
    //waitForKey(true);

    //de-initialize
    beDestroyGameState(&gamestate);

    closeCoSprite();
    return 0;
}

int gameLoop(beGameState* gamestate)
{
    bool quit = false;
    int returnCode = 0;

    //test init; not sure how I should handle drawing the board
    cCamera testCam;
    initCCamera(&testCam, (cDoubleRect) {0, 0, 40, 20}, 1.0, 0.0);
    //36 x-camera-coords for game board display, the last 4 are for scores/info (theoretically)
    cScene testScene;
    initCScene(&testScene, gamestate->board->bgColor, &testCam, NULL, 0, NULL, 0, (cResource*[1]) {&(gamestate->board->boardResource)}, 1, NULL, 0);
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
    bePiece* pickedPiece = NULL;

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

            //printf("collected click: {%f, %f}\n", clickPt.x, clickPt.y);

            //convert for making new cells and checking clicks on cells
            clickPt = cWindowCoordToCameraCoord(clickPt, *(gamestate->scene->camera));

            //printf("click on: {%f, %f}\n", clickPt.x, clickPt.y);
            //printf("click vs window: {%f, %f}\n", state.click.x * gamestate->scene->camera->rect.w / global.windowW, state.click.y * gamestate->scene->camera->rect.h / global.windowH);

            int collidedIndex = beCheckMapClick(gamestate->board, *(gamestate->scene->camera), clickPt);

            if (collidedIndex >= 0)
            {
                printf("Clicked on %s (%d)\n", gamestate->board->cells[collidedIndex].name, collidedIndex);
                if (pickedPiece == NULL)
                {
                    //if you clicked on a board with a piece, set it to pickedPiece
                    for(int i = 0; i < gamestate->board->numPlayers; i++)
                    {
                        for(int x = 0; x < gamestate->board->players[i].numPieces; x++)
                        {
                            if (gamestate->board->players[i].pieces[x].locationIndex == collidedIndex)
                            {
                                pickedPiece = &(gamestate->board->players[i].pieces[x]);
                                printf("found piece %d from player %d\n", x, i);
                            }
                        }
                    }
                }
                else
                {
                    //check if move works. If so, do it, otherwise, don't do it. Either way reset pickedPiece
                    if (gamestate->board->checkMovement(*pickedPiece, collidedIndex))
                    {
                        pickedPiece->locationIndex = collidedIndex;
                        printf("moved piece\n");
                    }
                    else
                        printf("movement failed\n");

                    pickedPiece = NULL;
                }
            }
            else
                printf("No click collision\n");
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

        //F2 reset zoom, rotation, and offsets
        if (state.keyStates[SDL_SCANCODE_F2])
        {
            gamestate->scene->camera->zoom = 1.0;
            gamestate->scene->camera->degrees = 0;
            gamestate->scene->camera->rect.x = 0;
            gamestate->scene->camera->rect.y = 0;
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
