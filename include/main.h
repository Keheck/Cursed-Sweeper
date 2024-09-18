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
int reveal_tile(t_tile_state *states, int x, int y);
void mark_tile(t_tile_state *states, int x, int y);

#define BIT(x) (1 << (x))

#define BIT_SET(x, b) ((x) |= (b))
#define BIT_CLEAR(x, b) ((x) &= ~(b))
#define BIT_TOGGLE(x, b) ((x) ^= (b))

#define IS_BIT_SET(x, b) (((x) & (b)) == (b))

#define BOMB_BIT BIT(0)
#define REVEAL_BIT BIT(1)
#define MARK_BIT BIT(2)
#define HOVER_BIT BIT(7)

#define IS_BOMB(x) IS_BIT_SET(x, BOMB_BIT)
#define IS_REVEALED(x) IS_BIT_SET(x, REVEAL_BIT)
#define IS_MARKED(x) IS_BIT_SET(x, MARK_BIT)
#define IS_HOVERED(x) IS_BIT_SET(x, HOVER_BIT)

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif