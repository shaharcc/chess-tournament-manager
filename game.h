#ifndef _GAME_H
#define _GAME_H


#include <stdio.h>

typedef struct games_t *Game;


/**
 * gameCreate: allocates a new participance for a certain player.
 * 
 * @param id - the new game's id.
 * @param first_player - the id of the first player in the game.
 * @param second_player - the id of the second player in the game.
 * @param winner - the enum that containes the identity of the game's winner.
 * @param play_time - the game time.
 *
 * @return NULL if the allocation failed, or a pointer to the new game that was created otherwise.
 *
 */
Game gameCreate(int id, int first_player, int second_player, Winner winner, double play_time);

/**
 * gameCopy: duplicate a given game - allocate a new one and copy the data.
 *
 * @param game_to_copy - the game that is copied. Must be non-NULL.
 * 
 * @return NULL if the allocation of the new game failed, 
 *         or a pointer to the new game that was created otherwise. 
 * 
 */
MapDataElement gameCopy(MapDataElement game_to_copy);

/** 
 * gameDestroy: frees all resources of a game in a certain tournament.
 * 
 * @param generic_game - points to the game that is being removed.
 * 
 */
void gameDestroy(MapDataElement generic_game);

/**
 * gameDataValidate: checks wether the data that is given for adding a new game is valid.
 * 
 * @param tournaments - the map of tournaments that containes the tournament in question.
 * @param players - the map of players that containes the players of the game that will be added.
 * @param tournament_id - the id of the tournament to which the new game is added to.
 * @param first_player - the id of the first player in the game.
 * @param second_player - the id of the first player in the game.
 * @param play_time - the given game's time.
 * 
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_INVALID_ID - if the tournament ID number, either the players or the winner is invalid or both players
 *                        have the same ID number.
 *     CHESS_TOURNAMENT_NOT_EXIST - if the tournament does not exist in the system.
 *     CHESS_TOURNAMENT_ENDED - if the tournament already ended.
 *     CHESS_GAME_ALREADY_EXISTS - if there is already a game in the tournament with the same two players
 *                                 (both were not removed).
 *     CHESS_INVALID_PLAY_TIME - if the play time is negative.
 *     CHESS_EXCEEDED_GAMES - if one of the players played the maximum number of games allowed
 *     CHESS_SUCCESS - if all data is valid.
 * 
 */
ChessResult gameDataValidate(Map tournaments, Map players, int tournament_id, int first_player,
                            int second_player, int play_time);

/**
 * gameMakeId: return a number that will be provided to a gime as his id.
 * 
 * @param tournaments - the map of tournaments that includes all games in the system.
 * @param tournament_id - the specific tournament to which we add the new game, that
 *                        will recieve the return id.
 * 
 * @return an id.
 * 
 */
int gameMakeId(Map tournaments, int tournament_id);

/**
 * gameGetPlayTime: returns the game time.
 * 
 * @param game - the game in which the time that it took is going to be returned.
 * 
 * @return the game time.
 * 
 */
int gameGetPlayTime(Game game);

/**
 * gameGetSecondPlayer: returns the id of the first player in a given game.
 * 
 * @param game - the game in which the wanted player had played.
 * 
 * @return the player's id.
 * 
 */
int gameGetFirstPlayer(Game game);

/**
 * gameGetSecondPlayer: returns the id of the second player in a given game.
 * 
 * @param game - the game in which the wanted player had played.
 * 
 * @return the player's id.
 * 
 */
int gameGetSecondPlayer(Game game);

/**
 * gameGetWinner: returns the winner of a given game.
 * 
 * @param game - the game in which the winner is going to be returned.
 * 
 * @return the winner's enum.
 * 
 */
Winner gameGetWinner(Game game);

/**
 * gameUpdateWinner: changes the winner in a given game to his opponent.
 * 
 * @param game - the game in which the winner is going to be changed.
 * @param new_winner - the new winner of the game's enum.
 * 
 */
void gameUpdateWinner(Game game, Winner new_winner);


#endif //_GAME_H

