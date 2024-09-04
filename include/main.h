#ifndef MAIN_H
#define MAIN_H

#include <argp.h>
#include <stdint.h>

struct config {
    int sizex;
    int sizey;
    int bombs;
    char bombs_is_abs;
    char is_zen;
};

typedef struct config t_config;
typedef uint8_t t_tile_state;

error_t parse(int key, char *arg, struct argp_state *state);
void render_game(t_tile_state *tile_states);
void ensure_game_size();
void setup_colors();
void setup_game(t_tile_state *tile_states);
uint8_t get_bomb_count(t_tile_state *tile_states, int x, int y);
void reveal_tile(t_tile_state *states, int x, int y);

#endif