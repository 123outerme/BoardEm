#ifndef BEGAMESTATE_H_INCLUDED
#define BEGAMESTATE_H_INCLUDED

#include "beRulesetBoard.h"

#define BE_OFFLINE 0
#define BE_PEER 1
#define BE_HOST 2

typedef struct _beGameState
{
    beRuleset* ruleset;  /**< Keeps track of the current Ruleset */
    beBoard* board;  /**< Keeps track of the current Board */
    cScene* scene;  /**< Context for keypresses, etc. */
    int onlineState; /**< 0 - offline, 1 - non-host, 2 - host */
} beGameState;

void beInitGameState(beGameState* gamestate, beRuleset* ruleset, beBoard* board);

char* beExportTurnData(beGameState* gamestate);

void beDestroyGameState(beGameState* gamestate);

char* exportedTurnData; //out here bc returning local pointers is sketchy

#endif // BEGAMESTATE_H_INCLUDED
