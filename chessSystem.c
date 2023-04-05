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
#include <ctype.h>
#include <assert.h>

#define NUM_OF_COMPONENTS 2
#define UNDEFINED -1

struct chess_system_t {
    Map tournaments; 
    Map players; 
};


ChessSystem chessCreate()
{
    ChessSystem chess_system_t = (ChessSystem)malloc(sizeof(*chess_system_t));
    if (chess_system_t == NULL){
        printf("Dynamic Allocation Error");
        return NULL;
    }
    Map tournaments = mapCreate(tournamentCopy, idCopy, (freeMapDataElements)tournamentDestroy, idFree, idCompare);                                      
    if (tournaments == NULL){
        printf("Dynamic Allocation Error");
        free(chess_system_t);
        return NULL;
    }
    chess_system_t->tournaments = tournaments;
    chess_system_t->players = mapCreate(playerCopy, idCopy, (freeMapDataElements)playerDestroy, idFree, idCompare);                                    ;
    if (chess_system_t->players == NULL){
        printf("Dynamic Allocation Error");
        mapDestroy(chess_system_t->tournaments);
        free(chess_system_t);
        return NULL;
    }
    return chess_system_t;
}

void chessDestroy(ChessSystem chess_system)
{
    if(chess_system == NULL){
        return;
    }
    mapDestroy(chess_system->tournaments);
    mapDestroy(chess_system->players);
    free(chess_system);
}

ChessResult chessAddTournament (ChessSystem chess, int tournament_id,
                                int max_games_per_player, const char* tournament_location)
{
    if (chess == NULL || tournament_location == NULL){
        return CHESS_NULL_ARGUMENT;
    }
    ChessResult res_of_create = CHESS_SUCCESS;
    Tournament tournament = tournamentCreate(chess->tournaments, tournament_id, max_games_per_player,tournament_location, &res_of_create);
    if (tournament == NULL){
        return res_of_create; 
    }
    if (res_of_create != CHESS_SUCCESS){
        tournamentDestroy(tournament);
        return res_of_create;
    }
    assert(res_of_create == CHESS_SUCCESS);
    MapResult res_of_put = mapPut(chess->tournaments, &tournament_id, tournament);

    tournamentDestroy(tournament);

    if (res_of_put != MAP_SUCCESS){
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS; 
}

ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{
    if (chess == NULL){
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id < 0){
        return CHESS_INVALID_ID;
    }
    if (mapContains(chess->tournaments, &tournament_id) == false){
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    MapResult remove_res = mapRemove(chess->tournaments, &tournament_id);
    if(remove_res != MAP_SUCCESS){
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;
}
  
ChessResult chessEndTournament(ChessSystem chess, int tournament_id)
{
    if(chess == NULL){
        return CHESS_NULL_ARGUMENT;
    }
    if (tournamentValidateId (tournament_id) == false){
        return CHESS_INVALID_ID;
    }
    if(!mapContains(chess->tournaments, &tournament_id)){
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Tournament tournament = mapGet(chess->tournaments, &tournament_id); 
    if (tournamentCheckIfEnded(tournament) == true){
        return CHESS_TOURNAMENT_ENDED;
    }
    if(mapGetSize(tournamentGetGames(tournament)) == 0){
        return CHESS_NO_GAMES;
    }
    ChessResult res = CHESS_SUCCESS;
    tournamentEnd(chess->tournaments, tournament_id);
    if (res != CHESS_SUCCESS){
        return res;
    }
    int winner_id = tournamentCalculateWinnerId(chess->tournaments, chess->players, tournament_id);
    if (winner_id == -1)
        return CHESS_OUT_OF_MEMORY; 
    winnerIdUpdate(chess->tournaments, tournament_id, winner_id);
    return res; 
}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
{
    if (chess == NULL){
        return CHESS_NULL_ARGUMENT;
    }
    Tournament curr_tournament;
    int counter = 0; 
    MAP_FOREACH(int*, tournament_iter, chess->tournaments)
    {
        curr_tournament = mapGet(chess->tournaments, tournament_iter);
        if(tournamentCheckIfEnded(curr_tournament) == true){
            counter++; 
            if (printStatistics(chess->players, path_file, curr_tournament) != CHESS_SUCCESS)
                return CHESS_SAVE_FAILURE;
            }
        free(tournament_iter);
    }
    if (counter == 0){
        return CHESS_NO_TOURNAMENTS_ENDED;
    }
    return CHESS_SUCCESS; 
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    if(chess == NULL)
        return CHESS_NULL_ARGUMENT;
    
    ChessResult validity = (gameDataValidate(chess->tournaments, chess->players, tournament_id, 
                                            first_player, second_player, play_time));
    if(validity != CHESS_SUCCESS)
        return validity;
    int game_id = gameMakeId(chess->tournaments, tournament_id);
    Tournament tournament = mapGet(chess->tournaments, &tournament_id);
    Game game = gameCreate(game_id, first_player, second_player, winner, play_time);
    if (game == NULL)
        return CHESS_OUT_OF_MEMORY;
    
    Map games = tournamentGetGames(tournament);
    MapResult res_of_put = mapPut(games, &game_id, game);
    gameDestroy(game);
    if (res_of_put != MAP_SUCCESS){
        return CHESS_OUT_OF_MEMORY;
    }

    ChessResult res_of_update = updatePlayersData(chess->players, first_player, second_player, winner, play_time, tournament_id);
    if(res_of_update != CHESS_SUCCESS)
        return res_of_update;

    return CHESS_SUCCESS;
}

ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
    ChessResult validity = playerDataValidate(chess->players, player_id);
    if(validity != CHESS_SUCCESS)
        return validity;
    setOpponentAsWinner(chess->tournaments, player_id);
    if (mapRemove(chess->players, &player_id) != MAP_SUCCESS){
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;
}

double chessCalculateAveragePlayTime (ChessSystem chess, int player_id, ChessResult* chess_result)
{
    *chess_result = CHESS_SUCCESS;
    if(chess == NULL){
        *chess_result = CHESS_NULL_ARGUMENT;
        return UNDEFINED;
    }
    *chess_result = playerDataValidate(chess->players, player_id);
    if(*chess_result != CHESS_SUCCESS){
        return UNDEFINED;
    }
    double average_time = playerCalculateAveragePlayTime(chess->players, player_id);
    return average_time;
}

ChessResult chessSavePlayersLevels (ChessSystem chess, FILE* file)
{
    if(chess == NULL){
        return CHESS_NULL_ARGUMENT;
    }
    int players_size = mapGetSize(chess->players);
    double **players_array = playersRankArrayCreate(players_size, NUM_OF_COMPONENTS);
    playerAssignLevel(players_array, chess->players);
    if(!printToFile(chess->players, players_array, file))
    {
        destroyArray(chess->players, players_array);
        return CHESS_SAVE_FAILURE;
    }
    destroyArray(chess->players, players_array);
    return CHESS_SUCCESS;
}

