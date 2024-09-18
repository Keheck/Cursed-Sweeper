# Cursed Sweeper

Cursed Sweeper is a CLI implementation of the well known Minesweeper game. It
is launched and played entirely from the command line/terminal. For now, this
game can only be played on Linux. I don't know if I will ever make a release
for Windows, maybe in a separate repository.

## How to play

To play the game, you first need to have the `ncurses` library installed. If
you're on Debian or its descendants, use apt with `#apt install libncurses6`
to install the library. Afterwards, you should be good to go.

To start the game, go to `[TBA]` and type `./cursed-sweeper`. This will start
a game with default configuration: a 15x15 grid with 10% of the tiles being
bombs. The green tile is the one you've currently selected, which will always
be in the upper left corner.

You can move the cursor around with the arrow keys. To reveal a tile, press
Space. If there's no bomb under it, a number will appear which tells you
the number of bombs on the surrounding 8 tiles. Should none of these tiles
contain a bomb, they will be revealed as well, possibly causing a chain
reaction of safe tiles being revealed.

In order to flag/mark a tile as possibly containing a bomb, press the `m` key.
You will see the number of remaining bombs at the bottom of the screen go down.
This is to help you keep track of the amount of tiles you should flag before
having marked more tiles than there are bombs. Marked tiles are indicated by
two yellow question marks, and marked tiles cannot be revealed, preventing you
from accidentally losing your game.

The game will go on like this - revealing and marking tiles - until either you
mess up and dig up a bomb (turning your cursor red instead of green and ending
the game) or you reveal all tiles except those containing a bomb. In either
case, the game will reveal the time it took you to play, and prompt you to
press `q` to return to the console. (You can also press `q` at any time during
a game to quit instantly)

## Customization

You don't have to play with the default settings. You can customize the game size
and the amount of bombs in play by adding command line arguments after the game's name.
The first two arguments are numbers that give the horizontal and vertical size of
the board, respectively, and the third one will give the amount of tiles containing
bombs, as a percentage rounded down. (For example, a board size of `15x15=225` and
a bomb count of 15 will convert 15% of the 225 tiles into bombs, resulting in 33 total
bombs (`225*15% = 225*0.15 = 33.75`, round down to `33`)). Alternatively, you can use the
`--absolute` option to have the bomb count be the absolute number of bombs, independent of
the board size. In that case, a bomb count of `20` will result in 20 bombs being placed,
be it on a `10x10` or `20x20` board.

## Build from source

In order to build the game yourself, you will also need to have a C-compiler installed
(like GCC) and the developer files of the ncurses library (called `libncursesx-dev`,
where x is whatever version you installed the curses binaries on), as well as the
build tool `make` (or build it by hand if you prefer to suffer). This will create
the release version of the game in `./build/bin/`

## Missing features

* [ ] An actual victory screen
* [ ] Implementing the zen mode
* [ ] Make the engine cleaner
* [ ] Add a nice TUI for customizing the game
* [ ] What else?