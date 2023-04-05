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
#include <assert.h>

#define NUM_OF_COMPONENTS 2
#define LEVEL 0
#define ID 1

struct player_t{
    int* player_id;
    int num_wins;
    int num_losses;
    int num_draws;
    int num_of_games;
    double play_time;
    Map participances;
};


Player playerCreate(int id)
{
    Player player = malloc(sizeof(*player));
    if(player == NULL)
        return NULL;
    
    player->player_id = malloc(sizeof(*(player->player_id)));
    if (player->player_id == NULL){
        free(player->player_id);
        return NULL;
    }
    Map participances = mapCreate(participanceCopy, idCopy, participanceDestroy, idFree, idCompare);
    if(participances == NULL){
        free(player->player_id);
        free(player);
        return NULL;
    }
    player->participances = participances;
    *(player->player_id) = id;
    return player;
}

MapDataElement playerCopy(MapDataElement player_to_copy)
{
    if (player_to_copy == NULL){
        return NULL;
    }
    Player player = player_to_copy; 

    Player new_player = malloc(sizeof(*new_player));
    if (new_player == NULL){
        return NULL;
    }

    new_player->player_id = malloc(sizeof(*(new_player->player_id)));
    if (new_player->player_id == NULL){
        free(new_player);
        return NULL;
    }
    *(new_player->player_id) = *(player->player_id);

    new_player->participances = mapCopy(player->participances);
    if (new_player->participances == NULL){
        free(new_player->player_id);
        free(new_player);
        return NULL;
    }

    new_player->num_wins = player->num_wins;
    new_player->num_losses = player->num_losses;
    new_player->num_draws = player->num_draws;
    new_player->num_of_games = player->num_of_games;
    new_player->play_time = player->play_time;
    return new_player; 
}



void playerDestroy(Player player)
{
    mapDestroy(player->participances);
    free(player->player_id);
    free(player);
}

//checks if the id is valid, meaning more than 0.
static bool idValidate(int id)
{
    if(id > 0)
        return true;
    return false;
}

ChessResult playerDataValidate(Map players, int player_id)
{
    if(!idValidate(player_id)){
        return CHESS_INVALID_ID;
    }
    if(!mapContains(players, &player_id)){
        return CHESS_PLAYER_NOT_EXIST;
    }  
    return CHESS_SUCCESS;
}


void setOpponentAsWinner(Map tournaments, int player_id)
{
    Tournament curr_tournament;
    Game curr_game;
    MAP_FOREACH(int*, tournament_iter, tournaments)
    {
        curr_tournament = mapGet(tournaments, tournament_iter);
        if(tournamentCheckIfEnded(curr_tournament)){
            free(tournament_iter);
            continue;
        }
        Map games = tournamentGetGames(curr_tournament);
        MAP_FOREACH(int*, game_iter, games)
        {
            curr_game = mapGet(games, game_iter);
            if(gameGetFirstPlayer(curr_game) == player_id)
            {
                if(gameGetWinner(curr_game) != SECOND_PLAYER)
                    gameUpdateWinner(curr_game, SECOND_PLAYER);
            }
            else if(gameGetSecondPlayer(curr_game) == player_id)
            {
                if(gameGetWinner(curr_game) != FIRST_PLAYER)
                    gameUpdateWinner(curr_game, FIRST_PLAYER);
            }
            free(game_iter);
        }
        free(tournament_iter);
    }
}


double playerCalculateAveragePlayTime(Map players, int player_id)
{
    Player player = mapGet(players, &player_id);
    double total_play_time = player->play_time;
    int num_of_games = player->num_of_games;
    return (double)(total_play_time/num_of_games);
}

static void swap(double**arr,int player1,int player2)
{
    int temp;
    for(int i = 0; i < NUM_OF_COMPONENTS; i++){
        temp=arr[player1][i];
        arr[player1][i]=arr[player2][i];
        arr[player2][i]=temp;
    }
}

//sorts a given array
static void bubbleSort(double** array, int size)
{
   for (int i = 0; i < size-1; i++)     
       for (int j = 0; j < size-i-1; j++)
           if (array[j][LEVEL] < array[j+1][LEVEL])
              swap(array, j, j+1);
}

void playerAssignLevel(double **players_array, Map players)
{
    int counter = 0;
    Player curr_player;

    MAP_FOREACH(int*, player_iter, players){
        curr_player = mapGet(players, player_iter);
        players_array[counter][LEVEL] = playerCalculateLevel(curr_player);
        players_array[counter][ID] = *(curr_player->player_id);
        counter++;
        free(player_iter);
    }
    
    int size = mapGetSize(players);
    bubbleSort(players_array, size);
}
        
double playerCalculateLevel(Player player)
{
    double num_wins, num_losses, num_draws, n;
    num_wins = player->num_wins;
    num_losses = player->num_losses;
    num_draws = player->num_draws;
    n = player->num_of_games;
    return (double)((6*num_wins-10*num_losses+2*num_draws)/n);
}


ChessResult printToFile(Map players, double** players_array, FILE* file)
{
    for(int i = 0; i < mapGetSize(players); i++){
        if(!fprintf(file, "%.0lf %.2lf\n", players_array[i][ID], players_array[i][LEVEL]))
            return CHESS_SAVE_FAILURE;
    }

    return CHESS_SUCCESS;
}

//adds a new player to the map of players in the chess system
static ChessResult addPlayer(Map players, int id)
{
    Player player = playerCreate(id);
    if(player == NULL){
        return CHESS_OUT_OF_MEMORY;
    }
    player->num_wins = 0;
    player->num_losses = 0;
    player->num_draws = 0;
    player->num_of_games = 0;
    player->play_time = 0;

    MapResult res_of_put = mapPut(players, &id, player);
    playerDestroy(player);
    if(res_of_put != MAP_SUCCESS){
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;
}


ChessResult updatePlayersData(Map players, int first_player, int second_player, Winner winner, int play_time, int tour_id){
    assert(players != NULL);
    Player player1 = mapGet(players, &first_player);
    Player player2 = mapGet(players, &second_player);

    player1->num_of_games++;
    player1->play_time += play_time;
    player2->num_of_games++;
    player2->play_time += play_time;

    if(winner == FIRST_PLAYER){
        player1->num_wins++;
        player2->num_losses++;
    }
    else if(winner == SECOND_PLAYER){
        player2->num_wins++;
        player1->num_losses++;
    }
    else{
        player1->num_draws++;
        player2->num_draws++;
    }

    Map participances1 = playerGetParticipances(player1);
    participanceRaiseNumOfGames(participances1, tour_id);
    
    Map participances2 = playerGetParticipances(player2);
    participanceRaiseNumOfGames(participances2, tour_id);

    if(winner == FIRST_PLAYER)
        participanceWinnerUpdate(player1->participances, player2->participances, tour_id);
    else if(winner == SECOND_PLAYER)
        participanceWinnerUpdate(player2->participances, player1->participances, tour_id);
    else
        participanceDrawUpdate(player1->participances, player2->participances, tour_id);
    return CHESS_SUCCESS;
}

Map playerGetParticipances(Player player)
{
    return player->participances; 
}

//checks if a given player is new in the chess system
static bool playerCheckIfNew(Map players, int player_id)
{
    if(mapContains(players, &player_id)){
        return false;
    }
    return true;
}

//adds a participance in a given tournament to a given player
static ChessResult addParticipance(int tournament_id, Player player)
{
    Participance participance = participanceCreate(player->participances, tournament_id);
    if(participance == NULL){
        return CHESS_OUT_OF_MEMORY;
    }
    MapResult res_of_put = mapPut(player->participances, &tournament_id, participance);
    participanceDestroy(participance);
    if(res_of_put != MAP_SUCCESS){
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;
}

//checks if the number of games exceeded the maximum possible
static bool isMaxGamesExceeded(int num_of_games, int max)
{
    if (num_of_games < max){
        return false;
    }
    return true;
}



ChessResult playerCheckIfCanPlayInTournament(Map players, int player_id, int tournament_id, int max_games_for_player)
{
    ChessResult res = CHESS_SUCCESS;
    
    if (playerCheckIfNew(players, player_id)){
        res = addPlayer(players, player_id);
        if (res != CHESS_SUCCESS){
            return res;
        }
        Player player = mapGet(players, &player_id); 
        res = addParticipance(tournament_id, player);
        return res;
    }


    else { //if player is already in the players map
        Player player = mapGet(players, &player_id); 
        assert(player != NULL);

        if (mapContains(player->participances, &tournament_id)){
            Participance participance = mapGet(player->participances, &tournament_id);
            assert(participance != NULL);
            int num_of_games = participanceGetNumOfGames(participance);
            if (isMaxGamesExceeded(num_of_games, max_games_for_player) == true){
                return CHESS_EXCEEDED_GAMES;
            }
            return CHESS_SUCCESS;
        }
        else {
            res = addParticipance(tournament_id, player);
            return res;
        }
    }
}
