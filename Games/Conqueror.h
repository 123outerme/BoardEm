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
} beConquerorArmy;

//board
bool conquerorCheckMovement(bePiece piece, int cellId);

//ruleset
void conquerorGameSetup(beBoard* board);
void conquerorApplyCorpBonus(bePlayer* player);
void conquerorApplyMovement(beBoard* board, bePlayer* player, bePiece* piece, int destCellIndex);
void conquerorUpdateScores(beBoard* board);
int conquerorCheckWin(beBoard* board);

//player
void initConquerorArmy(beConquerorArmy* army, int reinforcements);
void destroyConquerorArmy(void* armyPtr);

#endif // CONQUEROR_H_INCLUDED
