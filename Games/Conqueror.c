#include "Conqueror.h"

/** \brief Checks if a movement on the Conqueror board is valid
 *
 * \param piece bePiece
 * \param cellId int
 * \return bool - true if movement is valid and therefore applied, false if not and therefore not applied
 *
 */
bool conquerorCheckMovement(bePiece piece, int cellId)
{
    bool isValid = false;
    int validMoves[42][6] = {
                                 {1, 4, 5, 7, -1, -1}, //W. US: E. US, Alberta, Ontario, C. America
                                 {0, 6, 5, 7, -1, -1}, //E. US: W. US, E. Canada, Ontario, C. America
                                 {3, 4, 32, -1 , -1, -1}, //Alaska: N.W. Territory, Alberta, Kamchatka
                                 {2, 4, 5, -1, -1, -1}, //N.W. Territory: Alaska, Alberta, Ontario
                                 {0, 2, 3, 5, -1, -1}, //Alberta: W. US, Alaska, N.W. Territory, Ontario
                                 {0, 1, 3, 4, 6, -1}, //Ontario: W. US, E. US, N.W. Territory, Alberta, E. Canada
                                 {1, 5, 12, -1, -1, -1}, //E. Canada: E. US, Ontario, Greenland
                                 {0, 1, 8, -1, -1, -1}, //C. America: W. US, E. US, Venezuela
                                 {7, 9, 10, -1, -1, -1}, //Venezuela: C. America, Peru, Brazil
                                 {8, 10, 11, -1, -1, -1}, //Peru: Venezuela, Brazil, Argentina
                                 {8, 9, 11, 19, -1, -1}, //Brazil: Venezuela, Peru, Argentina, N. Africa
                                 {9, 10, -1, -1, -1, -1}, //Argentina: Peru, Brazil
                                 {6, 13, -1, -1, -1, -1}, //Greenland: E. Canada, Iceland
                                 {12, 14, 18, -1, -1, -1}, //Iceland: Greenland, UK, Scandinavia
                                 {13, 15, 16, 18, -1, -1}, //UK: Iceland, W. Europe, N. Europe, Scandinavia
                                 {13, 14, 16, 17, 19, -1}, //W. Europe: Iceland, UK, N. Europe, S. Europe, N. Africa
                                 {14, 15, 17, 18, 26, -1}, //N. Europe: UK, W. Europe, S. Europe, Scandinavia, Russia
                                 {15, 16, 19, 20, 25, 26}, //S. Europe: W. Europe, N. Europe,  N. Africa, Egypt, Middle East, Russia
                                 {13, 14, 16, 26, -1, -1}, //Scandinavia: Iceland, UK, N. Europe, Russia
                                 {10, 15, 17, 20, 22, -1}, //N. Africa: Brazil, W. Europe, S. Europe, Egypt, C. Africa
                                 {17, 19, 21, 22, 25, -1}, //Egypt: S. Europe, N. Africa, E. Africa, C. Africa, Middle East
                                 {20, 22, 23, 24, 25, -1}, //E. Africa: Egypt, C. Africa, S. Africa, Madagascar, Middle East
                                 {19, 20, 21, 23, -1, -1}, //C. Africa: N. Africa, Egypt, E. Africa, S. Africa
                                 {19, 21, 22, 24, -1, -1}, //S. Africa: N. Africa, E. Africa, C. Africa, Madagascar
                                 {21, 23, -1, -1, -1, -1}, //Madagascar: E. Africa, S. Africa
                                 {17, 20, 21, 26, 27, 35}, //Middle East: S. Europe, Egypt, E. Africa, Russia, Afghanistan, India
                                 {16, 17, 18, 25, 27, 28}, //Russia: N. Europe, S. Europe, Scandinavia, Middle East, Afghanistan, Ural
                                 {25, 26, 28, 34, 35, -1}, //Afghanistan: Middle East, Russia, Ural, China, India
                                 {26, 27, 29, 33, 34, -1}, //Ural: Russia, Afghanistan, Siberia, Mongolia, China
                                 {28, 30, 31, 33, -1, -1}, //Siberia: Ural, Yakutsk, Irkutsk, Mongolia
                                 {29, 31, 32, -1, -1, -1}, //Yakutsk: Siberia, Irkutsk, Kamchatka
                                 {29, 30, 32, 33, -1, -1}, //Irkutsk: Siberia, Yakutsk, Kamchatka, Mongolia
                                 {2, 30, 31, 33, 37, -1}, //Kamchatka: Alaska, Yakutsk, Irkutsk, Mongolia, Japan
                                 {29, 31, 32, 34, 37, -1}, //Mongolia: Siberia, Irkutsk, Kamchatka, China, Japan
                                 {27, 28, 29, 33, 35, 36}, //China: Afghanistan, Ural, Siberia, Mongolia, India, S.E. Asia
                                 {25, 27, 34, 36, -1, -1}, //India: Middle East, Afghanistan, China, S.E. Asia
                                 {34, 35, 38, -1, -1, -1}, //S.E. Asia: China, India, Indonesia
                                 {32, 33, -1, -1, -1, -1}, //Japan: Kamchatka, Mongolia
                                 {36, 40, -1, -1, -1, -1}, //Indonesia: S.E. Asia, W. Australia
                                 {38, 40, 41, -1, -1, -1}, //New Guinea: Indonesia, W. Australia, E. Australia
                                 {38, 39, 41, -1, -1, -1}, //W. Australia: Indonesia, New Guinea, E. Australia
                                 {39, 40, -1, -1, -1, -1} //E. Australia: new Guinea, W. Australia
                            };

    for(int i = 0; i < 6; i++)  //check if the place we are coming from is in the list of acceptable squares for the place we are trying to go to
        isValid = isValid | (piece.locationIndex == validMoves[cellId][i]);

    return isValid;
}

/** \brief Applies movement for a Conqueror game (does battle on the board)
 *
 * \param board beBoard*
 * \param player bePlayer*
 * \param piece bePiece*
 * \param destCellIndex int
 */
void conquerorApplyMovement(beBoard* board, bePlayer* player, bePiece* piece, int destCellIndex)
{
    if (piece->num >= 0)  //if we have 2+ troops in our army
    {
        //do battle

        //if you win, then:
        int resultTroops = piece->num / 2;  //this will be the result troops you conquer the next territory with
        piece->num -= resultTroops;
        void* piecesRealloc = realloc((void*) player->pieces, (player->numPieces + 1) * sizeof(bePiece));
        if (piecesRealloc != NULL)
        {
            //free(player->pieces);  //idk why this has to not free in order to not be dumb
            player->pieces = piecesRealloc;
            beInitPiece(&(player->pieces[player->numPieces]), resultTroops, piece->sprite, destCellIndex);
            player->numPieces++;
        }
        else
        {
            printf("out of memory. Check logs\n");
            cLogEvent(boardEmLogger, "ERROR", "Out of memory", "Cannot realloc player pieces array");
        }
    }
}

/** \brief Updates scores for a Conqueror game
 *
 * \param board beBoard*
 */
void conquerorUpdateScores(beBoard* board)
{

    for(int i = 0; i < board->numPlayers; i++)
    {
        board->players[i].score = board->players[i].numPieces * 5;  //5 points for each territory
        for(int x = 0; x < board->players[i].numPieces; x++)
        {
            board->players[i].score += board->players[i].pieces[x].num;  //1 point for each troop in each territory
        }
    }
}

/** \brief Checks if a Conqueror game has been won
 *
 * \param board beBoard*
 * \return int 1-4 if that num player won, otherwise 0
 */
int conquerorCheckWin(beBoard* board)
{
    int winningPlayerIndex = 0;
    for(int i = 0; i < board->numPlayers; i++)
    {
        if (board->players[i].numPieces == board->cellsSize)
            winningPlayerIndex = i + 1;
    }
    return winningPlayerIndex;
}

/** \brief Sets up a game that has the Conqueror ruleset
 *
 * \param board beBoard* the board to be initialized
 */
void conquerorGameSetup(beBoard* board)
{
    //technically a 2-player game is supposed to have a 3rd neutral army but who cares

    for(int i = 0; i < board->numPlayers; i++)
    {
        board->players[i].subclass = malloc(sizeof(beConquerorArmy));
        initConquerorArmy((beConquerorArmy*) board->players[i].subclass, 50 - (5 * board->numPlayers));
        board->players[i].freeSubclass = &destroyConquerorArmy;
    }
    board->gamePhase = BE_PHASE_SETUP;
}

void conquerorApplyCorpBonus(bePlayer* player)
{ //TODO: figure out what this will do and why
    //whenever GO is conquered
    beConquerorArmy* army = (beConquerorArmy*) player->subclass;
    army->reinforcements += 5;  //add 5 more reinforcements (per turn?)
}

/** \brief Inits an army for a player
 *
 * \param army beConquerorArmy*
 * \param reinforcements int
 */
void initConquerorArmy(beConquerorArmy* army, int reinforcements)
{
    army->reinforcements = reinforcements;
}

/** \brief De-allocates an army for a player
 *
 * \param armyPtr void*
 */
void destroyConquerorArmy(void* armyPtr)
{
    beConquerorArmy* army = (beConquerorArmy*) armyPtr;
    army->reinforcements = 0;
}
