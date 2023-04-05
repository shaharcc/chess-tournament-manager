#include "chessSystem.h"
#include "map.h"
#include "tournament.h"
#include "game.h"
#include "player.h"
#include "participance.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct games_t{
    int* game_id;
    int first_player;
    int second_player;
    Winner winner;
    double play_time;
};

Game gameCreate(int id, int first_player, int second_player, Winner winner, double play_time) {
    Game game = malloc(sizeof(*game));
    if(game == NULL)
        return NULL;

    game->game_id = malloc(sizeof(*(game->game_id)));
    if (game->game_id == NULL) {
        free(game);
        return NULL;
    }
    *(game->game_id) = id;
    game->first_player = first_player;
    game->second_player = second_player;
    game->winner = winner;
    game->play_time = play_time;
    return game; 
}

MapDataElement gameCopy(MapDataElement game_to_copy) {
    Game new_game = malloc(sizeof(*new_game));
    if (new_game == NULL)
        return NULL;
        
    new_game->game_id = malloc(sizeof(*(new_game->game_id)));
    if (new_game->game_id == NULL) {
        free(new_game);
        return NULL;
    }
    Game game = game_to_copy;

    *(new_game->game_id) = *(game->game_id);
    new_game->first_player = game->first_player;
    new_game->second_player = game->second_player;
    new_game->winner = game->winner;
    new_game->play_time = game->play_time;
    return (MapDataElement)new_game; 
}

void gameDestroy(MapDataElement generic_game) {
    Game game = generic_game;
    free(game->game_id);
    free(game);
}

// checks wether a given id is valid or not.
static bool idValidate(int id) {
    if(id > 0)
        return true;
    return false;
}

// checks wether a given plaer is in the chess system or not.
static bool isPlayerInSystem(Map players, int player_id) {
    if(mapContains(players, &player_id))
        return true;
    return false;
}

// checks if a pair of players had played together in a given tournament or not.
static bool checkIfPlayersPlayedTogether(Tournament tournament, int first_player, int second_player) {
    Map games = tournamentGetGames(tournament);
    Game curr_game;
    int original_first_player;
    int original_second_player;
    MAP_FOREACH(int*, game_iter, games){
        curr_game = mapGet(games, game_iter);
        original_first_player = curr_game->first_player;
        original_second_player = curr_game->second_player;
        if((first_player == original_first_player && second_player == original_second_player) ||
           (first_player == original_second_player && second_player == original_first_player)){
               return true;
           }
    free(game_iter);
    }
    return false;
}

ChessResult gameDataValidate(Map tournaments, Map players, int tournament_id, int first_player,
                            int second_player, int play_time) {
    if(players == NULL)
        return CHESS_NULL_ARGUMENT;

    if(!idValidate(tournament_id) || !idValidate(first_player) || !idValidate(second_player))
        return CHESS_INVALID_ID;

    if(first_player == second_player)
        return CHESS_INVALID_ID;

    if(!mapContains(tournaments, &tournament_id))
        return CHESS_TOURNAMENT_NOT_EXIST;

    Tournament tournament = mapGet(tournaments, &tournament_id);
    if(tournamentCheckIfEnded(tournament) == true)
        return CHESS_TOURNAMENT_ENDED;

    if((isPlayerInSystem(players, first_player)) && (isPlayerInSystem(players, second_player))){
        if(checkIfPlayersPlayedTogether(tournament, first_player, second_player)){
            return CHESS_GAME_ALREADY_EXISTS;
        }
    }

    if(play_time < 0)
        return CHESS_INVALID_PLAY_TIME;

    int max_games_for_player = tournamentGetMaxGamesForPlayer(tournament);
    ChessResult res1 = playerCheckIfCanPlayInTournament(players, first_player, tournament_id, max_games_for_player);
    ChessResult res2 = playerCheckIfCanPlayInTournament(players, second_player, tournament_id, max_games_for_player);

    if((res1 == CHESS_SUCCESS) && (res2 == CHESS_SUCCESS)){
        return CHESS_SUCCESS;
    }  
    else if((res1 == CHESS_EXCEEDED_GAMES) || (res2 == CHESS_EXCEEDED_GAMES)){
        return CHESS_EXCEEDED_GAMES;
    }
    else{
        return CHESS_OUT_OF_MEMORY;
    }    
}

int gameMakeId(Map tournaments, int tournament_id)  {
    Tournament tournament = mapGet(tournaments, &tournament_id);
    Map games = tournamentGetGames(tournament);
    int size = mapGetSize(games);
    return size+1;
}

int gameGetPlayTime(Game game) {
    return game->play_time;
}

int gameGetFirstPlayer(Game game) {
    return game->first_player;
}

int gameGetSecondPlayer(Game game) {
    return game->second_player;
}

Winner gameGetWinner(Game game) {
    return game->winner;
}

void gameUpdateWinner(Game game, Winner new_winner) {
    game->winner = new_winner;
}
