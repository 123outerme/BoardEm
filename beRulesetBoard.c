#include "beRulesetBoard.h"

/** \brief
 *
 * \param cell beCell*
 * \param pts cDoublePt*
 * \param ptsSize int
 * \param neighbors beCell*
 */
void beInitCell(beCell* cell, cDoublePt* pts, int ptsSize, beCell* neighbors, int neighborsSize)
{
    //*
    //printf("-- %d\n", ptsSize);
    cell->points = calloc(ptsSize, sizeof(cDoublePt));
    memcpy(cell->points, pts, ptsSize * sizeof(cDoublePt));
    //*/
    cell->ptsSize = ptsSize;

    cell->neighbors = calloc(neighborsSize, sizeof(beCell));
    memcpy(cell->neighbors, neighbors, neighborsSize * sizeof(beCell));
    cell->neighborsSize = neighborsSize;

    cell->housedPlayer = NULL;
}

/** \brief
 *
 * \param beBoard* board
 * \param cResource* boardRes
 * \param int w
 * \param int h
 * \param void (*applyPlayerMovement)(beBoard*, bePlayer, int))
 */
void beInitBoard(beBoard* board, bePlayer* players, int numPlayers, cResource* boardRes, beCell* cells, int cellsSize, int w, int h, void (*applyPlayerMovement)(beBoard*, bePlayer*))
{
    board->players = calloc(numPlayers, sizeof(bePlayer));
    memcpy(board->players, players, numPlayers * sizeof(bePlayer));

    board->numPlayers = numPlayers;
    board->boardResource = boardRes;
    board->cells = calloc(cellsSize, sizeof(cDoublePt));
    memcpy(board->cells, cells, cellsSize * sizeof(beCell));
    board->width = w;
    board->height = h;
    board->applyPlayerMovement = applyPlayerMovement;
}

/** \brief
 *
 * \param beRuleset* ruleset
 * \param void (*playerTurn)(beBoard*)
 * \param void (*updateScores)(beBoard*)
 * \param int (*checkWin)(beBoard*)
 * \param void (*applyMoneyGameBonus)(beBoard*)
 */
void beInitRuleset(beRuleset* ruleset,
                    void (*playerTurn)(beBoard*, bePlayer*),
                    void (*updateScores)(beBoard*),
                    int (*checkWin)(beBoard*),
                    void (*applyMoneyGameBonus)(beBoard*))
{
    ruleset->playerTurn = playerTurn;
    ruleset->updateScores = updateScores;
    ruleset->checkWin = checkWin;
    ruleset->applyMoneyGameBonus = applyMoneyGameBonus;
}

//

/** \brief Destroys an allocated beCell.
 *
 * \param cell beCell*
 */
void beDestroyCell(beCell* cell)
{
    //set all points to 0
    for(int i = 0; i < cell->ptsSize; i++)
    {
        cell->points[i].x = 0;
        cell->points[i].y = 0;
    }

    //cleanup points
    free(cell->points);
    cell->points = NULL;
    cell->ptsSize = 0;

    //cleanup neighbors
    free(cell->neighbors);
    cell->neighbors = NULL;
    cell->neighborsSize = 0;

    //cleanup misc
    cell->housedPlayer = NULL;
}

/** \brief Destroys an allocated beBoard.
 *
 * \param board beBoard*
 */
void beDestroyBoard(beBoard* board)
{
    //cleanup players
    for(int i = 0; i < board->numPlayers; i++)
    {
        beDestroyPlayer(&(board->players[i]));
    }
    free(board->players);
    board->players = NULL;
    board->numPlayers = 0;

    //cleanup CResource
    if (board->boardResource != NULL)
        destroyCResource(board->boardResource);

    board->boardResource = NULL;

    //cleanup cells
    for(int i = 0; i < board->cellsSize; i++)
    {
        beDestroyCell(&(board->cells[i]));
    }
    free(board->cells);
    board->cells = NULL;
    board->cellsSize = 0;

    //cleanup misc
    board->width = 0;
    board->height = 0;
    board->applyPlayerMovement = NULL;
}

void beDestroyRuleset(beRuleset* ruleset)
{
    ruleset->playerTurn = NULL;
    ruleset->updateScores = NULL;
    ruleset->checkWin = NULL;
    ruleset->applyMoneyGameBonus = NULL;
}
