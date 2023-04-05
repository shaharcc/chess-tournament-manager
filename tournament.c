#include "chessSystem.h"
#include "map.h"
#include "tournament.h"
#include "game.h"
#include "player.h"
#include "participance.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdbool.h>
#include <ctype.h>

#define UNDEFINED -1
#define ID 0
#define WINS 1
#define LOSSES 2
#define DRAWS 3
#define RANK 4
#define NUM_OF_COMPONENTS 5

struct tournament_t{
    int* id;
    char* location;
    int winner_id;
    int max_games_for_player;
    bool is_still_going;
    Map games;
};

//get the number of players that have played in this tournament
static int getPlayersNum(Map players, int tournament_id)
{
    int counter = 0; 
    Player curr_player;
    Participance curr_participance;
    MAP_FOREACH(int*, player_iter, players)
    {
        curr_player = mapGet(players, player_iter); //check if player_iter is without &
        MAP_FOREACH(int*, participance_iter, playerGetParticipances(curr_player))
        {
            curr_participance = mapGet(playerGetParticipances(curr_player), participance_iter);
            int* curr_id = participanceGetId(curr_participance);
            if(idCompare(curr_id, &tournament_id) == 0){
                counter++;
            }
        free(participance_iter);
        free(curr_id);
        }
    free(player_iter);
    }
    return counter;   
}

//get the average game time of all the games in the tournament
static double getAverageGameTime(Tournament tournament, int* longest_time, int* games_num)
{
    int total_play_time = 0;
    int current_play_time = 0;
    Game curr_game;
    MAP_FOREACH(int*, game_iter, tournament->games)
    {
        curr_game = mapGet(tournament->games, game_iter);
        (*games_num)++;
        current_play_time = gameGetPlayTime(curr_game);
        total_play_time += current_play_time;
        
        if (current_play_time > *longest_time){
            *longest_time = current_play_time;     
        }
    free(game_iter);
    }
    return (double)(total_play_time / *games_num);
}

int tournamentGetMaxGamesForPlayer(Tournament tournament)
{
    return tournament->max_games_for_player; 
}

bool gameCheckIfInTournament(Tournament tournament, int game_id)
{
    if(mapContains(tournament->games, &game_id)){
        return true;
    }
    return false;
}

Map tournamentGetGames(Tournament tournament)
{
    return tournament->games;
}

int* tournamentGetId(Tournament tournament)
{
    return tournament->id;
}

int idCompare(MapKeyElement id1, MapKeyElement id2)
{
    return (*(int*)id1 - *(int*)id2);
}


MapKeyElement idCopy(MapKeyElement id)
{
    int* copied_id = malloc(sizeof(*copied_id));
    if (copied_id == NULL)
        return NULL;
    *copied_id = *((int*)id);
    return (MapKeyElement)copied_id;
}


void idFree(MapKeyElement id)
{
    free(id);
}




bool tournamentValidateId (int tournament_id)
{
    if (tournament_id < 0){
        return false;
    }
    return true;
}


void winnerIdUpdate(Map tournaments, int tournament_id, int winner_id){
    //all the conditions are checked in the mother-function
    Tournament tournament = mapGet(tournaments, &tournament_id);
    tournament->winner_id = winner_id;
}


//get the winner id by making the calculation of it score
static int CalculateWinnerId(double** player_rank, int size)
{
    int winner_index = 0;
    double max_rank = 0;
    for(int i = 0; i < size; i++){
        if(player_rank[i][RANK] > player_rank[winner_index][RANK]){
            winner_index = i;
            max_rank = player_rank[winner_index][RANK];
        }
        else if(player_rank[i][RANK] == max_rank){
            if(player_rank[i][LOSSES] < player_rank[winner_index][LOSSES]){
                winner_index =i;
                max_rank = player_rank[winner_index][RANK];
            }
            else if (player_rank[i][LOSSES] == player_rank[winner_index][LOSSES]){
                if(player_rank[i][WINS] > player_rank[winner_index][WINS]){
                    winner_index =i;
                    max_rank = player_rank[winner_index][RANK];
                }
                else if (player_rank[i][WINS] == player_rank[winner_index][WINS]){
                    if (player_rank[i][ID] < player_rank[winner_index][ID]){
                        winner_index =i;
                        max_rank = player_rank[winner_index][RANK];
                    }
                    else
                    {
                        continue;
                    }
                }
                else {
                    continue;
                }
            }
            else{
                continue;
            }
        }
        else{
            continue;
        }
    }
    return player_rank[winner_index][ID];
}


//sets the array of the players rank by putting in the data of their ids, numbers of wins, losses and draws.
static void arraySet(double** player_rank, Map players, int tournament_id)
{
    int counter = 0;
    Player curr_player;
    Participance curr_participance;
    Map player_participances;
    MAP_FOREACH(int*, player_iter, players)
    {
        curr_player = mapGet(players, player_iter);
        assert(curr_player != NULL);
        player_participances = playerGetParticipances(curr_player);
        assert(player_participances != NULL);
        MAP_FOREACH(int*, participance_iter, player_participances)
        {
            curr_participance = mapGet(player_participances, participance_iter);
            if(idCompare(participance_iter, &tournament_id) == 0)
            {
                int* id = participanceGetId(curr_participance);
                player_rank[counter][ID] = *id;
                int wins = participanceGetWins(curr_participance);
                player_rank[counter][WINS] = wins;
                int losses =  participanceGetLosses(curr_participance);
                player_rank[counter][LOSSES] = losses;
                int draws = participanceGetDraws(curr_participance);
                player_rank[counter][DRAWS] = draws;
                double rank = 2*participanceGetWins(curr_participance) + 1*participanceGetDraws(curr_participance);
                player_rank[counter][RANK] = rank;
                free(id);
            }
            free(participance_iter);
        }
        free(player_iter);
    counter++;
    }
}


ChessResult printStatistics(Map players, char* path_file, Tournament tournament) //Add checks for file functions
{
    FILE* statistics = fopen(path_file, "w");
    if (statistics == NULL)
        return CHESS_SAVE_FAILURE; 
    int winner_id = tournament->winner_id;
    int longest_time = 0, games_num = 0;
    double average_game_time = getAverageGameTime(tournament, &longest_time, &games_num);
    char *location = tournament->location;
    int num_of_players = getPlayersNum(players, *(tournament->id));
    if(!fprintf(statistics, "%d\n%d\n%.2lf\n%s\n%d\n%d\n", winner_id, longest_time,
        average_game_time, location, games_num, num_of_players)){
        fclose(statistics);
        return CHESS_SAVE_FAILURE;
    }   
    fclose(statistics);        
    return CHESS_SUCCESS;
}

//validate the given location of the tournament. Must start with capital letter that is followed by small letters or spaces.
static bool tournamentValidateLocation(const char* tournament_location)
{
    if (isupper(tournament_location[0]) == false){
        return false;
    }
    int len = strlen(tournament_location);
    for (int i=1; i<len; i++)
    {
        if (!(islower(tournament_location[i]) || isspace(tournament_location[i]))){
            return false;
        }
    }
    return true;
}


Tournament tournamentCreate(Map tournaments, int tournament_id, int max_games_per_player,
                            const char* tournament_location, ChessResult* chess_result) 
{
    if (tournament_location == NULL){
        *chess_result = CHESS_NULL_ARGUMENT;
        return NULL;
    }
    if (tournamentValidateId(tournament_id) == false){
        *chess_result = CHESS_INVALID_ID;
        return NULL;
    }
    if (mapContains(tournaments, &tournament_id) == true){
        *chess_result = CHESS_TOURNAMENT_ALREADY_EXISTS; 
        return NULL;
    }
    if (tournamentValidateLocation(tournament_location) == false){
        *chess_result =  CHESS_INVALID_LOCATION;
        return NULL;
    }
    if (max_games_per_player < 0){
        *chess_result = CHESS_INVALID_MAX_GAMES;
        return NULL; 
    }
    
    Tournament tournament = malloc(sizeof(*tournament));
    if (tournament == NULL){
        return NULL;
    }
    Map games = mapCreate(gameCopy, idCopy, gameDestroy, idFree, idCompare);
    if (games == NULL){
        return NULL;
    }
    tournament->games = games; 
    
    tournament->id = malloc(sizeof(*(tournament->id)));
    if (tournament->id == NULL){
        mapDestroy(games);
        free(tournament);
        return NULL;
    }
    *(tournament->id) = tournament_id;

    tournament->location = malloc(sizeof(*(tournament->location))*strlen(tournament_location)+1);
    if (tournament->location == NULL){
        free(tournament->id);
        mapDestroy(games);
        free(tournament);
        return NULL;
    }
    strcpy(tournament->location, tournament_location);
    tournament->winner_id = UNDEFINED; 
    tournament->max_games_for_player = max_games_per_player;
    tournament-> is_still_going = true; 
    
    return tournament; 
}

MapDataElement tournamentCopy(MapDataElement tournament_to_copy)
{
    if (tournament_to_copy == NULL){
        return NULL;
    }
    Tournament tournament_copy = (Tournament)malloc(sizeof(*tournament_copy));
    if (tournament_copy == NULL){
        return NULL;
    }
    Tournament tournament = tournament_to_copy;
    tournament_copy->games = mapCopy(tournament->games); 
    if (tournament_copy->games == NULL)
    {
        free(tournament_copy);
        return NULL;
    }
    
    tournament_copy->id = malloc(sizeof(*(tournament_copy->id)));
    if (tournament_copy->id == NULL){
        mapDestroy(tournament->games);
        free(tournament_copy);
        return NULL;
    }
    *(tournament_copy->id) = *(tournament->id);

    tournament_copy->location = malloc(sizeof(*tournament_copy->location)*strlen(tournament->location)+1);
    if (tournament_copy->location == NULL){
        free(tournament_copy->id);
        mapDestroy(tournament_copy->games);
        free(tournament_copy);
        return NULL;
    }
    strcpy(tournament_copy->location, tournament->location);
    tournament_copy->winner_id = tournament-> winner_id;
    tournament_copy->max_games_for_player = tournament->max_games_for_player;
    tournament_copy->is_still_going = tournament->is_still_going;

    return (MapDataElement)tournament_copy;
}

void tournamentDestroy(Tournament tournament)
{
    free(tournament->location);
    free(tournament->id);
    if(tournament->games != NULL){
        mapDestroy(tournament->games);
    }
    free(tournament);
}

void tournamentEnd(Map tournaments, int tournament_id)
{   
    Tournament tournament = mapGet(tournaments, &tournament_id);
    tournament->is_still_going = false;
}


double** playersRankArrayCreate(int player_size, int num_of_components)
{
    double** array = malloc(sizeof(*array)*player_size);
    if(array == NULL){
        return NULL;
    }
    for(int i = 0; i < player_size; i++){
        array[i] = malloc(sizeof(double)*num_of_components);
        if(array[i] == NULL){
            for(int j = 0; j < i; j++){
                free(array[j]);
            }
            free(array);
            return NULL; 
        }
    }
    return array;
}

void destroyArray(Map map, double** array)
{
    for(int i = 0; i < mapGetSize(map); i++)
        free(array[i]);
    free(array);
}


bool tournamentCheckIfEnded(Tournament tournament)
{
    if(tournament->is_still_going == false)
        return true;
    return false;
}

int tournamentCalculateWinnerId(Map tournaments, Map players, int tournament_id)
{
    int players_size = mapGetSize(players);
    double** player_rank = playersRankArrayCreate(players_size, NUM_OF_COMPONENTS);
    if(player_rank == NULL){
        return UNDEFINED;
    }
    arraySet(player_rank, players, tournament_id);
    int winner_id = CalculateWinnerId(player_rank, players_size); 
    destroyArray(players, player_rank);

    return winner_id;
}
