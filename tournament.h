#ifndef TOURNAMENT_H_
#define TOURNAMENT_H_

#include <stdio.h>
#include <stdbool.h>


/** Type for representing one tournament */
typedef struct tournament_t *Tournament;


/**
 * tournamentCreate: allocates a new tournament.
 * 
 * @param tournaments - the map to which the tournament is added.
 * @param tournament_id - new tournament id. Must be positive, and unique.
 * @param max_games_per_player - the maximum number of games a player can take part in for this tournament. Must be at least 0.
 * @param tournament_location - the location in which the tournament takes place. Must be non-Null, start which capital letter 
 *                              and afterwards contain only small letters or spaces.
 * @param chess_result -  if there are no errors in the parameters its value is CHESS_SUCCESS. otherwise, its value is the specific error occured. 
 * 
 * @return
 * NULL if the allocation failed, or a pionter to the new tournament created otherwise.
 * 
 */
Tournament tournamentCreate(Map tournaments, int tournament_id, int max_games_per_player,
                            const char* tournament_location, ChessResult* chess_result);


/**
 * tournamentCopy: duplicate a given tournament - allocate a new one and copy all the data.
 *
 *  @param tournament - the tournament that is copied. Must be not NULL.
 * 
 * @return
 * NULL if the allocation of the new tournament failed, or a pionter to the new tournament created otherwise. 
 * 
 */
MapDataElement tournamentCopy(MapDataElement tournament_to_copy);

/** 
 * tournamentDestroy: Deletes an existing tournament and frees all resources.
 * 
 * @param tournaments - the map from which the tournament is deleted.
 * @param tournament_id - the tournament the is deleted.
 * 
 */
void tournamentDestroy(Tournament tournament);

/**
 * tournamentEnd: ending the tournament so that no more new games could be added to it.
 * 
 * @param tournaments - the map that contains the tournament that is ended.
 * @param tournament_id - the tournament to end.
 * 
 */
void tournamentEnd(Map tournaments, int tournament_id);

/**
 * tournamentCalculateWinnerId: calculate the winner with the highest rank of all players in the tournament's games, and update the tournament with this data. 
 *                              player rank = ( num_of_wins * 2 + num_of_draws * 1 ) / ( num_of_games_of_player )
 *                              If two players have the same rank, the player with least losses will be chosen.
 *                              If two players have the same number of losses, the player with the most wins will be chosen
 *                              If two players have the same number of wins and losses,
 *                              the player with smaller id will be chosen.
 * 
 * @param tournaments - a map of all the tournaments in the chess system.
 * @param players - a map of all the players in the chess system.
 * @param tournament_id - the tournament of which the winner is calculated.
 * 
 * @return
 * the id of the winner
 * 
 */
int tournamentCalculateWinnerId(Map tournaments, Map players, int tournament_id);

/**
 * IdCompare: compare two ids.
 * 
 * @param Id1 - first id.
 * @param Id2 - second id.
 * 
 * @return
 * A postive number if Id1 is bigger, negative number if Id2 is bigger and 0 if they're equal.
 * 
 */
int idCompare(MapKeyElement id1, MapKeyElement id2);


/**
 * tournamentCheckIfEnded: check if the tournament is over.
 * 
 * @param tournament - the tournament that is checked.
 * 
 * @return
 * TRUE if it is over, or FALSE if not.
 * 
 */
bool tournamentCheckIfEnded(Tournament tournament);

/**
 * printStatistics: prints to a given file the statistics of a given tournament. the statistics are printed by this order:
 *                  the tournament winner id,
 *                  the longest game time in the tournament,
 *                  the average game time of the tournament,
 *                  the tournament location,
 *                  the number of games in the tournament,
 *                  the number of players who took part in the tournament.
 *        
 * @param players - a map of all the players in the chess system.
 * @param path_file - the path of the file to which the statistics are printed to,
 * @param tournament - the tournament which its statistics are checked and printed.
 * 
 * @return
 * CHESS_SAVE_FAILURE if the file failed to open or to save the statistics printed to it.
 * CHESS_SUCCESS otherwise.
 * 
 */
ChessResult printStatistics(Map players, char* path_file, Tournament tournament);


/**
 * tournamentGetMaxGamesForPlayer: give the maximum number of games in the tournament a player can take part in.
 * 
 * @param tournament - the tournament that its maximum number of games for player is checked.
 * 
 * @return the maximum number of games for player.
 *
 */
int tournamentGetMaxGamesForPlayer(Tournament tournament);

/**
 *  gameCheckIfInTournament: check if a tournament contains a given game by it's id.
 * 
 * @param tournament - the tournament which contain the game or not.
 * @param game_id - the id of the game.
 * 
 * @return TRUE if the tournament contains the id, or FALSE if not.
 * 
 */
bool gameCheckIfInTournament(Tournament tournament, int game_id);

/**
 *  playersRankArrayCreate: creates an array to save the rank of the players.
 *                          
 * @param player_size - the number of all players in the system.
 * @param num_of_components - number of different type of players' data needs to be saved.
 * 
 * @return
 * NULL in case the allocation failed, or the array allocated otherwise.
 * 
 */
double** playersRankArrayCreate(int player_size, int num_of_components);

/**
 *  destroyArray: deallocated a given array
 *                          
 * @param map - the map that its size is the array size.
 * @param array - the array to deallocate
 * 
 */
void destroyArray(Map map, double** array);

/**
 *  tournamentGetGames: get the tournamnet's map of games.
 *                          
 * @param tournament - a specific tournament his map of games the function gets.
 *
 * @return
 * map of games
 * 
 */
Map tournamentGetGames(Tournament tournament);

/**
 *  tournamentGetId: get the tournamnet's id.
 *                          
 * @param tournament - a specific tournament his id the function gets.
 *
 * @return
 * the tournament's id
 * 
 */
int* tournamentGetId(Tournament tournament);


/**
 * idCopy: allocate a new copy of a given id.
 *                          
 * @param id - id to copy.
 *
 * @return
 * the copy of the id.
 * 
 */
MapKeyElement idCopy (MapKeyElement id);

/**
 * idFree: deallocate a given id.
 *                          
 * @param id - id to deallocate.
 * 
 */
void idFree(MapKeyElement id);


/**
 *  winnerIdUpdate: update the final winner of the tournament as soon as it's over. 
 *                          
 * @param tournaments - a map of tournaments in the chess system.
 * @param tournament_id - the id of the tournament that has ened. 
 * @param  winner_id - the id of the tournamnet's winner. 
 * 
 */
void winnerIdUpdate(Map tournaments, int tournament_id, int winner_id);

/**
 * tournamentValidateId: check if the given tournament id is valid, meaning more than 0.
 *                          
 * @param tournament_id - the id to validate. 
 * 
 * @return
 * TRUE if it is valid, or FALSE if it is not.
 * 
 */
bool tournamentValidateId (int tournament_id);

#endif /* TOURNAMENT_H_ */
