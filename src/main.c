#include <curses.h>
// #include <locale.h>
// #include <stdlib.h> //noetig fuer atexit()
#include <argp.h>
#include <stdlib.h>
#include <time.h>

#include "ansi.h"
#include "main.h"
#include "int_list.h"

#define COLOR_ORANGE 8

static t_config game_config = {10, 10, 12, 0, 0};

int main(int argc, char **argv)
{
    const struct argp_option options[] = {
        {"absolute", 'a', 0, OPTION_ARG_OPTIONAL, "Instructs the game to interpret BOMB_RATIO as the number of bombs instead of a ratio between bomb count and board size."},
        {"zen", 'z', 0, OPTION_ARG_OPTIONAL, "Time will not be recorded, and after a game is completed, the next one begins a few moments later."},
        {0}
    };

    struct argp parser = {options, parse, "[X-SIZE [Y-SIZE [BOMB_RATIO]]]",
        "Starts a game of minesweeper, in the terminal! Can be played as expected with your mouse cursor.\v"
        MODIFY("X-SIZE", BOLD) " and " MODIFY("Y-SIZE", BOLD) " are the horizontal and vertical size of the board, respectively. "
        MODIFY("BOMB_RATIO", BOLD) " is the ratio of bombs compared to the number of tiles, expressed as a percentage. "
        "For example, with an " MODIFY("X_SIZE", BOLD) " and " MODIFY("Y-SIZE", BOLD) " of 15 (so an area of 225 tiles), "
        "with a " MODIFY("BOMB_RATIO", BOLD) " of 10, the game would contain (225/100)*10=22,5 bombs "
        "(in case of a floating point number, the game will round down, resulting in 22 bombs in this case). "
        "Using ratios is easier in order to better control the difficulty of the game. Should an absolute number of "
        "bombs be required, no matter the size, the " MODIFY("--absolute", ULIN) " option may be passed, instructing the game to interpret "
        "BOMB_RATIO as the number of bombs on the field instead. Ratios between 10 and 35 work best, higher numbers "
        "tend to produce more \"guessing\" scenarios."};

    argp_parse(&parser, argc, argv, 0, 0, &game_config);
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    raw();
    noecho();

    setup_colors();
    ensure_game_size();

    t_tile_state *tile_states = calloc(game_config.sizex * game_config.sizey, sizeof(*tile_states));
    setup_game(tile_states);

    int ch;
    curs_set(1);
    move(0, 0);
    int x = 1;
    int y = 0;
    do {
        ensure_game_size();
        render_game(tile_states);
        move(y, x);
        
        ch = getch();

        switch (ch)
        {
            case KEY_DOWN:
            case KEY_UP:
                ch == KEY_DOWN ? y++ : y--;
                break;
            case KEY_LEFT:
            case KEY_RIGHT:
                ch == KEY_RIGHT ? (x += 2) : (x -= 2);
                break;
            case ' ':
                reveal_tile(tile_states, x/2, y);
                break;
            case 'm':
                mark_tile(tile_states, x/2, y);
                break;
            default:
                break;
        }

        if(x < 0) x = 0;
        if(y < 0) y = 0;

        if(x/2 >= game_config.sizex) x = game_config.sizex*2-1;
        if(y >= game_config.sizey) y = game_config.sizey-1;

    } while (ch != 'x');
    
    //cleanup:
    free(tile_states);
    endwin();
    return 0;
}

void setup_game(t_tile_state *tile_states) {
    srand(time(NULL));
    size_t position_count = game_config.sizex * game_config.sizey;
    int_list_t *possible_bombs = create_list(position_count);

    int bomb_count = game_config.bombs_is_abs ? game_config.bombs : position_count / 100.0 * game_config.bombs;

    for(int i = 0; i < bomb_count; i++) {
        size_t selected = (float)rand() / RAND_MAX * position_count;
        
        tile_states[get_index(possible_bombs, selected)] = 1;
        remove_index(possible_bombs, selected);
        position_count--;
    }
}

error_t parse(int key, char *arg, struct argp_state *state) {
    switch(key) {
        case ARGP_KEY_ARG:
            if(state->arg_num >= 3) {
                fprintf(state->err_stream, "Too many arguments passed, expected up to three!\n");
                return ARGP_ERR_UNKNOWN;
            }

            char* rest;
            int value = strtol(arg, &rest, 10);
            if(*rest != 0) {
                fprintf(state->err_stream, "Failed to parse argument %d, expected number but got \"%s\"\n", state->arg_num, arg);
                return ARGP_ERR_UNKNOWN;
            }

            if(value <= 0) {
                fprintf(state->err_stream, "Failed to parse argument %d, number must be at least 1!\n", state->arg_num);
                return ARGP_ERR_UNKNOWN;
            }

            switch(state->arg_num) {
                case 0:
                    ((t_config*)(state->input))->sizex = value;
                    break;
                case 1:
                    ((t_config*)(state->input))->sizey = value;
                    break;
                case 2:
                    ((t_config*)(state->input))->bombs = value;
                    break;
            }

            return 0;
        case 'a':
            ((t_config*)(state->input))->bombs_is_abs = 1;
            return 0;
        case 'z':
            ((t_config*)(state->input))->is_zen = 1;
            return 0;
    }

    return ARGP_ERR_UNKNOWN;
}

#define TIME_LITERAL  "Time elapsed: XXXs"
#define BOMBS_LITERAL "Bombs remaining: XX"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void ensure_game_size() {
    const int recX = MAX(game_config.sizex*2, MAX(sizeof(TIME_LITERAL), sizeof(BOMBS_LITERAL)));
    const int recY = game_config.sizey+2;

    int currX, currY;
    getmaxyx(stdscr, currY, currX);

    if(currX < recX || currY < recY) {
        clear();
        printw("Your terminal is too small, please increase its size until the game continues...");

        while(currX < recX || currY < recY) {
            getmaxyx(stdscr, currY, currX);
            move(1, 0);
            clrtoeol();
            mvprintw(1, 0, "Current screen size: %d * %d\n", currX, currY);
            printw("Recommended screen size: %d, %d", recX, recY);
            refresh();
        }

        getch();
    }

    flushinp();
}

void setup_colors() {
    if(!has_colors())
        printw("WARNING: Your terminal does not support colors. Colors make the game easier to parse. Try to find a terminal that supports colors.\n");
    else {
        start_color();

        NCURSES_COLOR_T red[3];
        NCURSES_COLOR_T green[3];

        color_content(COLOR_RED, &red[0], &red[1], &red[2]);
        color_content(COLOR_GREEN, &green[0], &green[1], &green[2]);

        init_color(COLOR_ORANGE, 
            red[0] + (NCURSES_COLOR_T)((float)green[0] * 0.5), 
            red[1] + (NCURSES_COLOR_T)((float)green[1] * 0.5), 
            red[2] + (NCURSES_COLOR_T)((float)green[2] * 0.5)
        );

        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(5, COLOR_YELLOW, COLOR_BLACK);
        init_pair(6, COLOR_ORANGE, COLOR_BLACK);
        init_pair(7, COLOR_RED, COLOR_BLACK);
        init_pair(8, COLOR_BLUE, COLOR_BLACK);

        init_pair(10, COLOR_YELLOW, COLOR_WHITE);
    }
}

#define BIT(x) (1 << (x))

#define BIT_SET(x, b) ((x) |= (b))
#define BIT_CLEAR(x, b) ((x) &= ~(b))
#define BIT_TOGGLE(x, b) ((x) ^= (b))

#define IS_BIT_SET(x, b) (((x) & (b)) == (b))

#define BOMB_BIT BIT(0)
#define REVEAL_BIT BIT(1)
#define MARK_BIT BIT(2)

#define IS_BOMB(x) IS_BIT_SET(x, BOMB_BIT)
#define IS_REVEALED(x) IS_BIT_SET(x, REVEAL_BIT)
#define IS_MARKED(x) IS_BIT_SET(x, MARK_BIT)

void render_game(t_tile_state *tile_states) {
    clear();
    curs_set(0);

    for(int y = 0; y < game_config.sizey; y++) {
        move(y, 0);

        for(int x = 0; x < game_config.sizex; x++) {
            size_t index = y * game_config.sizex + x;
            t_tile_state status = tile_states[index];

            if(!IS_REVEALED(status)) {
                if(IS_MARKED(status)) {
                    attrset(COLOR_PAIR(10) | A_BOLD);
                    printw("??");
                }
                else {
                    attrset(A_REVERSE | COLOR_PAIR(0));
                    printw("  ");
                }
            }
            else if(IS_BOMB(status)) {
                attrset(A_REVERSE | COLOR_PAIR(3));
                printw("  ");
            }
            else{
                uint8_t bomb_count = get_bomb_count(tile_states, x, y);
                attrset(COLOR_PAIR(bomb_count));
                printw("%2u", bomb_count);
            }
        }
    }

    standend();
    curs_set(1);
    refresh();
}

uint8_t get_bomb_count(t_tile_state *tile_states, int x, int y) {
    uint8_t result = 0;

    for(int offx = -1; offx <= 1; offx++) {
        for(int offy = -1; offy <= 1; offy++) {
            if(offx == 0 && offy == 0) continue;

            if(x + offx < 0 || x + offx >= game_config.sizex
              || y + offy < 0 || y + offy >= game_config.sizey) {
                continue;
            }

            size_t index = (y+offy)*game_config.sizex + x + offx;

            if(IS_BOMB(tile_states[index])) {
                result++;
            }            
        }
    }

    return result;
}

void reveal_tile(t_tile_state *tile_states, int x, int y) {
    if(x < 0 || x >= game_config.sizex || y < 0 || y >= game_config.sizey) 
        return;

    size_t index = y * game_config.sizex + x;
    
    if(IS_REVEALED(tile_states[index]))
        return;

    BIT_SET(tile_states[index], REVEAL_BIT);
    BIT_CLEAR(tile_states[index], MARK_BIT);
    
    if(get_bomb_count(tile_states, x, y) == 0) {
        reveal_tile(tile_states, x+1, y);
        reveal_tile(tile_states, x-1, y);
        reveal_tile(tile_states, x, y+1);
        reveal_tile(tile_states, x, y-1);
        reveal_tile(tile_states, x+1, y+1);
        reveal_tile(tile_states, x+1, y-1);
        reveal_tile(tile_states, x-1, y+1),
        reveal_tile(tile_states, x-1, y-1);
    }
}

void mark_tile(t_tile_state *tile_states, int x, int y) {
    if(x < 0 || x >= game_config.sizex || y < 0 || y >= game_config.sizey) 
        return;

    size_t index = y * game_config.sizex + x;
    
    if(IS_REVEALED(tile_states[index]))
        return;
    
    BIT_TOGGLE(tile_states[index], MARK_BIT);    
}