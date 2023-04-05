#ifndef _PLAYERS_H
#define _PLAYERS_H

#include <stdio.h>

/** Type for representing one player */
typedef struct player_t* Player;

/**
 * playerCreate: allocates a new player and it's resources.
 * 
 * @param id - the id of the new player created. Must be unique. 
 *
 * @return
 * NULL if the allocation failed, or the new tournament created otherwise.
 * 
 */
Player playerCreate(int id);

/**
 * playerCopy: Allocates a copy of an existing player and copy all its data to the new one.
 * 
 * @param player - target player. 
 *
 * @return
 * 	NULL if a NULL was sent or a memory allocation failed.
 * 	A new copy of the player otherwise.
 * 
 */
MapDataElement playerCopy(MapDataElement player_to_copy);


/**
 * playerDestroy: destroy an existing player and deallocates all his resources.
 * 
 * @param player - the player that is going to be removed.
 * 
 */
void playerDestroy(Player player);


/**
 * playerDataValidate: validate the player's information given.
 * 
 * @param players - a map of all players in the chess system. 
 * @param player_id - the id of the player its data is validated.
 * 
 * @return
 * CHESS_INVALID_ID - if the player id is invalid, meaning it is less than 0.
 * CHESS_PLAYER_NOT_EXIST - if the player doesn't exist in the players' map.
 * CHESS_SUCCESS - otherwise.
 */
ChessResult playerDataValidate(Map players, int player_id);


/**
 * setOpponentAsWinner: in case of a player that is being removed from a tournament which is still going, sets the other player as the winner of the game.
 * 
 * @param tournaments - a map of all tournaments in the chess system. 
 * @param player_id - the id of the player that is removed and his opponents is set as winner.
 * 
 */
void setOpponentAsWinner(Map tournaments, int player_id);

/**
 * updatePlayersData: after a game is other, updates the game data for both players. 
 * 
 * @param players - a map of all players in the chess system. 
 * @param fisrt_player - the id of the first player in the game.
 * @param second_player - the id of the second player in the game.
 * @param winner - the winner of the game. Could be the fisrt player, the second one or a draw.
 * @param play_time - the total playtime of the game.
 * @param tournament_id - the tournament to which the game belongs.
 * 
 * @return
 * CHESS_OUT_OF_MEMORY if the creation of a new participance for the player in the tournament faild.
 * CHESS_SUCCESS otherwise.
 * 
 */
ChessResult updatePlayersData(Map players, int first_player, int second_player, Winner winner, int play_time, int tournament_id);


/**
 * playerGetParticipances: gets a map of all the player participances in all the different tournaments and their information.
 * 
 * @param player - a player of whom the participances in the tournament's games is gotten.  
 * 
 * @return 
 * a map of the player's participances
 */
Map playerGetParticipances (Player player);

/**
 * playerCalculateLevel: calculate the level of each player in the chess system. 
 *                       player level = 6*(number of his wins) - 10*(number of his losses) +2*(number of his draws)
 * 
 * @param player - a player whose level is calculated.  
 * 
 * @return 
 * the player's level.
 */
double playerCalculateLevel(Player player);

/**
 * playerAssignLevel: assign all the players levels in the array given.
 * 
 * @param players_array - an array that contains the level of each player in the chess system.
 * @param players - a map of all the players in the cess system.
 * 
 */
void playerAssignLevel(double **players_array, Map players);



/**
 * printToFile: prints to a given file the id and the level of each player in the chess system.
 * 
 * @param players - a map of all the players in the cess system.
 * @param players_array - an array that contains the level of each player in the chess system.
 * @param file - a file to which the data is printed
 * 
 * @return
 * CHESS_SAVE_FAILURE if failed to save the data printed to it.
 * CHESS_SUCCESS otherwise.
 * 
 */
ChessResult printToFile(Map players, double** players_array, FILE* file);

/**
 * playerCalculateAveragePlayTime: calculates the average play time of a given player by dividing his total play time in his numbers of games.
 * 
 * @param players - a map of all the players in the cess system.
 * @param player_id - the id of the player.
 * 
 * @return
 * the player's average play time.
 * 
 */
double playerCalculateAveragePlayTime(Map players, int player_id);

/**
 *  playerCheckIfCanPlayInTournament: checks if the player can play in a given tournament, meaning he hasen't exceed the max num of games yet,
 *                                    
 * 
 * @param players - a map of all the players in the cess system.
 * @param player_id - the id of the player.
 * @param tournament_id - the id of the tournament.
 * @param max_games_for_player - the max number of games a player can take part in in this tournamant.
 * 
 * @return
 * CHESS_OUT_OF_MEMORY - if it's a new player and his allocation failed.
 * CHESS_EXCEEDED_GAMES - if it's a old player that has already exceeded the max num of games for this tournament.
 * CHESS_SUCCESS - otherwise, meaning the player can play in the tournament.
 */
ChessResult playerCheckIfCanPlayInTournament(Map players, int player_id, int tournament_id, int max_games_for_player);

#endif //_PLAYERS_H



