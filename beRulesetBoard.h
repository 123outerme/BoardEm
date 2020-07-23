#ifndef BEBOARDGAME_H_INCLUDED
#define BEBOARDGAME_H_INCLUDED

#include "CoSprite/csGraphics.h"
#include "CoSprite/csInput.h"
#include "CoSprite/csUtility.h"
#include "bePlayer.h"
#include <stdlib.h>

#define INFO_MAX_LENGTH 2

#define BE_PHASE_INIT 0
#define BE_PHASE_SETUP 1
#define BE_PHASE_PLAY 2

typedef struct _beCell
{
    char* name;
    cDoublePt* points;  /**< A dynamically allocated array of the points that make up a "cell" or board square */
    int ptsSize;  /**< The length of `points` */
    cDoublePt center;  /**< The automatically-filled center. Used for drawing piece movement */
    double radius;
    SDL_Color outlineColor;  /**< The color that will be used for the outline when drawn */
} beCell;

typedef struct _beBoard
{
    char* name;  /**< Name of the board */
    bePlayer* players;  /**< Dynamically allocated array of current players */
    int numPlayers;  /**< Number of players (max 4) */
    bool (*checkMovement)(bePiece, int);  /**< Function that takes in the current board and the player to be moved, checking validity of player movement and applying it if so */
    cResource boardResource;  /**< The CResource that helps us draw the board. */
    beCell* cells; /**< Array of cells */
    int cellsSize;  /**< The length of `cells` */
    cSprite bgImage;  /**< The background image in cSprite form */
    SDL_Color bgColor; /**< The color the background has */
    int width;  /**< The width (in px) of the board */
    int height;  /**< The height (in px) of the board */
    int gamePhase;  /**< 0 - init phase, 1 - setup phase, 2 - play phase */
} beBoard;

typedef struct _beRuleset
{
    void* subclass;  /**< The subclass data for the ruleset */
    void (*gameSetup)(beBoard*);  /**< Takes a ptr to the board */
    int (*playerTurnFrame)(beBoard*, bePlayer*, cInputState);  /**< Output 1 for quit, 0 for continue */
    void (*updateScores)(beBoard*);
    int (*checkWin)(beBoard*);  /**< checks the win condition(s). Outputs 1-4 if that # player won, otherwise outputs 0 */
    void (*applyMoneyGameBonus)(beBoard*);  /**< If landing on "GO" in "Monopoly"-type game, apply this bonus */
    void (*freeSubclass)(void*);  /**< Cleanup function for subclass */
} beRuleset;

void beInitCell(beCell* cell, char* name, cDoublePt* pts, int ptsSize, cDoublePt* center, double radius, SDL_Color outlineColor);
void beInitBoard(beBoard* board, bePlayer* players, int numPlayers, beCell* cells, int cellsSize, char* bgImgPath, int w, int h, bool (*checkMovement)(bePiece, int));
void beConstructGameBoard(beBoard* board, bePlayer* players, int playerCount, char* folderName, bool (*checkMovement)(bePiece, int));
void beInitRuleset(beRuleset* ruleset, void* subclass,
                    void (*gameSetup)(beBoard*),
                    int (*playerTurnFrame)(beBoard*, bePlayer*, cInputState),
                    void (*updateScores)(beBoard*),
                    int (*checkWin)(beBoard*),
                    void (*applyMoneyGameBonus)(beBoard*),
                    void (*freeSubclass)(void*));

//functions that allows player to select a specific cell (for movement, information, etc.)
int beCheckMapClick(beBoard* board, cCamera camera, cDoublePt click);
bool beCheckCellClick(beCell cell, cDoublePt click, cCamera camera);

//void beDestroyCell(beCell* cell, int ptsSize);
void beDestroyBoard(beBoard* board);
void beDestroyRuleset(beRuleset* ruleset);

//CoSprite function pointer "targets"
void beDrawBoardCoSprite(void* ptrBoard, cCamera camera);
void beDestroyBoardCoSprite(void* ptrBoard);
#endif // BEBOARDGAME_H_INCLUDED
