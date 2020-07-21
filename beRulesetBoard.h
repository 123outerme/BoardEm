#ifndef BEBOARDGAME_H_INCLUDED
#define BEBOARDGAME_H_INCLUDED

#include "CoSprite/csGraphics.h"
#include "CoSprite/csInput.h"
#include "CoSprite/csUtility.h"
#include "bePlayer.h"
#include <stdlib.h>

#define INFO_MAX_LENGTH 2

/*
typedef struct _beCell
{
    cDoublePt* points;  / **< A dynamically allocated array of the points that make up a "cell" or board square * /
    int ptsSize;  / **< The length of `points` * /
    cDoublePt center;  / **< The automatically-filled center. Used for drawing piece movement * /
    SDL_Color outlineColor;  / **< The color that will be used for the outline when drawn * /
} beCell;
//*/

typedef struct _beBoard
{
    char* name; /**< Name of the board */
    bePlayer* players;  /**< Dynamically allocated array of current players */
    int numPlayers;  /**< Number of players (max 4) */
    bool (*checkMovement)(bePiece, int);  /**< Function that takes in the current board and the player to be moved, checking validity of player movement and applying it if so */
    cResource boardResource;  /**< The CResource that helps us draw the board. */
    //start cells data
    cDoublePt** cells;  /**< A dynamically allocated array of `cDoublePt[]`s that construct the board */
    int cellsSize;  /**< The length of `cells` */
    int* ptsSize;  /**< The length of each individual cell */
    cDoublePt* centers;  /**< The centers of each cell */
    double* radii; /**< The distance from the center from which to further consider a click */
    SDL_Color* outlines;  /**< The outline color for each cell */
    char** names;  /**< The name data for each cell */
    //end cells data
    cSprite bgImage;  /**< The background image in cSprite form */
    SDL_Color bgColor; /**< The color the background has */
    int width;  /**< The width (in px) of the board */
    int height;  /**< The height (in px) of the board */

} beBoard;

typedef struct _beRuleset
{
    void* subclass; /**< The subclass data for the ruleset */
    int (*gameSetup)(beBoard*, bePlayer*); /**< Outputs the index of which player starts */
    int (*playerTurnFrame)(beBoard*, bePlayer*, cInputState);  /**< Output 1 for quit, 0 for continue */
    void (*updateScores)(beBoard*);
    int (*checkWin)(beBoard*); /**<  */
    void (*applyMoneyGameBonus)(beBoard*);  /**< If landing on "GO" in "Monopoly"-type game, apply this bonus */
    void (*freeSubclass)(void*); /**< Cleanup function for subclass */
} beRuleset;

//void beInitCell(beCell* cell, cDoublePt* pts, int cellsSize, cDoublePt* center, SDL_Color outlineColor);
void beInitBoard(beBoard* board, bePlayer* players, int numPlayers, cDoublePt** cells, int cellsSize, char* bgImgPath, int w, int h, bool (*checkMovement)(bePiece, int));
void beConstructGameBoard(beBoard* board, bePlayer* players, int playerCount, char* folderName);
void beInitRuleset(beRuleset* ruleset, void* subclass,
                    int (*playerTurnFrame)(beBoard*, bePlayer*, cInputState),
                    void (*updateScores)(beBoard*),
                    int (*checkWin)(beBoard*),
                    void (*applyMoneyGameBonus)(beBoard*),
                    void (*freeSubclass)(void*));

//functions that allows player to select a specific cell (for movement, information, etc.)
int beCheckMapClick(beBoard* board, cCamera camera, cDoublePt click);
bool beCheckCellClick(beBoard* board, cCamera camera, int cellIndex, cDoublePt click);
cDoublePt windowCoordToCameraCoord(cDoublePt pt, cCamera camera);
cDoublePt cameraCoordToWindowCoord(cDoublePt pt, cCamera camera);

//void beDestroyCell(beCell* cell, int ptsSize);
void beDestroyBoard(beBoard* board);
void beDestroyRuleset(beRuleset* ruleset);

//CoSprite function pointer "targets"
void beDrawBoardCoSprite(void* ptrBoard, cCamera camera);
void beDestroyBoardCoSprite(void* ptrBoard);

#endif // BEBOARDGAME_H_INCLUDED
