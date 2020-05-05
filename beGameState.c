#include "beGameState.h"

/** \brief Initializes a new gamestate.
 *
 * \param gamestate beGameState*
 * \param ruleset beRuleset*
 * \param board beBoard*
 * \param players bePlayer*
 * \param numPlayers int
 */
void beInitGameState(beGameState* gamestate, beRuleset* ruleset, beBoard* board)
{
    gamestate->ruleset = ruleset;
    gamestate->board = board;
    gamestate->onlineState = BE_OFFLINE;
}

/** \brief Exports the gamestate to JSON format, outputted to `exportedGameState`. Does not export the associated cScene or any keystates.
 *
 * \param gamestate beGameState*
 * \return char* - the address of `exportedGameState`
 */
char* beExportGameState(beGameState* gamestate)
{
    //TODO
    return exportedGameState;
}

/** \brief Destroys a gamestate, cleaning up ALL of its memory.
 *
 * \param gamestate beGameState*
 */
void beDestroyGameState(beGameState* gamestate)
{
    beDestroyBoard(gamestate->board);
    beDestroyRuleset(gamestate->ruleset);
}
