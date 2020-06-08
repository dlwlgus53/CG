#include <iostream>
#include<GL/glew.h>
#include <GL/freeglut.h>
#include "Camera.h"
extern int projection_mode;
Camera camera;

int button_pressed[3] = { GLUT_UP, GLUT_UP, GLUT_UP };// left, mid, right
int mouse_pos[2] = { 0,0 };//mouse position

						   
void mouse(int button, int state, int x, int y) {
	button_pressed[button] = state;
	mouse_pos[0] = x;
	mouse_pos[1] = y;
}







void wheel(int wheel, int direction, int x, int y) {

	int modifiers = glutGetModifiers(); //alt shift ctrl
	int is_alt_active = modifiers & GLUT_ACTIVE_ALT;
	int is_ctrl_active = modifiers & GLUT_ACTIVE_CTRL;
	int is_shift_active = modifiers & GLUT_ACTIVE_SHIFT;

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);

	GLfloat dx = 1.f * (x - mouse_pos[0]) / w;
	GLfloat dy = -1.f * (y - mouse_pos[0]) / h;

	if (is_alt_active)
		camera.dolly(direction);
	else
		camera.zoom(direction);


	camera.projection_mode = projection_mode;

	mat4 V = camera.get_viewing();
	mat4 P = camera.get_projection(1);

	glUniformMatrix4fv(2, 1, GL_FALSE, value_ptr(V));
	glUniformMatrix4fv(3, 1, GL_FALSE, value_ptr(P));
	glutPostRedisplay();

}

void passive_motion(int x, int y) {

}

void motion(int x, int y) {
	int modifiers = glutGetModifiers(); //alt shift ctrl
	int is_alt_active = modifiers & GLUT_ACTIVE_ALT;
	int is_ctrl_active = modifiers & GLUT_ACTIVE_CTRL;
	int is_shift_active = modifiers & GLUT_ACTIVE_SHIFT;

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);

	GLfloat dx = 1.f * (x - mouse_pos[0]) / w;
	GLfloat dy = -1.f * (y - mouse_pos[1]) / h;

	///add code
	if (!button_pressed[0] && is_alt_active) {
		camera.tumble(dx, dy);
	}
	else if (!button_pressed[1] && is_alt_active) {
		camera.track(dx, dy);
	}


	mouse_pos[0] = x;
	mouse_pos[1] = y;
	camera.projection_mode = projection_mode;

	mat4 V = camera.get_viewing();
	mat4 P = camera.get_projection(1);

	glUniformMatrix4fv(2, 1, GL_FALSE, value_ptr(V));
	glUniformMatrix4fv(3, 1, GL_FALSE, value_ptr(P));


	glutPostRedisplay();

}
