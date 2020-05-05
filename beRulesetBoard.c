#include "beRulesetBoard.h"

/** \brief
 *
 * \param cell beCell*
 * \param pts cDoublePt*
 * \param ptsSize int
 * \param neighbors beCell*
 * \param center cDoublePt* - Pass NULL for automatic center (width / 2, height / 2)
 * \param outline SDL_Color
 */
void beInitCell(beCell* cell, cDoublePt* pts, int ptsSize, beCell* neighbors, int neighborsSize, cDoublePt* center, SDL_Color outlineColor)
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

    if (center == NULL)
    {
        //iterate through each point, find the min and max x/y. This will find the width and height, then divide by 2 to yield center
        double minX = pts[0].x, maxX = pts[0].x, minY = pts[0].y, maxY = pts[0].y;
        for(int i = 1; i < ptsSize; i++)
        {
            if (pts[i].x > maxX)
                maxX = pts[i].x;

            if (pts[i].x < minX)
                minX = pts[i].x;

            if (pts[i].y > maxY)
                maxY = pts[i].y;

            if (pts[i].y < minY)
                minY = pts[i].y;
        }
        cell->center = (cDoublePt) {(maxX + minX) / 2.0, (maxY + minY) / 2.0};
        //printf("%f, %f\n", cell->center.x, cell->center.y);
    }
    else
        cell->center = *center;

    cell->outlineColor = outlineColor;
}

/** \brief
 *
 * \param beBoard* board
 * \param cResource* boardRes
 * \param int w
 * \param int h
 * \param void (*applyPlayerMovement)(beBoard*, bePlayer, int))
 */
void beInitBoard(beBoard* board, bePlayer* players, int numPlayers, beCell* cells, int cellsSize, char* bgImgPath, int w, int h, void (*applyPlayerMovement)(beBoard*, bePlayer*))
{
    board->players = calloc(numPlayers, sizeof(bePlayer));
    memcpy(board->players, players, numPlayers * sizeof(bePlayer));
    board->numPlayers = numPlayers;

    initCResource(&(board->boardResource), (void*) board, beDrawBoardCoSprite, beDestroyBoardCoSprite, 5);

    board->cells = calloc(cellsSize, sizeof(cDoublePt));
    memcpy(board->cells, cells, cellsSize * sizeof(beCell));
    board->cellsSize = cellsSize;

    SDL_Texture* bgTexture;
    loadIMG(bgImgPath, &bgTexture);
    initCSprite(&(board->bgImage), bgTexture, bgImgPath, 0, (cDoubleRect) {0, 0, w, h}, (cDoubleRect) {0, 0, w, h}, NULL, 1.0, SDL_FLIP_NONE, 0.0, false, NULL, 5);

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
                    int (*playerTurnFrame)(beBoard*, bePlayer*, cInputState),
                    void (*updateScores)(beBoard*),
                    int (*checkWin)(beBoard*),
                    void (*applyMoneyGameBonus)(beBoard*))
{
    ruleset->playerTurnFrame = playerTurnFrame;
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
    cell->outlineColor = (SDL_Color) {0, 0, 0, 0};
    cell->center = (cDoublePt) {0, 0};
    cell->housedPlayer = NULL;
}

/** \brief Destroys an allocated beBoard.
 *
 * \param board beBoard*
 */
void beDestroyBoard(beBoard* board)
{
    //cleanup players
    if (board->numPlayers > 0)
    {
        for(int i = 0; i < board->numPlayers; i++)
        {
            beDestroyPlayer(&(board->players[i]));
        }
        free(board->players);
        board->players = NULL;
    }
    board->numPlayers = 0;

    //cleanup CResource
    destroyCResource(&(board->boardResource));

    //cleanup cells
    if (board->cellsSize > 0)
    {
        for(int i = 0; i < board->cellsSize; i++)
        {
            beDestroyCell(&(board->cells[i]));
        }
        free(board->cells);
        board->cells = NULL;
    }
    board->cellsSize = 0;

    //cleanup background
    destroyCSprite(&(board->bgImage));

    //cleanup misc
    board->width = 0;
    board->height = 0;
    board->applyPlayerMovement = NULL;
}

/** \brief Destroys an allocated beRuleset
 *
 * \param ruleset beRuleset*
 */
void beDestroyRuleset(beRuleset* ruleset)
{
    ruleset->playerTurnFrame = NULL;
    ruleset->updateScores = NULL;
    ruleset->checkWin = NULL;
    ruleset->applyMoneyGameBonus = NULL;
}


/** \brief Helper function for board drawing (CoSprite use only)
 *
 * \param cell beCell
 * \param camera cCamera
 */
void beDrawCellCoSprite(beCell cell, cCamera camera)
{
    //set the per-cell draw color
    SDL_SetRenderDrawColor(global.mainRenderer, cell.outlineColor.r, cell.outlineColor.g, cell.outlineColor.b, cell.outlineColor.a);

    cDoublePt cellPoints[cell.ptsSize];

    //printf("testing draw cell - %d\n", board->cells[i].ptsSize);

    for(int i = 0; i < cell.ptsSize; i++)
        {
            //transform initial points to where they would visually be, by the camera's rotation/position/zoom/scaling
            double x = cell.points[i].x, y = cell.points[i].y;
            cellPoints[i] = (cDoublePt) {x * camera.zoom * global.windowW / camera.rect.w, y * camera.zoom * global.windowH / camera.rect.h};

            //if board isn't fixed do these three lines. The board is never set to be fixed though
            cellPoints[i] = rotatePoint(cellPoints[i], (cDoublePt) {global.windowW / 2, global.windowH / 2}, camera.degrees);
            cellPoints[i].x -= (camera.rect.x * global.windowW / camera.rect.w);
            cellPoints[i].y -= (camera.rect.y * global.windowH / camera.rect.h);
        }
        for(int i = 0; i < cell.ptsSize; i++)
        {
            //draw a line between the ith point and the ((i + 1) % ptsSize)th point
            SDL_RenderDrawLine(global.mainRenderer, (int) cellPoints[i].x, (int) cellPoints[i].y,
                               (int) cellPoints[(i + 1) % cell.ptsSize].x, (int) cellPoints[(i + 1) % cell.ptsSize].y);

            /* temp center point drawing
            SDL_SetRenderDrawColor(global.mainRenderer, 0x00, 0xFF, 0x00, 0xFF);
            SDL_RenderDrawPoint(global.mainRenderer, cell.center.x * global.windowW / camera.rect.w, cell.center.y * global.windowH / camera.rect.h);
            SDL_SetRenderDrawColor(global.mainRenderer, cell.outlineColor.r, cell.outlineColor.g, cell.outlineColor.b, cell.outlineColor.a);
            //*/
        }
}

//Function pointer "targets"
/** \brief CoSprite funct ptr use only. Draws the beBoard.
 *
 * \param ptrBoard void*
 * \param camera cCamera
 */
void beDrawBoardCoSprite(void* ptrBoard, cCamera camera)
{
    beBoard* board = (beBoard*) ptrBoard;

    //printf("testing draw board - %d\n", board->cellsSize);

    drawCSprite(board->bgImage, camera, false, false);

    //store previous draw color
    Uint8 prevR = 0x00, prevG = 0x00, prevB = 0x00, prevA = 0xFF;
    SDL_GetRenderDrawColor(global.mainRenderer, &prevR, &prevG, &prevB, &prevA);

    //draw cells
    for(int i = 0; i < board->cellsSize; i++)
        beDrawCellCoSprite(board->cells[i], camera);

    //reset draw color
    SDL_SetRenderDrawColor(global.mainRenderer, prevR, prevG, prevB, prevA);
}

/** \brief CoSprite funct ptr use only. Technically not necessary because BE already handles cleanup but just in case
 *
 * \param ptrBoard void*
 */
void beDestroyBoardCoSprite(void* ptrBoard)
{
    beBoard* board = (beBoard*) ptrBoard;
    //cleanup players
    if (board->numPlayers > 0)
    {
        for(int i = 0; i < board->numPlayers; i++)
        {
            beDestroyPlayer(&(board->players[i]));
        }
        free(board->players);
        board->players = NULL;
    }
    board->numPlayers = 0;

    //cleanup cells
    if (board->cellsSize > 0)
    {
        for(int i = 0; i < board->cellsSize; i++)
        {
            beDestroyCell(&(board->cells[i]));
        }
        free(board->cells);
        board->cells = NULL;
    }
    board->cellsSize = 0;

    //cleanup background
    destroyCSprite(&(board->bgImage));

    //cleanup misc
    board->width = 0;
    board->height = 0;
    board->applyPlayerMovement = NULL;
}
