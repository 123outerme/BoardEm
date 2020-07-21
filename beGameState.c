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

/** \brief Exports the turn data to JSON format, outputted to `exportedTurnData`.
 *
 * \param gamestate beGameState*
 * \return char* - the address of `exportedGameState`
 */
char* beExportTurnData(beGameState* gamestate)
{
    //TODO
    return exportedTurnData;
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
