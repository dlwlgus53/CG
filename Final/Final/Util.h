#pragma once


#include<GL/glew.h>
#include <vector>

typedef std::vector<GLfloat> GLvec;

void get_cube_3d(std::vector<GLfloat>& p, std::vector<GLfloat>& normal, GLfloat lx, GLfloat ly, GLfloat lz);
//get cone 
void get_cone_3d(std::vector<GLfloat>& p, std::vector<GLfloat>& normal,
	std::vector<size_t>& side_idx, std::vector<size_t>& bottom_idx, GLfloat radius,
	GLfloat height, GLint n);

void get_sphere_3d(GLvec& p, GLvec& normal, GLfloat r, GLint subh, GLint suba);

void get_cylinder_3d(std::vector<GLfloat>& p, std::vector<GLfloat>& normal, std::vector<size_t>& side_idx,
	std::vector<size_t>& top_idx, std::vector<size_t>& bottom_idx, GLfloat radius,
	GLfloat height, GLint n);

void get_torus_3d(std::vector<GLfloat>& p, std::vector<GLfloat>& normal, std::vector<std::vector<size_t>>& side_idx,
	GLfloat r0, GLfloat r1, GLint na, GLint nh);




void get_grid(std::vector<GLfloat>& p, GLfloat w, GLfloat h, int m, int n);

void bind_buffer(GLint buffer, GLvec& vec, int program,
	const GLchar* attri_name, GLint attri_size);


void bind_buffer(GLint buffer, std::vector<GLuint>& vec, int program);

void get_color_3d_by_pos(GLvec& c, GLvec& p, GLfloat offset);

