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

struct participance_t {
    int *tour_id;
    int num_of_games;
    int wins;
    int losses;
    int draws;
};

Participance participanceCreate(Map participances, int tour_id) {
    Participance participance = malloc(sizeof(*participance));
    if (participance == NULL) {
        return NULL;
    }
    participance->tour_id = malloc(sizeof(*(participance->tour_id)));
    if (participance->tour_id == NULL) {
        free(participance);
        return NULL;
    }
    *(participance->tour_id) = tour_id;
    participance->num_of_games = 0;
    participance->wins = 0;
    participance->losses = 0;
    participance->draws = 0;

    return participance; 
}
    
MapDataElement participanceCopy(MapDataElement participance_to_copy) {
    Participance new_participance = malloc(sizeof(*new_participance));
    if (new_participance == NULL){
        return NULL;
    }

    new_participance->tour_id = malloc(sizeof(*(new_participance->tour_id)));
    if (new_participance->tour_id == NULL) {
        free(new_participance);
        return NULL;
    }
    Participance participance = participance_to_copy;

    *(new_participance->tour_id) = *(participance->tour_id);
    new_participance->num_of_games = participance->num_of_games;
    new_participance->wins = participance->wins;
    new_participance->losses = participance->losses;
    new_participance->draws = participance->draws;

    return (MapDataElement)new_participance; 
}

void participanceDestroy(MapDataElement generic_participance) {
    Participance participance = generic_participance;
    free(participance->tour_id);
    free(participance);
}

int* participanceGetId(Participance participance) {
    int* id = malloc(sizeof(id));
    *id = *(participance->tour_id);
    
    return id; 
}

int participanceGetWins(Participance participance) {
    return participance->wins;
}

int participanceGetLosses(Participance participance) {
    return participance->losses;
}

int participanceGetDraws(Participance participance) {
    return participance->draws;
}

int participanceGetNumOfGames(Participance participance) {
    return participance->num_of_games;
}

void participanceRaiseNumOfGames(Map participances, int tournament_id) {
    assert (participances != NULL); 
    Participance participance = mapGet(participances, &tournament_id);
    (participance->num_of_games)++;
}

void participanceWinnerUpdate(Map winner_participances, Map loser_participances, int tour_id) {
    Participance winner_participance = mapGet(winner_participances, &tour_id);
    Participance loser_participance = mapGet(loser_participances, &tour_id);
    winner_participance->wins++;
    loser_participance->losses++;
}

void participanceDrawUpdate(Map player1_participances, Map player2_participances, int tour_id) {
    Participance player1_participance = mapGet(player1_participances, &tour_id);
    Participance player2_participance = mapGet(player2_participances, &tour_id);
    player1_participance->draws++;
    player2_participance->draws++;
}
