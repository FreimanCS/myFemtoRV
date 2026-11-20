#include "assets.h"
#include <keyboard.h>
#include "include/hub75.h"

#define SIZE 5
#define SPEED 2

int x = 10, y = 10;
int old_x = 10, old_y = 10;

void update_sprite(void) {
    int key = UART_pollkey();

    old_x = x;
    old_y = y;

    switch (key) {
    case 'a': x -= SPEED; break;
    case 'd': x += SPEED; break;
    case 'w': y -= SPEED; break;
    case 's': y += SPEED; break;
    }

    if (x < 0) x = 0;
    if (x > 64 - SIZE) x = 64 - SIZE;
    if (y < 0) y = 0;
    if (y > 64 - SIZE) y = 64 - SIZE;
}

int main(void) {
    fill_rect(LAYER_SPRITE, x, y, SIZE, SIZE, COLOR_RED);

    for (;;) {
        update_sprite();

        fill_rect(LAYER_SPRITE, old_x, old_y, SIZE, SIZE, COLOR_BLACK);
        fill_rect(LAYER_SPRITE, x, y, SIZE, SIZE, COLOR_RED);
    }
}
