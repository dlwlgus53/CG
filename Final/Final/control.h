#pragma once
#include <GL/freeglut.h>


void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void passive_motion(int x, int y);
void wheel(int wheel, int direction, int x, int y);

void toggle_mapview(bool map);