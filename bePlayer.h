#ifndef BEPLAYER_H_INCLUDED
#define BEPLAYER_H_INCLUDED

#include "CoSprite/csGraphics.h"
#include <stdlib.h>

typedef struct _bePlayer
{
    char* name;  /**< Especially useful for multiplayer */
    int score;
    cSprite sprite;
    bool isLocal;  /**< if false, they are an online representation of another player */
    bool isAI;  /**< if true, is an AI player */
    bool active;  /**< if false, player is disabled */
    void* subclass;  /**< Whatever data that relates specifically to the current ruleset */
    void (*freeSubclass)(void*);  /**< Function that can be used in beDestroyPlayer to free the subclass data */
} bePlayer;

void beInitPlayer(bePlayer* player, char* name, bool isLocal, bool isAI, bool active, void* subclass, void (*freeSubclass)(void*));
void beInitPlayerEmpty(bePlayer* player);
void beInitPlayerHuman(bePlayer* player, char* name, bool isLocal, void* subclass, void (*freeSubclass)(void*));
void beInitPlayerAI(bePlayer* ai, char* name, bool isLocal, void* subclass, void (*freeSubclass)(void*));

void beDestroyPlayer(bePlayer* player);

#endif // BEPLAYER_H_INCLUDED
