#include "pong.h"

#define PONG_WIDTH 64
#define PONG_HEIGHT 64
#define PADDLE_WIDTH 2
#define LEFT_PADDLE_HEIGHT 10
#define RIGHT_PADDLE_HEIGHT 9
#define PADDLE_SPEED 2
#define BALL_SIZE 2
#define POINTS_TO_WIN 11
#define MAX_BALL_SPEED 2

#define LEFT_PADDLE_X 3
#define RIGHT_PADDLE_X 59
#define SCORE_Y 2

static int left_y;
static int right_y;
static int old_left_y;
static int old_right_y;
static int ball_x;
static int ball_y;
static int old_ball_x;
static int old_ball_y;
static int ball_dx;
static int ball_dy;
static int ball_speed;
static int rally_hits;
static int left_score;
static int right_score;
static int game_over;
static int needs_full_render;

static int clamp_left_y(int y) {
  if (y < 0)
    return 0;
  if (y > PONG_HEIGHT - LEFT_PADDLE_HEIGHT)
    return PONG_HEIGHT - LEFT_PADDLE_HEIGHT;
  return y;
}

static int clamp_right_y(int y) {
  if (y < 0)
    return 0;
  if (y > PONG_HEIGHT - RIGHT_PADDLE_HEIGHT)
    return PONG_HEIGHT - RIGHT_PADDLE_HEIGHT;
  return y;
}

static void reset_ball(int direction) {
  ball_x = (PONG_WIDTH - BALL_SIZE) / 2;
  ball_y = (PONG_HEIGHT - BALL_SIZE) / 2;
  ball_dx = direction;
  ball_dy = 0;
  ball_speed = 1;
  rally_hits = 0;
}

static void reset_match(void) {
  left_score = 0;
  right_score = 0;
  game_over = 0;
  left_y = (PONG_HEIGHT - LEFT_PADDLE_HEIGHT) / 2;
  right_y = (PONG_HEIGHT - RIGHT_PADDLE_HEIGHT) / 2;
  reset_ball(1);
  old_left_y = left_y;
  old_right_y = right_y;
  old_ball_x = ball_x;
  old_ball_y = ball_y;
  needs_full_render = 1;
}

void pong_init(void) {
  cleanLayer(LAYER_BG);
  cleanLayer(LAYER_SPRITE);
  reset_match();
}

int pong_input(int key) {
  if (key == BTN_B)
    return -1;

  if (game_over && (key == BTN_A || key == BTN_START)) {
    reset_match();
    return 1;
  }

  if (key == BTN_UP) {
    old_left_y = left_y;
    left_y = clamp_left_y(left_y - PADDLE_SPEED);
  } else if (key == BTN_DOWN) {
    old_left_y = left_y;
    left_y = clamp_left_y(left_y + PADDLE_SPEED);
  }
  else
    return 0;

  return 1;
}

static void move_cpu(void) {
  int paddle_center = right_y + RIGHT_PADDLE_HEIGHT / 2;
  int ball_center = ball_y + BALL_SIZE / 2;

  if (paddle_center < ball_center)
    right_y = clamp_right_y(right_y + 1);
  else if (paddle_center > ball_center)
    right_y = clamp_right_y(right_y - 1);
}

static int overlaps_vertical(int y1, int h1, int y2, int h2) {
  return y1 < y2 + h2 && y1 + h1 > y2;
}

static int bounce_angle(int paddle_y, int paddle_height) {
  static const int angle_by_zone[8] = {-2, -1, -1, 0, 0, 1, 1, 2};
  int relative_y = (ball_y + BALL_SIZE / 2) - paddle_y;
  int zone = (relative_y * 8) / paddle_height;

  if (zone < 0)
    zone = 0;
  if (zone > 7)
    zone = 7;
  return angle_by_zone[zone];
}

static void accelerate_rally(void) {
  rally_hits++;
  if (rally_hits % 3 == 0 && ball_speed < MAX_BALL_SPEED)
    ball_speed++;
}

static void score_point(int direction) {
  if (direction < 0)
    left_score++;
  else
    right_score++;

  if (left_score >= POINTS_TO_WIN || right_score >= POINTS_TO_WIN) {
    game_over = 1;
    return;
  }

  reset_ball(direction);
}

void pong_update(void) {
  if (game_over)
    return;

  old_right_y = right_y;
  move_cpu();

  old_ball_x = ball_x;
  old_ball_y = ball_y;

  ball_x += ball_dx * ball_speed;
  ball_y += ball_dy;

  if (ball_y <= 0) {
    ball_y = 0;
    ball_dy = 1;
  } else if (ball_y >= PONG_HEIGHT - BALL_SIZE) {
    ball_y = PONG_HEIGHT - BALL_SIZE;
    ball_dy = -1;
  }

  if (ball_dx < 0 && ball_x <= LEFT_PADDLE_X + PADDLE_WIDTH &&
      ball_x + BALL_SIZE >= LEFT_PADDLE_X &&
      overlaps_vertical(ball_y, BALL_SIZE, left_y, LEFT_PADDLE_HEIGHT)) {
    ball_x = LEFT_PADDLE_X + PADDLE_WIDTH;
    ball_dx = 1;
    ball_dy = bounce_angle(left_y, LEFT_PADDLE_HEIGHT);
    accelerate_rally();
  }

  if (ball_dx > 0 && ball_x + BALL_SIZE >= RIGHT_PADDLE_X &&
      ball_x <= RIGHT_PADDLE_X + PADDLE_WIDTH &&
      overlaps_vertical(ball_y, BALL_SIZE, right_y, RIGHT_PADDLE_HEIGHT)) {
    ball_x = RIGHT_PADDLE_X - BALL_SIZE;
    ball_dx = -1;
    ball_dy = bounce_angle(right_y, RIGHT_PADDLE_HEIGHT);
    accelerate_rally();
  }

  if (ball_x < -BALL_SIZE)
    score_point(1);
  else if (ball_x >= PONG_WIDTH)
    score_point(-1);
}

static void draw_score(void) {
  int i;

  fill_rect(LAYER_SPRITE, 8, SCORE_Y, 16, 4, TRANSPARENT);
  fill_rect(LAYER_SPRITE, 40, SCORE_Y, 16, 4, TRANSPARENT);

  for (i = 0; i < left_score; i++)
    fill_rect(LAYER_SPRITE, 8 + i * 2, SCORE_Y, 1, 2, COLOR_CYAN);
  for (i = 0; i < right_score; i++)
    fill_rect(LAYER_SPRITE, 40 + i * 2, SCORE_Y, 1, 2, COLOR_RED);
}

void pong_render(void) {
  if (needs_full_render) {
    cleanLayer(LAYER_BG);
    cleanLayer(LAYER_SPRITE);
    draw_background(pong_scenario);
    needs_full_render = 0;
  } else {
    fill_rect(LAYER_SPRITE, LEFT_PADDLE_X, old_left_y,
              PADDLE_WIDTH, LEFT_PADDLE_HEIGHT, TRANSPARENT);
    fill_rect(LAYER_SPRITE, RIGHT_PADDLE_X, old_right_y,
              PADDLE_WIDTH, RIGHT_PADDLE_HEIGHT, TRANSPARENT);
    fill_rect(LAYER_SPRITE, old_ball_x, old_ball_y,
              BALL_SIZE, BALL_SIZE, TRANSPARENT);
  }

  draw_asset(LAYER_SPRITE, LEFT_PADDLE_X, left_y,
             PADDLE_WIDTH, LEFT_PADDLE_HEIGHT, pong_player1);
  draw_asset(LAYER_SPRITE, RIGHT_PADDLE_X, right_y,
             PADDLE_WIDTH, RIGHT_PADDLE_HEIGHT, pong_player2);
  draw_asset(LAYER_SPRITE, ball_x, ball_y,
             BALL_SIZE, BALL_SIZE, pong_ball);
  draw_score();
}
