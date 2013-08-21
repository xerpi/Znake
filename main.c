#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include "znake.h"

int main(int argc, char *argv[])
{
    srand (time(NULL));
    struct map_t map;
    map_init(&map, 0, 0, 21, 21);

    while(map.running) {
        map_update(&map);
        werase(map.win);
        map_draw(&map);
        wrefresh(map.win);
    }

    map_finish(&map);
    endwin();
    return 0;
}
