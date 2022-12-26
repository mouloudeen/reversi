#ifndef BOARD_H
#define BOARD_H

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Min/Max width board */
#define MIN_BOARD_SIZE 2
#define Max_BOARD_SIZE 10

/* uint */
typedef unsigned int uint;

/* Base bitboard type */
typedef unsigned __int128 bitboard_t;

typedef unsigned long long uint64;

/* Board discs */
typedef enum {
  BLACK_DISC = 'X',
  WHITE_DISC = 'O',
  EMPTY_DISC = '_',
  HINT_DISC = '*'
} disc_t;

/* A move in the reversi game */
typedef struct {
  size_t row;
  size_t column;
} move_t;

/* Store the score of a game */
typedef struct {
  unsigned short black;
  unsigned short white;
} score_t;

/* Reversi board (forward declaration to hide the implementation) */
typedef struct board_t board_t;

/* This function creates an empty Board with player and size given */
board_t *board_alloc(const size_t size, const disc_t player);

/* This function free the memory allocated to hold board */
void board_free(board_t *board);

/* This function initialize all squares of the board as a starting game */
board_t *board_init(const size_t size);

/* This function perform a deep copy of the board structure */
board_t *board_copy(const board_t *board);

/* This function return the size of board */
size_t board_size(const board_t *board);

/* This function return the player of board */
disc_t board_player(const board_t *board);

/* This function setting the current player of the given board */
void board_set_player(board_t *board, disc_t player);

/* This function get the content of the square at the given coordinates */
disc_t board_get(const board_t *board, const size_t row, const size_t column);

/* This function set the given disc at the given position */
void board_set(board_t *board, const disc_t disc, const size_t row,
               const size_t column);

/* This function return the score of the given board */
score_t board_score(const board_t *board);

/* This function calculated number of possibles moves */
size_t board_count_player_moves(board_t *board);

/* This function check if a move is valid within given board */
bool board_is_move_valid(const board_t *board, const move_t move);

/* This function apply a move according according to rules and set the board for
 * next move */
bool board_play(board_t *board, const move_t move);

/* This function store the next possible move to be examined */
move_t board_next_move(board_t *board);

/* Calculate numbers of caracters in file */
int numberofCharacters(FILE *f);

/* This function  write on the file descriptor fd or created FILE fd and return
  the number of printed characters */
int board_print(const board_t *board, FILE *fd);

/* This function change the player at the beginning of the game.*/
void change_game(board_t *board);

#endif /* BOARD_H */