#ifndef BEPLAYER_H_INCLUDED
#define BEPLAYER_H_INCLUDED

#include "CoSprite/csGraphics.h"
#include <stdlib.h>

typedef struct _beAction
{
    //how the hell do I even do this
    int typeCode;  /**< 0: movement, ... */
} beAction;

typedef struct _bePiece
{
    int num; /**< Either ID num, or num of troops */
    cSprite sprite;
    int locationIndex;
} bePiece;

typedef struct _bePlayer
{
    char* name;  /**< Especially useful for multiplayer */
    int score;
    bePiece* pieces;  /**< Array of pieces this player owns */
    int numPieces;  /**< Size of `pieces` */
    beAction* actions;  /**< Array of actions this player has taken this turn */
    int numActions;  /**< Size of `actions` */
    bool isLocal;  /**< if false, they are an online representation of another player */
    bool isAI;  /**< if true, is an AI player */
    bool active;  /**< if false, player is disabled */
    void* subclass;  /**< Whatever data that relates specifically to the current ruleset */
    void (*freeSubclass)(void*);  /**< Function that can be used in beDestroyPlayer to free the subclass data */
} bePlayer;

void beInitPiece(bePiece* piece, int num, cSprite sprite, int locationIndex);
void beInitPlayer(bePlayer* player, char* name, bePiece* pieces, int numPieces, bool isLocal, bool isAI, bool active, void* subclass, void (*freeSubclass)(void*));
void beInitPlayerEmpty(bePlayer* player);
void beInitPlayerHuman(bePlayer* player, char* name, bePiece* pieces, int numPieces, bool isLocal, void* subclass, void (*freeSubclass)(void*));
void beInitPlayerAI(bePlayer* ai, char* name, bePiece* pieces, int numPieces, bool isLocal, void* subclass, void (*freeSubclass)(void*));

void beDestroyPlayer(bePlayer* player);

#endif // BEPLAYER_H_INCLUDED
