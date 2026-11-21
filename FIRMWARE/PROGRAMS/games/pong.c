#include "pong.h"

static int paddle_y = 24;
static int prev_paddle_y = 24;

void pong_init() {
  paddle_y = 24;
  prev_paddle_y = 24;
  cleanLayer(LAYER_BG);
  cleanLayer(LAYER_SPRITE);
}

int pong_input(int key) {
  if (key == BTN_B) {
    return -1; // salir de vuelta a GAMES_MENU
  }
  if (key == BTN_UP && paddle_y > 0) {
    prev_paddle_y = paddle_y;
    paddle_y -= 2;
    return 1;
  }
  if (key == BTN_DOWN && paddle_y < 54) {
    prev_paddle_y = paddle_y;
    paddle_y += 2;
    return 1;
  }
  return 0;
}

void pong_render(void) {
  fill_rect(LAYER_SPRITE, 2, prev_paddle_y, 2, 10, 0xF0F);
  fill_rect(LAYER_SPRITE, 2, paddle_y, 2, 10, 0xFFF);
  prev_paddle_y = paddle_y;
}
