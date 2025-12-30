#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_SNAKES 9
#define NUM_LADDERS 6
#define BOARD_SIZE 100
#define MAX_PLAYERS 4
char *player_colors[MAX_PLAYERS] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m"}; 

//SNAKES POSITION: FROM TOP TO BOTTOM
int snakes[9] = {14,25,51,61,65,95,97,98,99};
int snakes_tail[9] = {5,19,31,42,57,91,77,83,22};

//LADDERS POSITION: FROM BOTTOM TO TOP
int ladders[6] = {9,23,32,58,66,79};
int ladders_top[6] = {12,37,48,62,77,82};

int player_ids[MAX_PLAYERS];
int player_positions[MAX_PLAYERS];
int num_players = 0;
int current_player = 0;

void setup_game() {
    printf("WELCOME TO SNAKE AND LADDER GAME!\n");
    printf("ENTER THE NUMBER OF PLAYERS(1-%d): ", MAX_PLAYERS);
    while ((scanf("%d", &num_players) != 1) || num_players < 1 || num_players > MAX_PLAYERS) {
        while(getchar() != '\n'); // Clear invalid input
        printf("INVALID INPUT.ENTER THE NUMBER OF PLAYER (1-%d): ", MAX_PLAYERS);
    }
    for(int i=0; i < num_players; i++) {
        player_ids[i] = i + 1;
        player_positions[i] = 0;
    }
    printf("GAME SETUP COMPLETE. PLAYERS: %d\n", num_players);
    srand(time(NULL));
}

int roll_dice() {
    return (rand() % 6) + 1;
}

int rolls[MAX_PLAYERS];
int max_roll = 0;
int first_player = 0;

void decide_first_player() {
    printf("\nROLLING A DICE TO DECIDE WHO PLAY FIRST...\n");
    for(int i = 0; i < num_players; i++) {
        rolls[i] = roll_dice();
        printf("PLAYER %d ROLLED: %d\n", player_ids[i], rolls[i]);
        if(rolls[i] > max_roll) {
            max_roll = rolls[i];
            first_player = i;
        }
    }
    printf("PLAYER %d GOES FIRST!\n", player_ids[first_player]);
    current_player = first_player;
}

int snake_or_ladder(int position) {
    for(int i = 0; i < NUM_SNAKES; i++) {
        if(position == snakes[i]) {
            printf("YOU'VE BEEN BITTEN BY SNAKE! SLIDE DOWN TO %d\n", snakes_tail[i]);
            return snakes_tail[i];
        }
    }
    for(int i = 0; i < NUM_LADDERS; i++) {
        if(position == ladders[i]) {
            printf("WOW,A LADDER! GOING UP TO %d\n", ladders_top[i]);
            return ladders_top[i];
        }
    }
    return position;
}
void print_board() {
    printf("\nCURRENT BOARD STATUS:\n");
    for(int i = 0; i < num_players; i++) {
        printf("%sPLAYER %d\033[0m: POSITION %d\n", player_colors[i], player_ids[i], player_positions[i]);
    }
    for (int row = 10; row >= 1; row--) {
        for (int col = 0; col < 10; col++) {
            int pos;
            if (row % 2 == 0) {
                pos = (row * 10) - col;
            } else {
                pos = ((row - 1) * 10) + col + 1;
            }
            int occupied = -1;
            for (int j = 0; j < num_players; j++) {
                if (player_positions[j] == pos) {
                    occupied = j;
                    break;
                }
            }
            if (occupied != -1) {
                printf("[%s%5d\033[0m]", player_colors[occupied], player_ids[occupied]);
            } else {
                printf("[%5d]", pos);
            }
        }
        printf("\n");
    }
    printf("\n");
}

int main(){
    setup_game();
    decide_first_player();
    print_board();
    print_board();

    int winner = 0;
    while (winner == 0) {
        printf("%sPLAYER %d\033[0m'S TURN. PRESS ENTER TO ROLL THE DICE...", player_colors[current_player], player_ids[current_player]);
        getchar();

        int dice = roll_dice();
        printf("%sPLAYER %d\033[0m ROLLED %d\n", player_colors[current_player], player_ids[current_player], dice);

        player_positions[current_player] += dice;

        if (player_positions[current_player] > BOARD_SIZE) {
            /* undo move and inform player */
            player_positions[current_player] -= dice;
            printf("ROLL EXCEEDS BOARD LIMIT. STAYING AT %d\n", player_positions[current_player]);
        } else {
            player_positions[current_player] = snake_or_ladder(player_positions[current_player]);
            printf("%sPLAYER %d\033[0m MOVED TO %d\n", player_colors[current_player], player_ids[current_player], player_positions[current_player]);
        }

        if (player_positions[current_player] == BOARD_SIZE) {
            printf("CONGRATULATIONS %sPLAYER %d\033[0m WINS THE GAME!\n", player_colors[current_player], player_ids[current_player]);
            winner = 1;
        } else {
            current_player = (current_player + 1) % num_players;
        }
        print_board();
    }

    return 0;
}