#include "../include/board.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Internal board_t structure  (hiden from the outside) */
struct board_t {
  size_t size;
  disc_t player;
  bitboard_t black;
  bitboard_t white;
  bitboard_t moves;
  bitboard_t next_move;
};

/* This function that set the bit with the bit with  row and column to '1' in
 * the returned bitboard */
static bitboard_t set_bitboard(const size_t size, const size_t row,
                               const size_t column) {

  bitboard_t newBitBoard = 1;

  return (newBitBoard << (size * (size - (row + 1)) + (size - (column + 1))));
}

/* This function shift all the discs on the board in the north */
static bitboard_t shift_north(const size_t size, const bitboard_t bitboard) {
  return (bitboard << size);
}

/* This function shift all the discs on the board in the south */
static bitboard_t shift_south(const size_t size, const bitboard_t bitboard) {
  return (bitboard >> size);
}

/* This function shift all the discs on the board in the west */
static bitboard_t shift_west(const size_t size, const bitboard_t bitboard) {
  bitboard_t mask = 0;
  for (size_t row = 0; row < size; row++) {
    mask = mask | set_bitboard(size, row, size - 1);
  }
  return ((bitboard << 1) & ~(mask));
}

/* This function shift all the discs on the board in the east */
static bitboard_t shift_east(const size_t size, const bitboard_t bitboard) {

  bitboard_t mask = 0;
  for (size_t row = 0; row < size; row++) {
    mask = mask | set_bitboard(size, row, 0);
  }
  return ((bitboard >> 1) & ~(mask));
}

/* This function shift all the discs on the board in the north-east */
static bitboard_t shift_ne(const size_t size, const bitboard_t bitboard) {

  bitboard_t board = shift_north(size, bitboard);
  board = shift_east(size, board);

  return board;
}

/* This function shift all the discs on the board in the south-east */
static bitboard_t shift_se(const size_t size, const bitboard_t bitboard) {

  bitboard_t board = shift_south(size, bitboard);
  board = shift_east(size, board);

  return board;
}

/* This function shift all the discs on the board in the north-west */
static bitboard_t shift_nw(const size_t size, const bitboard_t bitboard) {

  bitboard_t board = shift_north(size, bitboard);
  board = shift_west(size, board);

  return board;
}

/* This function shift all the discs on the board in the south-west */
static bitboard_t shift_sw(const size_t size, const bitboard_t bitboard) {

  bitboard_t board = shift_south(size, bitboard);
  board = shift_west(size, board);

  return board;
}

/* Array of direction to move */
static bitboard_t (*All_shift[8])(size_t, bitboard_t) = {
    shift_north, shift_south, shift_east, shift_west,
    shift_ne,    shift_nw,    shift_sw,   shift_se};

/* This function computes all the possibles moves and return bitboard with a
possibles moves marked with '1'. */
bitboard_t compute_moves(const size_t size, const bitboard_t player,
                         const bitboard_t opponent) {

  /*Empty cases*/
  bitboard_t mask = ((bitboard_t)1 << (size * size)) - (bitboard_t)1;
  bitboard_t empty = (~(player | opponent)) & mask;

  /*PossibleMove*/
  bitboard_t moves = 0;

  for (int d = 0; d < 8; d++) {
    bitboard_t playerShift = (All_shift[d](size, player));
    bitboard_t candidates = opponent & (playerShift);
    while (candidates != 0) {
      bitboard_t playerShifted = (All_shift[d](size, candidates));
      moves |= empty & (playerShifted);
      candidates = opponent & (playerShifted);
    }
  }

  return moves;
}

/* This function creates an empty Board with player and size given */
board_t *board_alloc(const size_t size, const disc_t player) {

  if (size % 2 != 0 || size > 10 || size <= 0) {
    return NULL;
  }

  board_t *board = malloc(sizeof(board_t));

  board->size = size;
  board->player = player;

  board->black = 0;
  board->white = 0;
  board->moves = 0;
  board->next_move = 0;

  return board;
}

/* This function free the memory allocated to hold board */
void board_free(board_t *board) { free(board); }

/* This function initialize all squares of the board as a starting game */
board_t *board_init(const size_t size) {

  if (size % 2 != 0 || size > 10 || size <= 0) {
    return NULL;
  }

  board_t *board = board_alloc(size, BLACK_DISC);

  if (board == NULL) {
    fprintf(stderr, "board_init(%ld) == NULL", size);
  }

  bitboard_t w1 =
      set_bitboard(board->size, board->size / 2 - 1, board->size / 2 - 1);

  bitboard_t w2 = set_bitboard(board->size, board->size / 2, board->size / 2);

  bitboard_t w3 =
      set_bitboard(board->size, board->size / 2 - 1, board->size / 2);

  bitboard_t w4 =
      set_bitboard(board->size, board->size / 2, board->size / 2 - 1);

  board->black = w3 | w4;
  board->white = w1 | w2;
  board->moves = compute_moves(size, board->black, board->white);
  board->next_move = 0;

  return board;
}

/* This function perform a deep copy of the board structure */
board_t *board_copy(const board_t *board) {

  if (!board) {
    return NULL;
  }

  board_t *boardCopy = board_alloc(board->size, board->player);

  boardCopy->black = board->black;
  boardCopy->white = board->white;
  boardCopy->moves = board->moves;
  boardCopy->next_move = board->next_move;

  return boardCopy;
}

/* This function return the size of board */
size_t board_size(const board_t *board) {
  if (!board) {
    fprintf(stderr, "illegal parameter!");
    exit(EXIT_FAILURE);
  }
  return board->size;
}

/* This function return the player of board */
disc_t board_player(const board_t *board) {
  if (board->size == 2) {
    return EMPTY_DISC;
  }

  if (!board) {
    return EMPTY_DISC;
  }
  return board->player;
}

/* This function setting the current player of the given board */
void board_set_player(board_t *board, disc_t player) {
  if (!board) {
    fprintf(stderr, "illegal parameter!");
    exit(EXIT_FAILURE);
  }
  board->player = player;
}

/* This function get the content of the square at the given coordinates */
disc_t board_get(const board_t *board, const size_t row, const size_t column) {
  if (!board) {
    return EMPTY_DISC;
  }
  if ((row > board->size) || (column > board->size)) {
    return EMPTY_DISC;
  }

  bitboard_t mask = set_bitboard(board->size, row, column);
  bitboard_t v1 = mask & board->black;
  bitboard_t v2 = mask & board->white;
  bitboard_t v3 = mask & board->moves;

  if (v1 != 0) {
    return BLACK_DISC;
  }

  else if (v2 != 0) {
    return WHITE_DISC;
  }

  else if (v3 != 0) {
    return HINT_DISC;
  }

  else
    return EMPTY_DISC;
}

/* set the given disc at given position */
void board_set(board_t *board, const disc_t disc, const size_t row,
               const size_t collumn) {
  if (board == NULL) {
    return;
  }

  size_t size = board_size(board);
  bitboard_t givendisc = set_bitboard(size, row, collumn);
  bitboard_t playerboard = 0;
  bitboard_t newplayerboard = 0;

  switch (disc) {
  default:
    break;

  case BLACK_DISC:
    playerboard = board->black;
    newplayerboard = playerboard | givendisc;
    board->black = newplayerboard;
    board->white = (board->white) & ~givendisc;

    if (board->player == BLACK_DISC) {
      board->moves = compute_moves(size, board->black, board->white);
    }

    if (board->player == WHITE_DISC) {
      board->moves = compute_moves(size, board->white, board->black);
    }
    break;

  case WHITE_DISC:
    playerboard = board->white;
    newplayerboard = playerboard | givendisc;
    board->white = newplayerboard;
    board->black = (board->black) & ~givendisc;

    if (board->player == BLACK_DISC) {
      board->moves = compute_moves(size, board->black, board->white);
    }

    if (board->player == WHITE_DISC) {
      board->moves = compute_moves(size, board->white, board->black);
    }
    break;

  case HINT_DISC:
    playerboard = board->moves;
    newplayerboard = playerboard | givendisc;
    board->moves = newplayerboard;
    board->white = (board->white) & ~givendisc;
    board->black = (board->black) & ~givendisc;
    break;

  case EMPTY_DISC:
    board->white = (board->white) & ~givendisc;
    board->moves = (board->moves) & ~givendisc;
    board->black = (board->black) & ~givendisc;

    if (board->player == BLACK_DISC) {
      board->moves = compute_moves(size, board->black, board->white);
    }

    if (board->player == WHITE_DISC) {
      board->moves = compute_moves(size, board->white, board->black);
    }
    break;
  }

  return;
}

/* This function calculated number of 1 in bitboard */
static size_t bitboard_popcount(const bitboard_t bitboard) {

  bitboard_t bitBoard1 = bitboard;
  bitboard_t mask1 =
      (((bitboard_t)0x5555555555555555) << 64) | 0x5555555555555555;
  bitboard_t mask2 =
      (((bitboard_t)0x3333333333333333) << 64) | 0x3333333333333333;
  bitboard_t mask3 =
      (((bitboard_t)0x0F0F0F0F0F0F0F0F) << 64) | 0x0F0F0F0F0F0F0F0F;
  bitboard_t mask4 =
      (((bitboard_t)0x0101010101010101) << 64) | 0x0101010101010101;

  bitBoard1 = bitBoard1 - ((bitBoard1 >> 1) & mask1);
  bitBoard1 = (bitBoard1 & mask2) + ((bitBoard1 >> 2) & mask2);

  return ((((bitBoard1 + (bitBoard1 >> 4)) & mask3) * mask4) >> 120);
}

/* This function return the score of the given board */
score_t board_score(const board_t *board) {
  if (!board) {
    fprintf(stderr, "illegal parameter!");
    exit(EXIT_FAILURE);
  }
  size_t bcount = bitboard_popcount(board->black);
  size_t wcount = bitboard_popcount(board->white);

  score_t score;
  score = (score_t){.black = bcount, .white = wcount};

  return score;
}

/* This function calculated number of possibles moves */
size_t board_count_player_moves(board_t *board) {
  return bitboard_popcount(board->moves);
}

/* This function calculated number of possibles moves for opponents*/
size_t board_count_opponent_moves(board_t *board) {
  bitboard_t opponent = 0;
  bitboard_t player = 0;

  if (board_player(board) == BLACK_DISC) {
    opponent = board->white;
    player = board->black;
  }

  else {
    opponent = board->black;
    player = board->white;
  }
  bitboard_t move = compute_moves(board->size, opponent, player);

  return bitboard_popcount(move);
}

/* This function check if a move is valid within given board */
bool board_is_move_valid(const board_t *board, const move_t move) {
  bitboard_t mask = set_bitboard(board->size, move.row, move.column);
  if ((mask & board->moves) != 0) {
    return true;
  }
  return false;
}

/* This function apply a move according according to rules and set the board
 * for next move */
bool board_play(board_t *board, const move_t move) {

  if ((board_is_move_valid(board, move) == false) &&
      (board_count_player_moves(board) != 0)) {
    return false;

  }

  else {

    size_t size = board->size;
    disc_t player = board->player;
    bitboard_t bopponent = 0;
    bitboard_t bplayer = 0;
    disc_t opponent = EMPTY_DISC;
    size_t row = move.row;
    size_t col = move.column;

    /* set disc */
    bitboard_t maskmove = set_bitboard(size, row, col);
    board_set(board, player, row, col);

    if (player == BLACK_DISC) {
      bopponent = board->white;
      bplayer = board->black;
      opponent = WHITE_DISC;
    } else {
      bopponent = board->black;
      bplayer = board->white;
      opponent = BLACK_DISC;
    }

    /* flip all discs */
    bitboard_t accu = 0;
    bitboard_t mask = ((bitboard_t)1 << (size * size)) - (bitboard_t)1;
    bitboard_t empty = (~(bplayer | bopponent)) & mask;

    for (int d = 0; d < 8; d++) {
      bitboard_t moves = 0;
      bitboard_t candidates = All_shift[d](size, maskmove) & bopponent;

      while (candidates != 0) {
        moves |= (bopponent & candidates);
        bitboard_t ve = All_shift[d](size, candidates);
        candidates = bopponent & ve;

        if (ve == 0 || (ve & empty) != 0) {
          moves = 0;
        }
      }
      accu |= moves;
    }

    if (player == BLACK_DISC) {
      board->white = board->white & ~accu;
      board->black = board->black | accu;
    }

    else {
      board->white = board->white | accu;
      board->black = board->black & ~accu;
    }

    /* end of flip */
    if (player == BLACK_DISC) {
      bopponent = board->white;
      bplayer = board->black;
      opponent = WHITE_DISC;
    } else {
      bopponent = board->black;
      bplayer = board->white;
      opponent = BLACK_DISC;
    }

    if (board_count_opponent_moves(board) != 0) {

      board_set_player(board, opponent);
      board->moves = compute_moves(board->size, bopponent, bplayer);
      return true;
    }

    if (board_count_player_moves(board) != 0 &&
        board_count_opponent_moves(board) == 0) {

      board_set_player(board, player);
      board->moves = compute_moves(board->size, bplayer, bopponent);
      return true;
    }

    if (board_count_player_moves(board) == 0 &&
        board_count_opponent_moves(board) == 0) {
      board_set_player(board, EMPTY_DISC);
      return true;
    }
  }

  return true;
}

/* This function store the next possible move to be examined */
move_t board_next_move(board_t *board) {
  move_t move;

  if (board->next_move == 0) {

    bitboard_t bopponent = 0;
    bitboard_t bplayer = 0;
    if (board->player == BLACK_DISC) {
      bopponent = board->white;
      bplayer = board->black;
    } else {
      bopponent = board->black;
      bplayer = board->white;
    }

    board->next_move = compute_moves(board->size, bplayer, bopponent);
  }

  bitboard_t maskmove = board->next_move;

  bitboard_t maskerase =
      set_bitboard(board->size, board->size - 1, board->size - 1);
  size_t count = 0;

  while ((maskmove & maskerase) == 0) {
    count = count + 1;
    maskerase = maskerase << 1;
  }

  board->next_move = maskmove ^ maskerase;
  size_t number = (board->size * board->size) - count;
  size_t col1 = (number % board->size);
  size_t col = col1 - 1;

  size_t row = (number - col1) / board->size;

  move.row = row;
  move.column = col;

  return move;
}

/* Calculate numbers of caracters in file */
int numberofCharacters(FILE *f) {
  int nbc = 0;
  char c = fgetc(f);

  while (c != EOF) {
    nbc++;
    c = fgetc(f);
  }

  return (nbc ? nbc : -1);
}

/* This function  write on the file descriptor fd or created FILE fd and
return the number of printed characters */
int board_print(const board_t *board, FILE *fd) {
  if (!board) {
    return -1;
    fprintf(stderr, "illegal parameter\n");
    exit(EXIT_FAILURE);
  }

  if (!fd) {
    return -1;
    fprintf(stderr, "unable to open file fd\n");
    exit(EXIT_FAILURE);
  }

  board_t *p = board_copy(board);

  fprintf(fd, "%c player's turn.\n", board_player(p));
  fprintf(fd, "\n");
  if (board_size(p) == 10) {
    fprintf(fd, "   ");

    for (size_t i = 0; i < board_size(p); i++) {
      fprintf(fd, "%c ", 'A' + (int)i);
    }

    fprintf(fd, "\n");

    for (size_t i = 0; i < board_size(p); i++) {

      if (i == 9) {
        fprintf(fd, "%lu ", i + 1);
      }

      else {
        fprintf(fd, " %lu ", i + 1);
      }

      for (size_t j = 0; j < board_size(p); j++) {
        fprintf(fd, "%c ", board_get(p, i, j));
      }

      fprintf(fd, "\n");
    }
  }

  else {
    fprintf(fd, "  ");

    for (size_t i = 0; i < board_size(p); i++) {
      fprintf(fd, "%c ", 'A' + (int)i);
    }

    fprintf(fd, "\n");

    for (size_t i = 0; i < board_size(p); i++) {
      fprintf(fd, "%.1lu ", i + 1);

      for (size_t j = 0; j < board_size(p); j++) {
        fprintf(fd, "%c ", board_get(p, i, j));
      }
      fprintf(fd, "\n");
    }
  }

  fprintf(fd,
          "\n"
          "Score: 'X' = %u, 'O' = %u\n",
          board_score(p).black, board_score(p).white);

  free(p);

  return numberofCharacters(fd);
}

/* This function change the player at the beginning of the game.*/
void change_game(board_t *board) {
  disc_t player = board->player;
  disc_t opponent = EMPTY_DISC;

  bitboard_t bplayer = 0;
  bitboard_t bopponent = 0;

  if (player == BLACK_DISC) {
    opponent = WHITE_DISC;
    bplayer = board->black;
    bopponent = board->white;
  }

  if (player == WHITE_DISC) {
    opponent = BLACK_DISC;
    bplayer = board->white;
    bopponent = board->black;
  }

  if ((board_count_player_moves(board) == 0) &&
      (board_count_opponent_moves(board) != 0)) {
    board_set_player(board, opponent);
    board->moves = compute_moves(board->size, bopponent, bplayer);
  }

  if (board_count_player_moves(board) != 0) {
    board->moves = compute_moves(board->size, bplayer, bopponent);
  }

  if ((board_count_player_moves(board) == 0) &&
      (board_count_opponent_moves(board) == 0)) {
    board_set_player(board, EMPTY_DISC);
    board->moves = 0;
  }
}