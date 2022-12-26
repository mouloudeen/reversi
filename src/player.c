#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/player.h"

static bool MovePassed = false;

/* This function return a random move amongs the possibles ones. */
move_t random_player(board_t *board) {
  size_t nbMoves = board_count_player_moves(board);

  if (!MovePassed) {
    srand(time(NULL));
    MovePassed = true;
  }

  size_t r = rand() % nbMoves;

  for (size_t i = 0; i <= r; i++) {
    board_next_move(board);
  }
  return board_next_move(board);
}

/* Delete spaces and tabulations on the string*/
void DeleteSpaces(char *buffer) {
  char *newBuffer = NULL;

  for (newBuffer = buffer; (*buffer != '\n'); ++buffer) {
    if ((*buffer != '\000') && (*buffer != '\t') && *buffer != ' ') {
      *(newBuffer++) = *buffer;
    }
  }
  *(newBuffer) = NULL;
}

/* This function save the content of the board in a file given by user. */
static void game_save(board_t *board) {

  char *buffer = NULL;
  size_t n = 0;
  FILE *fd = NULL;

  fprintf(stdout, "Give a filename to save the game (default: 'board.txt'): ");
  getline(&buffer, &n, stdin);

  if ((strlen(buffer) == 0) || (strlen(buffer) == 1)) {
    fd = fopen("board.txt", "w+");
  }

  else {
    DeleteSpaces(buffer);
    fd = fopen(buffer, "w+");
  }

  fprintf(fd, "%c\n", board_player(board));

  for (size_t i = 0; i < board_size(board); i++) {
    for (size_t j = 0; j < board_size(board); j++) {
      disc_t disc = board_get(board, i, j);
      if (disc == HINT_DISC) {
        disc = EMPTY_DISC;
      }
      fprintf(fd, "%c", disc);
    }
    fprintf(fd, "\n");
  }

  free(buffer);
  buffer = NULL;

  fclose(fd);
}

/* This function will interact with the user in order  to ask move.*/
move_t human_player(board_t *board) {

  char *buffer = NULL;

  size_t n = 0;
  bool answers = false;

  size_t rows = 0;
  size_t cols = 0;

  move_t move;

  while (answers == false) {
    answers = true;
    if (board_size(board) == 10) {
      fprintf(stdout, "   ");

      for (size_t i = 0; i < board_size(board); i++) {
        fprintf(stdout, "%c ", 'A' + (int)i);
      }

      fprintf(stdout, "\n");

      for (size_t i = 0; i < board_size(board); i++) {

        if (i == 9) {
          fprintf(stdout, "%lu ", i + 1);
        }

        else {
          fprintf(stdout, " %lu ", i + 1);
        }

        for (size_t j = 0; j < board_size(board); j++) {
          fprintf(stdout, "%c ", board_get(board, i, j));
        }

        fprintf(stdout, "\n");
      }
    }

    else {
      fprintf(stdout, "  ");

      for (size_t i = 0; i < board_size(board); i++) {
        fprintf(stdout, "%c ", 'A' + (int)i);
      }

      fprintf(stdout, "\n");

      for (size_t i = 0; i < board_size(board); i++) {
        fprintf(stdout, "%.1lu ", i + 1);

        for (size_t j = 0; j < board_size(board); j++) {
          fprintf(stdout, "%c ", board_get(board, i, j));
        }
        fprintf(stdout, "\n");
      }
    }
    fprintf(stdout,
            "\n"
            "Score: 'X' = %u, 'O' = %u\n\n",
            board_score(board).black, board_score(board).white);

    fprintf(stdout,
            "\n\n"
            "%c player's turn.\n",
            board_player(board));
    fprintf(stdout,
            "Give your move (e.g 'A5' or 'a5'), press 'q' or 'Q' to quit: ");
    getline(&buffer, &n, stdin);
    DeleteSpaces(buffer);

    if (islower(buffer[0])) {

      cols = buffer[0] - 'a';
    }

    if (isupper(buffer[0])) {
      cols = buffer[0] - 'A';
    }

    rows = (buffer[1]) - '1';

    if (buffer[1] == '1' && buffer[2] == '0') {
      rows = 9;
    }

    char q = 'n';
    for (size_t i = 0; i < strlen(buffer); i++) {
      if (buffer[i] == 'q' || buffer[i] == 'Q') {
        q = 'y';
        break;
      }
    }

    if (q == 'y') {
      q = 'N';
      fprintf(stdout, "Quitting, do you want to save this game (y/N),\n");
      getline(&buffer, &n, stdin);

      for (size_t i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == 'y' || buffer[i] == 'Y') {
          q = 't';
          break;
        }
      }
      if (q != 't') {
        free(buffer);
        return (move_t){.row = 15, .column = 15};
      }

      else {
        game_save(board);
        free(buffer);
        return (move_t){.row = 20, .column = 20};
      }
    }

    if ((strlen(buffer) > 4) ||
        ((!isalpha(buffer[0])) && (!isdigit(buffer[1])))) {
      answers = false;
      fprintf(stdout, "Wrong input, try again!\n");
    }

    if (cols >= board_size(board)) {
      answers = false;
      fprintf(stdout, "Row out of bounds. Wrong input, try again!\n");
    }

    if (rows >= board_size(board)) {
      answers = false;
      fprintf(stdout, "Column out of bounds. Wrong input, try again!\n");
    }

    if (!answers) {
      fprintf(stdout, "This move is invalid. Wrong input, try again!\n");
    }

    if ((cols < board_size(board)) && (rows < board_size(board))) {
      answers = true;
      move = (move_t){.row = rows, .column = cols};
    }
  }
  free(buffer);
  return move;
}
