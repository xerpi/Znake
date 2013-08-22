#ifndef _ZNAKE_H_
#define _ZNAKE_H_

#include <sys/time.h>

#define randnum(min,max) (rand()%(max-min + 1) + min)
#define timeval_to_msec(t) ((t).tv_sec*1000 + (t).tv_usec/1000)

#define MAP_EDGE_WIDTH 1
#define MAP_EDGE_HEIGHT 2
#define DEFAULT_SPEED_DELAY 300.0f

enum znake_dir_t {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_RIGHT,
    DIR_LEFT
};

struct vector2d {
    int x;
    int y;
};


struct znake_t {
    struct vector2d  head;
    struct vector2d  **body;
    struct vector2d  tail;
    enum znake_dir_t direction;
    int body_length, max_size;
};


struct map_t {
    WINDOW *win;
    struct znake_t  znake;
    struct vector2d position;
    struct vector2d size;
    struct vector2d half_size;
    struct vector2d food;
    int key_pressed, running, score;
    struct timeval current_time, old_time, delta_time;
    float speed_delay;
};

#define map_mvprintf(map, x, y, ...) \
    mvwprintw(map->win, y, x, ##__VA_ARGS__)

#define map_mvaddch(map, x, y, c) \
    mvwaddch(map->win, y, x, c)


void znake_init(struct znake_t *znake, int x, int y, int max_size);
void znake_reset(struct znake_t *znake, int x, int y);
void znake_update(struct znake_t *znake);
void znake_add_body(struct znake_t *znake);
int  znake_self_collision(struct znake_t *znake);
int  znake_head_collision(struct znake_t *znake, int x, int y);
int  znake_collisions_with(struct znake_t *znake, int x, int y);
void znake_clear(struct znake_t *znake);
void znake_kill(struct znake_t *znake);

void map_init(struct map_t *map, int x, int y, int w, int h);
void map_reset(struct map_t *map);
void map_update(struct map_t *map);
void map_draw(struct map_t *map);
void map_read_input(struct map_t *map);
void map_handle_input(struct map_t *map);
void map_finish(struct map_t *map);
void map_lose(struct map_t *map);

#endif
