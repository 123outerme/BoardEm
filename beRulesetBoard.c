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
 /*
void beInitCell(beCell* cell, cDoublePt* pts, int ptsSize, cDoublePt* center, SDL_Color outlineColor)
{
    // *
    //printf("-- %d\n", ptsSize);
    cell->points = calloc(ptsSize, sizeof(cDoublePt));
    memcpy(cell->points, pts, ptsSize * sizeof(cDoublePt));
    // * /
    cell->ptsSize = ptsSize;

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
//*/

/** \brief
 *
 * \param beBoard* board
 * \param cResource* boardRes
 * \param int w
 * \param int h
 * \param void (*applyPlayerMovement)(beBoard*, bePlayer, int))
 */
void beInitBoard(beBoard* board, bePlayer* players, int numPlayers, cDoublePt** cells, int cellsSize, char* bgImgPath, int w, int h, bool (*checkMovement)(bePiece, int))
{
    board->players = calloc(numPlayers, sizeof(bePlayer));
    memcpy(board->players, players, numPlayers * sizeof(bePlayer));
    board->numPlayers = numPlayers;

    initCResource(&(board->boardResource), (void*) board, beDrawBoardCoSprite, beDestroyBoardCoSprite, 5);

    //printf("--%d\n", cellsSize);

    board->cells = calloc(cellsSize, sizeof(cDoublePt*));
    for(int i = 0; i < cellsSize; i++)
    {
        board->cells[i] = cells[i];
        //printf("-%d\n", i);
    }
    //memcpy(board->cells, cells, cellsSize * sizeof(beCell));
    board->cellsSize = cellsSize;

    SDL_Texture* bgTexture;
    loadIMG(bgImgPath, &bgTexture);

    int srcW, srcH;
    SDL_QueryTexture(bgTexture, NULL, NULL, &srcW, &srcH);

    initCSprite(&(board->bgImage), bgTexture, bgImgPath, 0, (cDoubleRect) {0, 0, w, h}, (cDoubleRect) {0, 0, srcW, srcH}, NULL, 1.0, SDL_FLIP_NONE, 0.0, false, NULL, 5);

    board->width = w;
    board->height = h;
    board->checkMovement = checkMovement;
}

/** \brief Initializes the game board from a file
 *
 * \param gamestate beGameState*
 * \param players bePlayer*
 */
void beConstructGameBoard(beBoard* board, bePlayer* players, int playerCount, char* folderName)
{
    char* pointsFile = calloc(MAX_PATH, sizeof(char));
    snprintf(pointsFile, MAX_PATH, "assets/%s/cells.dat", folderName);  //gives us a string that takes us to our cells

    char* infoFile = calloc(MAX_PATH, sizeof(char));
    snprintf(infoFile, MAX_PATH, "assets/%s/info.dat", folderName);  //gives us a string that takes us to our data

    //read info data
    board->name = calloc(20, sizeof(char));
    readLine(infoFile, 0, 20, &(board->name));
    //printf("%s\n", board->name);

    char* infoData = calloc(50, sizeof(char));
    readLine(infoFile, 1, 50, &infoData);
    int r = strtol(strtok(infoData, "#, "), NULL, 16);
    board->bgColor = (SDL_Color) {r, strtol(strtok(NULL, ", "), NULL, 16), strtol(strtok(NULL, ", "), NULL, 16), 0xFF};
    //printf("{%x, %x, %x, 0xFF}\n", board->bgColor.r, board->bgColor.g, board->bgColor.b);

    //* new version (everything stored in board)
    int cellCount = checkFile(pointsFile);
    cDoublePt* cells[cellCount];
    //printf("%d\n", cellCount);

    board->ptsSize = calloc(cellCount, sizeof(int));
    board->outlines = calloc(cellCount, sizeof(SDL_Color));
    board->centers = calloc(cellCount, sizeof(cDoublePt));
    board->names = calloc(cellCount, sizeof(char*));
    board->radii = calloc(cellCount, sizeof(double));

    for(int i = 0; i < cellCount; i++)
    {
        char* lineData = calloc(2048, sizeof(char));
        readLine(pointsFile, i, 2048, &lineData);
        //printf("%s\n", lineData);

        int pointCount = strtol(strtok(lineData, "[]"), NULL, 10);
        board->ptsSize[i] = pointCount;
        //board->outlines[i] = (SDL_Color) {0xFF, 0x00, 0x00, 0xFF};
        //printf("%d\n", pointCount);
        cells[i] = calloc(pointCount, sizeof(cDoublePt));

        for(int j = 0; j < pointCount; j++)
        {
            cells[i][j].x = strtod(strtok(NULL, "{,} "), NULL);
            cells[i][j].y = strtod(strtok(NULL, "{,} "), NULL);
            //printf("%f, %f\n", points[j].x, points[j].y);
        }
        board->centers[i].x = strtod(strtok(NULL, "(,) "), NULL);
        board->centers[i].y = strtod(strtok(NULL, "(,) "), NULL);
        board->radii[i] = strtod(strtok(NULL, "(,) "), NULL);
        char* name = strtok(NULL, "\"-");
        board->names[i] = calloc(strlen(name), sizeof(char));
        strcpy(board->names[i], name);
        board->outlines[i] = (SDL_Color) {strtol(strtok(NULL, "#, "), NULL, 16), strtol(strtok(NULL, ", "), NULL, 16), strtol(strtok(NULL, ", "), NULL, 16), strtol(strtok(NULL, "#, "), NULL, 16)};
        //printf("%x, %x, %x, %x\n", board->outlines[i].r, board->outlines[i].g, board->outlines[i].b, board->outlines[i].a);

        //ERROR: SEGFAULT on this line (sometimes)
        free(lineData);
        //init center point (potentially)

        //init neighbors list (line num)
    }
    free(pointsFile);
    //*/

    /* old version (beCells)
    int cellCount = checkFile(pointsFile);
    beCell cells[cellCount];
    //printf("%d\n", cellCount);

    for(int i = 0; i < cellCount; i++)
    {
        char* lineData = calloc(2048, sizeof(char));
        readLine(pointsFile, i, 2048, &lineData);
        //printf("%s\n", lineData);

        int pointCount = strtol(strtok(lineData, "[]"), NULL, 10);
        //printf("%d\n", pointCount);
        cDoublePt* points = calloc(pointCount, sizeof(cDoublePt));

        for(int j = 0; j < pointCount; j++)
        {
            points[j].x = strtod(strtok(NULL, "{,} "), NULL);
            points[j].y = strtod(strtok(NULL, "{,} "), NULL);
            //printf("%f, %f\n", points[j].x, points[j].y);
        }
        free(lineData);
        //init center point (potentially)

        //init neighbors list (line num)

        beInitCell(&(cells[i]), points, pointCount, NULL, (SDL_Color) {0xFF, 0x00, 0x00, 0xFF});
        free(points);
    }
    free(pointsFile);
    //*/

    /* direct allocation
    int cellCount = 4;
    beCell cells[cellCount];
    {
        cDoublePt pts[4] = {(cDoublePt) {7.0875, 8.333333}, (cDoublePt) {6.1125, 10.291667}, (cDoublePt) {4.275, 9.958333}, (cDoublePt) {4.3875, 8.291667}};
        cDoublePt pts2[8] = {(cDoublePt) {7.0875, 8.333333}, (cDoublePt) {8.362500, 8.708333}, (cDoublePt) {8.4375, 9.125}, (cDoublePt) {9.825, 8.5}, (cDoublePt) {10.05, 8.833333}, (cDoublePt) {8.737500, 10.875000}, (cDoublePt) {6.937500, 10.833333}, (cDoublePt) {6.1125, 10.291667}};
        cDoublePt pts3[7] = {(cDoublePt) {2.6625, 6.958333}, (cDoublePt) {2.6625, 5.416667}, (cDoublePt) {1.1625, 5.041667}, (cDoublePt) {0.1875, 5.541667}, (cDoublePt) {0.225000, 6.833333}, (cDoublePt) {1.162500, 7.291667}, (cDoublePt) {1.612500, 6.708333}};
        cDoublePt pts4[5] = {(cDoublePt) {2.6625, 5.416667}, (cDoublePt) {7.3125, 5.125}, (cDoublePt) {8.625, 6.25}, (cDoublePt) {7.275, 6.875000}, (cDoublePt) {2.7, 6.416667}};

        beInitCell(&(cells[0]), (cDoublePt*) pts, 4, NULL, (SDL_Color) {0xFF, 0x00, 0x00, 0xFF});
        beInitCell(&(cells[1]), (cDoublePt*) pts2, 8, NULL, (SDL_Color) {0xFF, 0x00, 0x00, 0xFF});
        beInitCell(&(cells[2]), (cDoublePt*) pts3, 7, NULL, (SDL_Color) {0xFF, 0x00, 0x00, 0xFF});
        beInitCell(&(cells[3]), (cDoublePt*) pts4, 5, NULL, (SDL_Color) {0xFF, 0x00, 0x00, 0xFF});
    }
    //*/

    //initialize board (needs players + cells)

    char* mapImgPath = calloc(MAX_PATH, sizeof(char));
    strcpy(mapImgPath, "assets/");
    strcat(mapImgPath, folderName);
    strcat(mapImgPath, "/map.png");

    beInitBoard(board, players, playerCount, cells, cellCount, mapImgPath, 20 * 48, 48 * 10, NULL);
    free(mapImgPath);
    //free(cells);
}

/** \brief
 *
 * \param beRuleset* ruleset
 * \param void (*playerTurn)(beBoard*)
 * \param void (*updateScores)(beBoard*)
 * \param int (*checkWin)(beBoard*)
 * \param void (*applyMoneyGameBonus)(beBoard*)
 */
void beInitRuleset(beRuleset* ruleset, void* subclass,
                    int (*playerTurnFrame)(beBoard*, bePlayer*, cInputState),
                    void (*updateScores)(beBoard*),
                    int (*checkWin)(beBoard*),
                    void (*applyMoneyGameBonus)(beBoard*),
                    void (*freeSubclass)(void*))
{
    ruleset->subclass = subclass;
    ruleset->playerTurnFrame = playerTurnFrame;
    ruleset->updateScores = updateScores;
    ruleset->checkWin = checkWin;
    ruleset->applyMoneyGameBonus = applyMoneyGameBonus;
    ruleset->freeSubclass = freeSubclass;
}

/** \brief Checks a click against all cells to determine if a cell was clicked, and which
 * Expects the click in the camera-relative coordinate system
 * \param board beBoard*
 * \param camera cCamera
 * \param click cDoublePt
 * \return int - index of cell clicked, or -1 otherwise
 */
int beCheckMapClick(beBoard* board, cCamera camera, cDoublePt click)
{
    int indexesCollided[board->cellsSize];
    int maxCollided = 0;
    for(int i = 0; i < board->cellsSize; i++)  //get a sub-list of cells where the click COULD have landed
    {
        if (getDistance(click.x, click.y, board->centers[i].x, board->centers[i].y) < board->radii[i])
            indexesCollided[maxCollided++] = i;
    }

    int collidedIndex = -1;

    //printf("Found:\n");
    for(int i = 0; i < maxCollided; i++)  //check each cell previously found with the more accurate but slower check
    {
        //printf("%s\n", gamestate->board->names[indexesCollided[i]]);
        if (beCheckCellClick(board, camera, indexesCollided[i], click))
        {
            collidedIndex = indexesCollided[i];
            break;
        }
    }
    return collidedIndex;
}

/** \brief Given a click and a cell index, checks if a cell has been clicked on.
 * Expects the click in the camera-relative coordinate system
 * \param board beBoard*
 * \param camera cCamera
 * \param cellIndex int
 * \param click cDoublePt
 * \return bool - true if collided, false otherwise
 */
bool beCheckCellClick(beBoard* board, cCamera camera, int cellIndex, cDoublePt click)
{ //uses the ray-casting algorithm (found here: https://en.wikipedia.org/wiki/Point_in_polygon)
    int collisions = 0;
    click = cameraCoordToWindowCoord(click, camera);

    bool lineTangible[board->ptsSize[cellIndex]];
    for(int i = 0; i < board->ptsSize[cellIndex]; i++)
        lineTangible[i] = true;

    for(int rayY = 0; rayY < click.y; rayY++) //cast a ray heading in the +y direction
    {
        bool collided = false;
        for(int i = 0; i < board->ptsSize[cellIndex]; i++)
        {
            if (lineTangible[i])
            {
                SDL_Rect collisionRay = (SDL_Rect) {.x = click.x, .y = rayY, .w = 1, .h = 2};
                cDoublePt copy1 = (cDoublePt) {board->cells[cellIndex][i].x, board->cells[cellIndex][i].y},
                          copy2 = (cDoublePt) {board->cells[cellIndex][(i + 1) % board->ptsSize[cellIndex]].x, board->cells[cellIndex][(i + 1) % board->ptsSize[cellIndex]].y};
                          //intersect function modifies all data, so copies of the data have to be created

                copy1 = cameraCoordToWindowCoord(copy1, camera);
                copy2 = cameraCoordToWindowCoord(copy2, camera);
                int x1 = (int) copy1.x, x2 = (int) copy2.x, y1 = (int) copy1.y, y2 = (int) copy2.y;

                /* debug draw
                SDL_SetRenderDrawColor(global.mainRenderer, 0, 0, 0, 0xFF);
                SDL_RenderDrawRect(global.mainRenderer, &collisionRay);
                SDL_RenderDrawLine(global.mainRenderer, x1, y1, x2, y2);
                SDL_RenderPresent(global.mainRenderer);
                //*/

                bool lineCollision = SDL_IntersectRectAndLine(&collisionRay, &x1, &y1, &x2, &y2);
                if (lineCollision)
                    lineTangible[i] = false;

                collided = collided | lineCollision;
            }
        }
        if (collided)
            collisions++;
    }

    //printf("%d\n", collisions);
    return (collisions % 2);
}

cDoublePt windowCoordToCameraCoord(cDoublePt pt, cCamera camera)
{
    //add back the offsets
    pt.x += (camera.rect.x * global.windowW / camera.rect.w);
    pt.y += (camera.rect.y * global.windowH / camera.rect.h);

    //rotate the point back around
    pt = rotatePoint(pt, (cDoublePt) {global.windowW / 2, global.windowH / 2}, -1.0 * camera.degrees);

    //un-zoom the points relative to the center of the window
    pt.x = (pt.x - global.windowW / 2.0) / camera.zoom + global.windowW / 2.0;
    pt.y = (pt.y - global.windowH / 2.0) / camera.zoom + global.windowH / 2.0;

    //convert from px to camera scaling units
    pt.x *= camera.rect.w / global.windowW;
    pt.y *= camera.rect.h / global.windowH;

    return pt;
}

cDoublePt cameraCoordToWindowCoord(cDoublePt pt, cCamera camera)
{
    //convert from camera scaling units to px
    pt.x /= camera.rect.w / global.windowW;
    pt.y /= camera.rect.h / global.windowH;

    //zoom the points relative to the center of the window
    pt.x = (pt.x - global.windowW / 2.0) * camera.zoom + global.windowW / 2.0;
    pt.y = (pt.y - global.windowH / 2.0) * camera.zoom + global.windowH / 2.0;

    //rotate the point around
    pt = rotatePoint(pt, (cDoublePt) {global.windowW / 2, global.windowH / 2}, camera.degrees);

    //subtract out the offsets
    pt.x -= (camera.rect.x * global.windowW / camera.rect.w);
    pt.y -= (camera.rect.y * global.windowH / camera.rect.h);

    return pt;
}

/** \brief Destroys an allocated beCell.
 *
 * \param cell beCell*
 */
/*
void beDestroyCell(beCell* cell, int ptsSize)
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

    //misc cleanup
    cell->center = (cDoublePt) {0,0};
    cell->outlineColor = (SDL_Color) {0,0,0,0};

}
//*/

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

    //cleanup cells and relevant data
    if (board->cellsSize > 0)
    {
        for(int i = 0; i < board->cellsSize; i++)
        {
            free(board->cells[i]);
            free(board->names[i]);
        }
        free(board->cells);
        free(board->outlines);
        free(board->centers);
        free(board->names);
        board->cells = NULL;
    }
    board->cellsSize = 0;

    //cleanup background
    destroyCSprite(&(board->bgImage));

    //cleanup misc
    board->width = 0;
    board->height = 0;
    board->checkMovement = NULL;
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

    //subclass stuff
    if (ruleset->freeSubclass)
        ruleset->freeSubclass(ruleset->subclass);

    ruleset->freeSubclass = NULL;
    ruleset->subclass = NULL;
}


/** \brief Helper function for board drawing (CoSprite use only)
 *
 * \param cell beCell
 * \param camera cCamera
 */
void beDrawCellCoSprite(cDoublePt* cell, int ptsSize, SDL_Color color, cCamera camera)
{
    //set the per-cell draw color
    SDL_SetRenderDrawColor(global.mainRenderer, color.r, color.g, color.b, color.a);

    cDoublePt cellPoints[ptsSize];

    //printf("testing draw cell - %d\n", board->cells[i].ptsSize);

    for(int i = 0; i < ptsSize; i++)
        {
            cellPoints[i] = (cDoublePt) {cell[i].x * global.windowW / camera.rect.w, cell[i].y * global.windowH / camera.rect.h};

            //scale the points relative to the center of the window
            cellPoints[i].x = (cellPoints[i].x - global.windowW / 2.0) * camera.zoom + global.windowW / 2.0;
            cellPoints[i].y = (cellPoints[i].y - global.windowH / 2.0) * camera.zoom + global.windowH / 2.0;

            //if board isn't fixed do these three lines. The board is never set to be fixed though
            cellPoints[i] = rotatePoint(cellPoints[i], (cDoublePt) {global.windowW / 2, global.windowH / 2}, camera.degrees);
            cellPoints[i].x -= (camera.rect.x * global.windowW / camera.rect.w);
            cellPoints[i].y -= (camera.rect.y * global.windowH / camera.rect.h);
        }
        for(int i = 0; i < ptsSize; i++)
        {
            //draw a line between the ith point and the ((i + 1) % ptsSize)th point
            SDL_RenderDrawLine(global.mainRenderer, (int) cellPoints[i].x, (int) cellPoints[i].y,
                               (int) cellPoints[(i + 1) % ptsSize].x, (int) cellPoints[(i + 1) % ptsSize].y);

            /* temp center point drawing (beCell)
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
    {
        beDrawCellCoSprite(board->cells[i], board->ptsSize[i], board->outlines[i], camera);
        /* temp center point drawing
            SDL_SetRenderDrawColor(global.mainRenderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderDrawPoint(global.mainRenderer, board->centers[i].x * global.windowW / camera.rect.w, board->centers[i].y * global.windowH / camera.rect.h);
        //*/
        /* temp radius drawing
        SDL_SetRenderDrawColor(global.mainRenderer, 0x00, 0x00, 0x00, 0xFF);
        int deltaDeg = 10;
        for(int d = 0; d <= 360; d += deltaDeg)
            SDL_RenderDrawLine(global.mainRenderer, (board->centers[i].x + (board->radii[i] * cos(degToRad(d - deltaDeg)))) * global.windowW / camera.rect.w, (board->centers[i].y + (board->radii[i] * sin(degToRad(d - deltaDeg)))) * global.windowH / camera.rect.h, (board->centers[i].x + (board->radii[i] * cos(degToRad(d)))) * global.windowW / camera.rect.w, (board->centers[i].y + (board->radii[i] * sin(degToRad(d)))) * global.windowH / camera.rect.h);
        //*/
    }

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

    //cleanup cells and relevant data
    if (board->cellsSize > 0)
    {
        for(int i = 0; i < board->cellsSize; i++)
        {
            free(board->cells[i]);
            free(board->names[i]);
        }
        free(board->cells);
        free(board->outlines);
        free(board->centers);
        free(board->names);
        board->cells = NULL;
    }
    board->cellsSize = 0;

    //cleanup background
    destroyCSprite(&(board->bgImage));

    //cleanup misc
    board->bgColor = (SDL_Color) {0x00, 0x00, 0x00, 0x00};
    board->width = 0;
    board->height = 0;
    board->checkMovement = NULL;
}
