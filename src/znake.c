#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/time.h>
#include "znake.h"


void znake_init(struct znake_t *znake, int x, int y, int max_size)
{
    znake->body = malloc(sizeof(struct vector2d *) * max_size);
    znake->body_length = 0;
    znake->max_size = max_size;
    znake_reset(znake, x, y);
}

void znake_reset(struct znake_t *znake, int x, int y)
{
    znake->head.x=x;
    znake->head.y=y;
    znake->tail.x=x;
    znake->tail.y=y+1;
    znake_clear(znake);
    znake->direction = DIR_NONE;
}

void znake_update(struct znake_t *znake)
{
    if(znake->direction != DIR_NONE) {
        if(znake->body_length == 0) {
            znake->tail.x = znake->head.x;
            znake->tail.y = znake->head.y;
        } else {
            znake->tail.x = znake->body[znake->body_length-1]->x;
            znake->tail.y = znake->body[znake->body_length-1]->y;
            int i;
            for(i = znake->body_length-1; i > 0; i--) {
                znake->body[i]->x = znake->body[i-1]->x;
                znake->body[i]->y = znake->body[i-1]->y;
            }
            znake->body[0]->x = znake->head.x;
            znake->body[0]->y = znake->head.y;
        }
    }


    switch(znake->direction)
    {
    case DIR_UP:
        znake->head.y--;
        break;
    case DIR_DOWN:
        znake->head.y++;
        break;
    case DIR_RIGHT:
        znake->head.x++;
        break;
    case DIR_LEFT:
        znake->head.x--;
        break;
    default:
        break;
    }
}


void znake_add_body(struct znake_t *znake)
{
    if(znake->body_length < znake->max_size) {
        znake->body[znake->body_length] = malloc(sizeof(struct vector2d));
        znake->body[znake->body_length]->x = znake->tail.x;
        znake->body[znake->body_length]->y = znake->tail.y;
        znake->body_length++;
    }
}

int znake_self_collision(struct znake_t *znake)
{
    if(znake->body_length == 0) {
        return 0;
    } else {
        if((znake->head.x == znake->tail.x) && (znake->head.y == znake->tail.y)) {
            return 1;
        }
        int i;
        for(i = 0; i < znake->body_length; i++) {
            if((znake->head.x == znake->body[i]->x) && (znake->head.y == znake->body[i]->y)) {
                return 1;
            }
        }
    }
    return 0;
}

int  znake_head_collision(struct znake_t *znake, int x, int y)
{
    if((znake->head.x == x) && (znake->head.y == y)) {
        return 1;
    }
    return 0;
}

int  znake_collisions_with(struct znake_t *znake, int x, int y)
{
    if((znake->head.x == x) && (znake->head.y == y)) {
        return 1;
    }
    if((znake->tail.x == x) && (znake->tail.y == y)) {
        return 1;
    }
    int i;
    for(i = 0; i < znake->body_length; i++) {
        if((znake->head.x == x) && (znake->head.y == y)) {
            return 1;
        }
    }
    return 0;
}

void znake_clear(struct znake_t *znake)
{
    if(znake->body_length > 0) {
        int i;
        for(i = 0; i < znake->body_length; i++) {
            free(znake->body[i]);
        }
        znake->body_length = 0;
    }
}

void znake_kill(struct znake_t *znake)
{
    znake_clear(znake);
    free(znake->body);
}



static void map_draw_znake(struct map_t *map)
{
    struct znake_t *znake = &map->znake;
    if(znake->body_length > 0) {
        int i;
        for(i = 0; i < znake->body_length; i++) {
            map_mvaddch(map, MAP_EDGE_WIDTH + znake->body[i]->x, MAP_EDGE_HEIGHT + znake->body[i]->y, 'O');
        }
    }

    map_mvaddch(map, MAP_EDGE_WIDTH + znake->tail.x, MAP_EDGE_HEIGHT + znake->tail.y, 'o');
    map_mvaddch(map, MAP_EDGE_WIDTH + znake->head.x, MAP_EDGE_HEIGHT + znake->head.y, '+');
}

static void map_draw_food(struct map_t *map)
{
    map_mvaddch(map, MAP_EDGE_WIDTH + map->food.x, MAP_EDGE_HEIGHT + map->food.y, '#');
}


static void map_change_food_pos(struct map_t *map)
{
    int x = randnum(0, map->size.x-1);
    int y = randnum(0, map->size.y-1);
    while(znake_collisions_with(&map->znake, x, y)) {
        x = randnum(0, map->size.x-1);
        y = randnum(0, map->size.y-1);
    }
    map->food.x = x;
    map->food.y = y;
}

void map_init(struct map_t *map, int x, int y, int w, int h)
{
    map->position.x = x;
    map->position.y = y;
    map->size.x = w;
    map->size.y = h;
    map->half_size.x = w/2;
    map->half_size.y = h/2;
    map->win = initscr();
    noecho();
    cbreak();
    nodelay(map->win, TRUE);
    keypad(map->win, TRUE);
    curs_set(0);
    znake_init(&map->znake, map->half_size.x, map->half_size.y, w*h);
    map_reset(map);
}

void map_reset(struct map_t *map)
{
    znake_reset(&map->znake, map->half_size.x, map->half_size.y);
    map_change_food_pos(map);
    map->running = 1;
    map->score = 0;
    map->speed_delay = DEFAULT_SPEED_DELAY;
    gettimeofday(&map->current_time, NULL);
    gettimeofday(&map->old_time, NULL);
}

void map_lose(struct map_t *map)
{
    map_change_food_pos(map);
}

static int map_snake_edges_collision(struct map_t *map)
{
    struct znake_t *znake = &map->znake;
    /*if((znake->head.x < 0) || (znake->head.y < 0)  ||
       (znake->head.x >= map->size.x) || (znake->head.y >= map->size.y)) {
            return 1;
    }*/

    if(znake->head.x < 0) {
        znake->head.x = map->size.x-1;
    }
    if(znake->head.x >= map->size.x) {
        znake->head.x = 0;
    }
    if(znake->head.y < 0) {
        znake->head.y = map->size.y-1;
    }
    if(znake->head.y >= map->size.y) {
        znake->head.y = 0;
    }
    return 0;
}

void map_update(struct map_t *map)
{
    map_read_input(map);
    map_handle_input(map);

    gettimeofday(&map->current_time, NULL);
    timersub(&map->current_time, &map->old_time, &map->delta_time);
    if(timeval_to_msec(map->delta_time) >= map->speed_delay) {
        if(znake_self_collision(&map->znake) || map_snake_edges_collision(map) ) {
            map_lose(map);
            map_reset(map);
            redrawwin(map->win);
        } else {
            znake_update(&map->znake);
            if(znake_head_collision(&map->znake, map->food.x, map->food.y)) {
                map->score++;
                znake_add_body(&map->znake);
                map_change_food_pos(map);
            }
            gettimeofday(&map->old_time, NULL);
        }
        map->speed_delay -= 0.75f;
    }
    usleep(10*1000);
}

static void map_draw_edges(struct map_t *map)
{
    int i, j;
    wmove(map->win, 1, 0);
    for(j = 0; j <= (map->size.x+1); j++) wprintw(map->win, "_");
    for(i = 0; i < map->size.y; i++) {
        map_mvprintf(map, 0, i+MAP_EDGE_HEIGHT, "|");
        map_mvprintf(map, map->size.x+MAP_EDGE_WIDTH, i+MAP_EDGE_HEIGHT,"|");
    }
    wmove(map->win, map->size.y+MAP_EDGE_HEIGHT, 0);
    for(j = 0; j <= (map->size.x+1); j++) wprintw(map->win, "¯");
}

void map_draw(struct map_t *map)
{
    map_mvprintf(map, 0, 0, "Znake by xerpi       %i", map->score);
    map_draw_edges(map);
    map_draw_food(map);
    map_draw_znake(map);
}

void map_read_input(struct map_t *map)
{
    map->key_pressed = wgetch(map->win);
}

void map_handle_input(struct map_t *map)
{
    switch(map->key_pressed)
    {
    case 'a':
        znake_add_body(&map->znake);
        break;
    case 'q':
        map->running = 0;
        break;
    case 'r':
        map_reset(map);
        break;
    case KEY_UP:
        map->znake.direction = DIR_UP;
        break;
    case KEY_DOWN:
        map->znake.direction = DIR_DOWN;
        break;
    case KEY_RIGHT:
        map->znake.direction = DIR_RIGHT;
        break;
    case KEY_LEFT:
        map->znake.direction = DIR_LEFT;
        break;
    default:
        break;
    }
}

void map_finish(struct map_t *map)
{
    znake_kill(&map->znake);
    delwin(map->win);
}


