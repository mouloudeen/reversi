#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"

/* A player function 'move_t (*player_func) (board_t *)' returns a
* chosen move depending on the given board. */

move_t human_player (board_t *board);
move_t random_player(board_t *board);



#endif /* PLAYER_H */