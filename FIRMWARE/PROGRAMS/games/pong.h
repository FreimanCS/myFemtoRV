#pragma once
#include "../include/hub75.h"
#include "../pong_assets.h"

void pong_init(GameMode mode);
int pong_input(int key);
int pong_tick(void);
void pong_render(void);
void pong_mode_menu(void);
void pong_pause(void);
