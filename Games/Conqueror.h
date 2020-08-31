#ifndef CONQUEROR_H_INCLUDED
#define CONQUEROR_H_INCLUDED

#include "../beRulesetBoard.h"

/*
typedef struct _beConqueror
{
    //..
} beConqueror;
//*/

typedef struct _beConquerorArmy
{
    int reinforcements;
    SDL_Color color;
} beConquerorArmy;

//board
bool conquerorCheckMovement(bePiece piece, int cellId);

//ruleset
void conquerorGameSetup(beBoard* board);
void conquerorApplyCorpBonus(bePlayer* player);
void conquerorApplyMovement(beBoard* board, bePlayer* player, bePiece* piece, int collidedIndex);

//player
void initConquerorArmy(beConquerorArmy* army, int reinforcements, SDL_Color color);
void destroyConquerorArmy(void* armyPtr);

#endif // CONQUEROR_H_INCLUDED
