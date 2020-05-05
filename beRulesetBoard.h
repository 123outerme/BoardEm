#ifndef BEBOARDGAME_H_INCLUDED
#define BEBOARDGAME_H_INCLUDED

#include "CoSprite/csGraphics.h"
#include "CoSprite/csInput.h"
#include "bePlayer.h"
#include <stdlib.h>

typedef struct _beCell
{
    cDoublePt* points;  /**< A dynamically allocated array of the points that make up a "cell" or board square */
    int ptsSize;  /**< The length of `points` */
    struct _beCell* neighbors;  /**< Touching cells allocated clockwise from this cell starting from 0 = most North (favoring East) */
    int neighborsSize;  /**< The length of `neighbors` */
    bePlayer* housedPlayer;  /**< The player that currently resides in this cell (NULL if none) */
    cDoublePt center;  /**< The automatically-filled center. Used for drawing piece movement */
    SDL_Color outlineColor;  /**< The color that will be used for the outline when drawn */
} beCell;

typedef struct _beBoard
{
    bePlayer* players;  /**< Dynamically allocated array of current players */
    int numPlayers;  /**< Number of players (max 4) */
    void (*applyPlayerMovement)(struct _beBoard*, bePlayer*);  /**< Function that takes in the current board and the player to be moved, checking validity of player movement if local. */
    cResource boardResource;  /**< The CResource that helps us draw the board. */
    beCell* cells;  /**< A dynamically allocated array of `beCell`s that construct the board */
    int cellsSize;  /**< The length of `cells` */
    cSprite bgImage;  /**< The background image in cSprite form */
    int width;  /**< The width (in px) of the board */
    int height;  /**< The height (in px) of the board */

} beBoard;

typedef struct _beRuleset
{
    int (*playerTurnFrame)(beBoard*, bePlayer*, cInputState);  /**< Output 1 for quit, 0 for continue */
    void (*updateScores)(beBoard*);
    int (*checkWin)(beBoard*);
    void (*applyMoneyGameBonus)(beBoard*);  /**< If landing on "GO" in "Monopoly"-type game, apply this bonus */
} beRuleset;

void beInitCell(beCell* cell, cDoublePt* pts, int cellsSize, beCell* neighbors, int neighborsSize, cDoublePt* center, SDL_Color outlineColor);
void beInitBoard(beBoard* board, bePlayer* players, int numPlayers, beCell* cells, int cellsSize, char* bgImgPath, int w, int h, void (*applyPlayerMovement)(struct _beBoard*, bePlayer*));
void beInitRuleset(beRuleset* ruleset,
                    int (*playerTurnFrame)(beBoard*, bePlayer*, cInputState),
                    void (*updateScores)(beBoard*),
                    int (*checkWin)(beBoard*),
                    void (*applyMoneyGameBonus)(beBoard*));

//function that allows player to select a specific cell (for movement, information, etc.)

void beDestroyCell(beCell* cell);
void beDestroyBoard(beBoard* board);
void beDestroyRuleset(beRuleset* ruleset);

//CoSprite function pointer "targets"
void beDrawBoardCoSprite(void* ptrBoard, cCamera camera);
void beDestroyBoardCoSprite(void* ptrBoard);

#endif // BEBOARDGAME_H_INCLUDED
