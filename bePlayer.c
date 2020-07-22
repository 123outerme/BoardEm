#include "bePlayer.h"

/** \brief Creates a bePiece
 *
 * \param piece bePiece*
 * \param id int
 * \param sprite cSprite
 * \param locationIndex int
 */
void beInitPiece(bePiece* piece, int num, cSprite sprite, int locationIndex)
{
    piece->num = num;
    piece->sprite = sprite;
    piece->locationIndex = locationIndex;
}

/** \brief Creates a generic player
 *
 * \param player bePlayer*
 * \param name char*
 * \param pieces bePiece*
 * \param numPieces int
 * \param isLocal bool
 * \param isAI bool
 * \param active bool
 * \param subclass void*
 * \param void (*freeSubclass)(void*)
 */
void beInitPlayer(bePlayer* player, char* name, bePiece* pieces, int numPieces, bool isLocal, bool isAI, bool active, void* subclass, void (*freeSubclass)(void*))
{
    int nameSize = strlen(name);
    player->name = calloc(nameSize + 1, sizeof(char));
    strncpy(player->name, name, nameSize);

    player->score = 0;

    if (pieces != NULL && numPieces > 0)
    {
        player->pieces = calloc(numPieces, sizeof(bePiece));
        memcpy(player->pieces, pieces, numPieces * sizeof(bePiece));
        player->numPieces = numPieces;
    }
    else
    {
        player->pieces = NULL;
        player->numPieces = 0;
    }

    player->isLocal = isLocal;
    player->isAI = isAI;
    player->active = active;

    player->subclass = subclass;
    player->freeSubclass = freeSubclass;
}

/** \brief Creates an empty bePlayer
 *
 * \param player bePlayer*
 */
void beInitPlayerEmpty(bePlayer* player)
{
    beInitPlayer(player, "\0", NULL, 0, true, false, false, NULL, NULL);
}

/** \brief Creates a human player
 *
 * \param player bePlayer*
 * \param name char*
 * \param pieces bePiece*
 * \param numPieces int
 * \param isLocal bool
 * \param subclass void*
 * \param void (*freeSubclass)(void*)
 */
void beInitPlayerHuman(bePlayer* player, char* name, bePiece* pieces, int numPieces, bool isLocal, void* subclass, void (*freeSubclass)(void*))
{
    beInitPlayer(player, name, pieces, numPieces, isLocal, false, true, subclass, freeSubclass);
}

/** \brief Creates an AI player
 *
 * \param ai bePlayer*
 * \param name char*
 * \param pieces bePiece*
 * \param numPieces int
 * \param isLocal bool
 * \param subclass void*
 * \param void (*freeSubclass)(void*)
 */
void beInitPlayerAI(bePlayer* ai, char* name, bePiece* pieces, int numPieces, bool isLocal, void* subclass, void (*freeSubclass)(void*))
{
    beInitPlayer(ai, name, pieces, numPieces, isLocal, true, true, subclass, freeSubclass);
}

/** \brief Clears all memory from a Player
 *
 * \param player bePlayer*
 */
void beDestroyPlayer(bePlayer* player)
{
    free(player->name);
    player->name = NULL;

    free(player->pieces);
    player->pieces = NULL;
    player->numPieces = 0;

    player->isLocal = false;
    player->isAI = false;
    player->active = false;

    if (player->freeSubclass != NULL)
        player->freeSubclass(player->subclass);

    player->subclass = NULL;
    player->freeSubclass = NULL;
}
