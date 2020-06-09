#pragma once
#define _USE_MATH_DEFINES
#include <GL/freeglut.h>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<Math.h>
using namespace glm;



struct Camera {
	enum { ORTHOGRAPHIC, PERSPECTIVE };
	glm::vec3 eye;
	glm::vec3 center;
	glm::vec3 up;
	float zoom_factor;
	int projection_mode;
	float z_near;
	float z_far;
	float fovy;
	float x_right;

	Camera() :
		eye(0, 0, 8),
		center(0, 0, 0),
		up(0, 1, 0),
		zoom_factor(1.0f),
		projection_mode(ORTHOGRAPHIC),
		z_near(0.01f),
		z_far(100.0f),
		fovy((float)(M_PI/180.0*(30.0))),
		x_right(1.2f)
	{}
	glm::mat4 parallel(double r, double aspect, double n, double f) {
		double l = -r;
		double width = 2 * r;
		double height = width / aspect;
		double t = height / 2;
		double b = -t;
		return glm::ortho(l, r, b, t, n, f);
	}
	glm::mat4 get_viewing() { return glm::lookAt(eye, center, up); }
	glm::mat4 get_projection(float aspect) {
		glm::mat4 p;
		switch (projection_mode) {
		case ORTHOGRAPHIC:
			p = parallel((double)zoom_factor * x_right, aspect, z_near, z_far);
			break;
		case PERSPECTIVE:
			p = glm::perspective(zoom_factor * fovy, aspect, z_near, z_far);
			break;
		}
		return p;
	}
	

	void tumble(GLfloat dx, GLfloat dy) {
		vec4 disp(eye - center, 1);
		GLfloat alpha = 2.0f;
		mat4 V = get_viewing();
		mat4 Rx = rotate(mat4(), alpha * dy, vec3(transpose(V)[0]));
		mat4 Ry = rotate(mat4(), -alpha * dx, vec3(0, 1, 0));
		mat4 R = Ry * Rx;
		eye = center + vec3(R * disp);
		up = mat3(R) * up;

	}

	void track(GLfloat dx, GLfloat dy) {
		mat4 VT = transpose(get_viewing());
		eye += vec3(dx * VT[0] + (-dy * VT[1]));
		center += vec3(-dx * VT[0] + -dy * VT[1]);
	}

	void zoom(int dir) {
		if (dir > 0)
			zoom_factor *= 0.95f;
		else
			zoom_factor *= 1.05f;

	}

	void dolly(int dir) {
		vec3 disp(eye - center);
		if (dir > 0) {
			eye = center + 0.95f * disp;
		}
		else {
			eye = center + 1.05f * disp;
		}
	}
};



