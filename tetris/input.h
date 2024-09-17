#pragma once

void down();
void left();
void right();
// spin the falling piece left or right, if possible
void spin(int dir);
// move the falling piece as far down as it will go
void hard();
// hold a piece for later
void hold();
void joy_setup();
// set current player to match an input device
void set_player_from_device(int device);
// figure out which "device" from key pressed, i.e. WASD or Arrow keys
int device_from_key();
int menu_input(int key_or_button);
int assign(int device);
// handle a key press from a player
int key_down();
void key_up();
int joy_down();
void joy_up();
