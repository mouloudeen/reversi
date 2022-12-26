#define _POSIX_C_SOURCE 200809L

#include "reversi.h"
#include "../include/board.h"
#include "../include/player.h"
#include <limits.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <err.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

#define MAX_LENGTH 512

static int optc = 0;
static bool contest_mode = false;
static bool verbose = false;
static size_t size_board = 8;
static size_t black_ai = 0;
static size_t white_ai = 0;

struct option long_opts[] = {{"black-ai", optional_argument, NULL, 'b'},
                             {"contest", no_argument, NULL, 'c'},
                             {"help", no_argument, NULL, 'h'},
                             {"size", required_argument, NULL, 's'},
                             {"verbose", no_argument, NULL, 'v'},
                             {"version", no_argument, NULL, 'V'},
                             {"white-ai", optional_argument, NULL, 'w'},
                             {NULL, 0, NULL, 0}};

/* function display help */
void help(void) {
  printf("Usage: reversi [-s SIZE|-b [N]|-w [N]|-c|-v|-V|-h] [FILE]\n"
         "Play a reversi game with human or program players.\n"
         "  -s, --size SIZE\t\tboard size(min=1, max=5 (default=4))\n"
         "  -b, --black-ai [N]\tset tactic of black player (default: 0)\n"
         "  -w, --white-ai [N]\tset tactic of white player (default: 0)\n"
         "  -c, --contest\t\tenable 'contest' mode\n"
         "  -v, --verbose\t\tverbose output\n "
         "  -V, --version\t\tdisplay version and exit\n"
         "  -h, --help\t\tdisplay this help and exit\n"
         "Tactic list: human (0), random (1)\n ");
  exit(EXIT_SUCCESS);
}
/* function display version of the game */
void version(void) {
  printf("reversi %d.%d.%d \n", VERSION, SUBVERSION, REVISION);
  printf("This software allows to play to reversi game. \n");
  exit(EXIT_SUCCESS);
}
/* This function is a parser able to read a file and extract the needed
   information to build the current board.*/
static board_t *file_parser(const char *filename) {
  FILE *inputfile = NULL;
  board_t *p = NULL;

  disc_t player = EMPTY_DISC;
  inputfile = fopen(filename, "r+ ");
  if (!inputfile) {
    perror("reversi : \n");
    exit(EXIT_FAILURE);
  }

  /* Getting first line  */

  char c = fgetc(inputfile);

  while (c == '\n') {
    c = fgetc(inputfile);
    if (c == EOF) {
      break;
    }
  }

  while (c != '\n') {
    switch (c) {

    case ' ':
    case '\t':
      c = fgetc(inputfile);
      break;

    case 'X':
      if (player == BLACK_DISC || player == WHITE_DISC) {
        fprintf(stderr, "reversi : player is incorrect or missing\n");
        fclose(inputfile);
        exit(EXIT_FAILURE);
      }
      c = fgetc(inputfile);
      player = BLACK_DISC;
      break;

    case 'O':
      if (player == BLACK_DISC || player == WHITE_DISC) {
        fprintf(stderr, "reversi : player is incorrect or missing\n");
        fclose(inputfile);
        exit(EXIT_FAILURE);
      }
      c = fgetc(inputfile);
      player = WHITE_DISC;
      break;

    case '#':
      while (c != '\n') {
        c = fgetc(inputfile);
        if (c == EOF) {
          break;
        }
      }
      c = fgetc(inputfile);
      break;

    default:
      fprintf(stderr, "reversi : player is incorrect or missing\n");
      fclose(inputfile);
      exit(EXIT_FAILURE);
      break;
    }
  }

  disc_t firstline[10];

  c = fgetc(inputfile);
  size_t count = 0;

  while (c == '\n') {
    c = fgetc(inputfile);
    if (c == EOF) {
      break;
    }
  }

  /* Getting first line  */
  while (c != '\n') {
    switch (c) {

    case EMPTY_DISC:
    case BLACK_DISC:
    case WHITE_DISC:

      firstline[count] = c;

      count = count + 1;
      c = fgetc(inputfile);
      break;

    case ' ':
    case '\n':
      c = fgetc(inputfile);
      break;

    case '#':

      while (c != '\n') {
        c = fgetc(inputfile);
        if (c == EOF) {
          break;
        }
      }
      break;

    default:
      fprintf(stderr, "reversi : error: wrong character %c at line 0\n", c);
      fclose(inputfile);
      exit(EXIT_FAILURE);
      break;
    }
  }
  /* Size is correct or no*/
  if ((count != 2) && (count != 4) && (count != 6) && (count != 8) &&
      (count != 10)) {

    fprintf(stderr, "reversi : error: board's size is incorrect.\n ");
    fclose(inputfile);
    exit(EXIT_FAILURE);
  }
  p = board_alloc(count, player);
  for (size_t i = 0; i < count; i++) {
    board_set(p, firstline[i], 0, i);
  }

  c = fgetc(inputfile);
  size_t count1 = 0;
  size_t nbRows = 1;

  /* Watch others lines of the board*/
  while (c != EOF) {
    switch (c) {

    case EMPTY_DISC:
    case BLACK_DISC:
    case WHITE_DISC:
      board_set(p, c, nbRows, count1);

      count1 = count1 + 1;

      c = fgetc(inputfile);
      break;

    case ' ':
      c = fgetc(inputfile);
      break;

    case '\n':

      if (count1 != count && count1 != 0) {
        fprintf(stderr,
                "reversi: error : row %ld is malformed! (wrong number of "
                "columns)\n",
                nbRows);
        fclose(inputfile);
        exit(EXIT_FAILURE);
      }
      if (count1 != 0) {
        nbRows = nbRows + 1;
      }
      c = fgetc(inputfile);
      count1 = 0;
      break;

    case '#':
      while (c != '\n') {
        c = fgetc(inputfile);
        if (c == EOF) {
          break;
        }
      }
      break;

    default:
      fprintf(stderr, "reversi: error : wrong character %c at line %ld.\n", c,
              nbRows);
      fclose(inputfile);
      exit(EXIT_FAILURE);
      break;
    }
  }
  /* Testing difference between rows and colums */
  if (count != nbRows) {
    if (count > nbRows) {
      size_t diff = count - nbRows;
      fprintf(stderr, "reversi : error: board has %ld missing row(s).\n", diff);
      fclose(inputfile);
      exit(EXIT_FAILURE);
    }
    if (count < nbRows) {
      size_t diff = nbRows - count;
      fprintf(stderr, "reversi : error: board has %ld extra row(s).\n", diff);
      fclose(inputfile);
      exit(EXIT_FAILURE);
    }
  }

  fclose(inputfile);
  return p;
}

/*Array  of move_t*/
move_t (*table_play[2])(board_t *) = {human_player, random_player};

char blackm[10];
char whitem[10];

/*This function launch reversi game. */
static int game(move_t (*black)(board_t *), move_t (*white)(board_t *),
                board_t *board) {

  if (black_ai == 0) {
    strcat(blackm, "human");
  }

  if (black_ai == 1) {
    strcat(blackm, "random");
  }

  if (white_ai == 0) {
    strcat(whitem, "human");
  }

  if (white_ai == 1) {
    strcat(whitem, "random");
  }

  fprintf(stdout,
          "\n\n"
          "Welcome to this reversi game!\n"
          "Black player (X) is %s and white player (O) is %s.\n",
          blackm, whitem);
  fprintf(stdout, "Black player start!\n"
                  "\n");

  move_t move = (move_t){0, 0};

  while ((board_player(board) != EMPTY_DISC)) {

    if (board_player(board) == BLACK_DISC) {
      move = (*black)(board);
    } else {
      move = (*white)(board);
    }

    /* Conditions for stop this game*/
    if (((move.row == 15) && (move.column == 15)) ||
        ((move.row == 20) && (move.column == 20))) {
      break;
    }

    if ((black_ai == 1) || (white_ai == 1)) {
      printf("\n\n");
      board_print(board, stdout);
    }

    board_play(board, move);
  }
  /*Player resigned */
  if ((move.row == 15) && (move.column == 15)) {
    disc_t player = board_player(board);
    disc_t opponent;
    int nbre = 0;

    if (player == BLACK_DISC) {
      opponent = WHITE_DISC;
      nbre = -1;
    }

    else {
      opponent = BLACK_DISC;
      nbre = -2;
    }

    fprintf(stdout, " Player '%c' resigned. Player '%c' win the game.\n ",
            player, opponent);
    return nbre;
  }

  if ((move.row == 20) && (move.column == 20)) {
    fprintf(stdout, "Thanks for playing, see you soon for end of game!\n");
    return 0;
  }

  score_t score = board_score(board);
  size_t sblack = score.black;
  size_t swhite = score.white;

  board_print(board, stdout);

  int win = 0;
  disc_t winner = EMPTY_DISC;

  /* Find the winner of the game or draw*/
  if (sblack == swhite) {
    printf(
        "\n\n////////////////////////////////////////////////////////////////"
        "//\n\n"
        "Draw game, no winner.\n\n"
        "/////////////////////////////////////////////////////////////////"
        "\n\n\n");
    win = 0;

  }

  else {

    if (sblack > swhite) {
      winner = BLACK_DISC;
      win = 1;
    }

    else {
      winner = WHITE_DISC;
      win = 2;
    }
    printf(
        "\n\n////////////////////////////////////////////////////////////////"
        "//\n\n"
        "Player '%c' win the game!\n",
        winner);
    printf("\n/////////////////////////////////////////////////////////////////"
           "\n\n\n");
  }
  /*End of the game*/
  fprintf(stdout, "Thanks for playing, see you soon!\n");
  return win;
}

int main(int argc, char *argv[]) {

  while ((optc = getopt_long(argc, argv, "b::chs:vVw::", long_opts, NULL)) !=
         -1)
    switch (optc) {
    case 'b': /* option  black player */
      if (optarg != NULL) {
        black_ai = atoi(optarg);
        if ((((atoi(optarg)) < 0) || ((atoi(optarg)) > 1))) {
          printf("lol your argument T_T \n");
          exit(EXIT_FAILURE);
        }
      } else {
        printf("you actived b with the argument default. \n");
      }
      break;

    case 'c': /* option for contest */
      contest_mode = true;
      break;

    case 'h': /* option for display help. */
      help();
      break;

    case 's': /* option for size of board. */
      if (((atoi(optarg)) > 0) && ((atoi(optarg)) < 6)) {
        size_board = 2 * atoi(optarg);
      } else {
        printf("The number must be between 1 and 5 \n");
        exit(EXIT_FAILURE);
      }
      break;

    case 'v': /* option for verbose. */
      verbose = true;
      break;

    case 'V': /* option for display the version of the game. */
      version();
      break;

    case 'w': /* option  white player */
      if (optarg != NULL) {
        white_ai = atoi(optarg);
        if ((((atoi(optarg)) < 0) || ((atoi(optarg)) > 1))) {
          printf("lol your argument T_T \n");
          exit(EXIT_FAILURE);
        }
      } else {
        printf("you actived w with the argument default. \n");
      }
      break;

    default:
      printf("Tri 'reversi --help' for more information. \n");
      exit(EXIT_FAILURE);
    }

  board_t *board = NULL;

  if (optind < argc) {
    board = file_parser(argv[optind]);
    change_game(board);
  } else {
    board = board_init(size_board);
  }
  if (contest_mode) {
    if (optind >= argc) {
      errx(EXIT_FAILURE, "missing argument\n");
    }

    move_t move = random_player(board);
    free(board);
    fprintf(stdout, "%c%ld\n", (int)move.column + 'A', move.row + 1);
  } else {

    move_t (*black)(board_t *);

    move_t (*white)(board_t *);

    black = table_play[black_ai];
    white = table_play[white_ai];

    game(black, white, board);

    free(board);

    return EXIT_SUCCESS;
  }
  return EXIT_SUCCESS;
}