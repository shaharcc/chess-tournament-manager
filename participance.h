#ifndef _PARTICIPANCE_H
#define _PARTICIPANCE_H

#include <stdio.h>

typedef struct participance_t *Participance;


/**
 * participanceCreate: allocates a new participance for a certain player.
 * 
 * @param particpances - the map to which the particpance is added.
 * @param tour_id - the id that the future participance will obtain.
 *
 * @return NULL if the allocation failed, or a pointer to the new participance that was created otherwise.
 *
 */
Participance participanceCreate(Map participances, int tour_id);

/**
 * participanceCopy: duplicate a given participance - allocate a new one and copy the data.
 *
 * @param participance_to_copy - the participance that is copied. Must be non-NULL.
 * 
 * @return NULL if the allocation of the new participance failed, 
 *         or a pointer to the new participance that was created otherwise. 
 * 
 */
MapDataElement participanceCopy(MapDataElement participance_to_copy);

/** 
 * participanceDestroy: frees all resources of a participance of a certain player in a certain tournament.
 * 
 * @param participance - the participance that is going to be deleted.
 * 
 */
void participanceDestroy(MapDataElement generic_participance);

/**
 * participanceGetId: gives the id of a certain participance for a player in a certain tournament.
 * 
 * @param participance - the participance of which we would get the id of.
 * 
 * @return the id of the participance.
 *
 */
int* participanceGetId(Participance participance);

/**
 * participanceGetWins: gives the number of games a certain player won in a certain tourament.
 * 
 * @param participance - the participance of the player of which we would get the number of wins.
 * 
 * @return the number of games the player won at the tournament.
 *
 */
int participanceGetWins(Participance participance);

/**
 * participanceGetLosses: gives the number of games a certain player lost in a certain tourament.
 * 
 * @param participance - the participance of the player of which we would get the number of losses.
 * 
 * @return the number of games the player lost at the tournament.
 *
 */
int participanceGetLosses(Participance participance);

/**
 * participanceGetDraws: gives the number of games a certain player had a draw at, in a certain tourament.
 * 
 * @param participance - the participance of the player of which we would get the number of draws.
 * 
 * @return the number of games the player had a draw at the tournament.
 *
 */
int participanceGetDraws(Participance participance);

/**
 * participanceGetNumOfGames: gives the number of games a certain player played in a certain tourament.
 * 
 * @param participance - the participance of the player of which we would get the number of games in a certain tuornament.
 * 
 * @return the number of games the player played at the tournament.
 *
 */
int participanceGetNumOfGames(Participance participance);

/**
 * participanceRaiseNumOfGames: raises the number of games a certain player played in a certain tourament by one.
 * 
 * @param participance - the participance of which the number of games would be raised.
 *
 */
void participanceRaiseNumOfGames(Map participances, int tournament_id);

/**
 * participanceWinnerUpdate: raises the number of wins for the winner and the number of losses
 *                           for the loser, at their participance in a certain game- that belongs
 *                           to a certain tournament.
 * 
 * @param winner_participance - the map of participances of the player who won- in which the number of wins will
 *                              be raised, respectively to the tournament id.
 * @param winner_participance - the map of participances of the player who lost- in which the number of losses will
 *                              be raised, respectively to the tournament id.
 * @param tour_id - the id of the participance of which the number of wins and losses are going to be raised.
 * 
 */
void participanceWinnerUpdate(Map winner_participances, Map loser_participances, int tour_id);

/**
 * participanceDrawUpdate: raises the number of draws for both players at their participance in a certain game-
 *                           that belongs to a certain tournament.
 * 
 * @param player1_participances - the map of participances of the first player in the game where the score was a draw.
 * @param player2_participances - the map of participances of the second player in the game where the score was a draw.
 * @param tour_id - the id of the participance of which the number draws is going to be raised.
 * 
 */
void participanceDrawUpdate(Map player1_participances, Map player2_participances, int tour_id);


#endif //_PARTICIPANCE_H
