#define _USE_MATH_DEFINES
#define dDOUBLE

#include<stdio.h>
#include<GL/glew.h>
#include<GL/glut.h>
#include <GL/freeglut.h>
#include<ode/ode.h>
#include"Global.h"
#include"Util.h"
#include"control.h"
#include"LoadShaders.h"
#include"PlanetState.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<time.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <cstdio>
#include "loadobj.h"
#include "loadtex.h"
#include <drawstuff/drawstuff.h>
#include<windows.h>
#include <ode/common.h>
#include <Mmsystem.h>
#include <GL/freeglut.h>
#include <iostream>
#include "Camera.h"


#define dEpsilon 1E-6

#define UVAR(name, i) glGetUniformLocation(program[i], name)
#define UVARS(name) UVAR(name, shading_mode)


using namespace tinyobj;
using namespace glm;


enum { ORTHOGRAPHIC, PERSPECTIVE };
enum { MODEL_MAZE, MODEL_PLAYER1, MODEL_PLAYER2,NUM_OF_MODELS };
const char* model_files[NUM_OF_MODELS] = {
"models/maze.obj", "models/moon.obj", "models/moon.obj" };
float model_scales[NUM_OF_MODELS] = { 1.5f, 0.3f, 0.3f };
PlanetState mstate[NUM_OF_MODELS];
vector<real_t> vertices[NUM_OF_MODELS];
vector<real_t> normals[NUM_OF_MODELS];
vector<real_t> colors[NUM_OF_MODELS];
vector<vector<size_t>> vertex_map[NUM_OF_MODELS];
vector<vector<size_t>> material_map[NUM_OF_MODELS];
vector<shape_t> shapes[NUM_OF_MODELS];
vector<material_t> materials[NUM_OF_MODELS];
vector<real_t> texcoords[NUM_OF_MODELS];
map<string, size_t> texmap[NUM_OF_MODELS];

GLuint vao[NUM_OF_MODELS][2];//model_idx, shading_mode
GLuint vbo[NUM_OF_MODELS][4];//vertice, normal,  color,texture


static dWorldID ode_world; //simulation world
static dSpaceID ode_space; //collision space
static dJointGroupID ode_contactgroup; //a group of contact joint
static bool pause =  TRUE;




static dGeomID ode_plane_geom; 
//primitives
//static dBodyID ode_sphere_body;//rigid
//static dGeomID ode_sphere_geom;//geometry

// obj
static dBodyID ode_trimesh_body[NUM_OF_MODELS];//rigied
static dGeomID ode_trimesh_geom[NUM_OF_MODELS];//body 
static dTriMeshDataID ode_trimesh_data[NUM_OF_MODELS];//vertex to ode
static std::vector<dTriIndex> ode_trimesh_index[NUM_OF_MODELS];//index to ode


int projection_mode = PERSPECTIVE;



bool is_obj_valid = false;
bool is_tex_valid = false;


int up_down = 0;
int left_right = 0;
int d_left_right = 0;

int r_left_right = 0;
int dr_left_right = 0;

int shading_mode = 0; //original, phong, gourad
int model_num = 0;// grid - object]

bool see_map = false;

using namespace std;
typedef std::vector<GLfloat> GLvec;


GLuint program[2];


void init();
void display();


void bind_buffer(GLint buffer, GLvec& vec, int program, const GLchar* attri_name, GLint attri_size);
void bind_buffer(GLint buffer, int program, const GLchar* attri_name, GLint attri_size);

void toggle_projection();
//void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void wheel(int wheel, int direction, int x, int y);
void passive_motion(int x, int y);
void motion(int x, int y);

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(900, 900);
	glutCreateWindow(argv[0]);
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}


	init();
	toggle_projection();
	glutMouseWheelFunc(wheel);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passive_motion);
	glutMainLoop();

}



void build_program()
{
	ShaderInfo shaders[2][3] = {
		{
			{GL_VERTEX_SHADER, "phong.vert"},
			{GL_FRAGMENT_SHADER , "phong.frag"},
			{GL_NONE, NULL}},
		{
			{GL_VERTEX_SHADER, "gouraud.vert"},
			{GL_FRAGMENT_SHADER , "gouraud.frag"},
			{GL_NONE, NULL}},
	};

	for (int i = 0; i < 2; ++i)
		program[i] = LoadShaders(shaders[i]);


}



void bind_buffer(GLint buffer, GLvec& vec, int program, const GLchar* attri_name, GLint attri_size)
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vec.size(), vec.data(), GL_STATIC_DRAW);
	GLuint location = glGetAttribLocation(program, attri_name);
	glVertexAttribPointer(location, attri_size, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(location);
}
void bind_buffer(GLint buffer, int program, const GLchar* attri_name, GLint attri_size)
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	GLuint location = glGetAttribLocation(program, attri_name);
	glVertexAttribPointer(location, attri_size, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(location);
}
Camera camera;
//cube, sphere, cone, cyinder, torus, composition
void init()
{		
	srand(clock());
	build_program();
	glUseProgram(program[shading_mode]);

	//mstate[MODEL_MOON].orbit_radius = 1.0f;
	
	for (size_t k = 0; k < NUM_OF_MODELS; ++k)
	{
		attrib_t attrib;
		is_obj_valid = load_obj(model_files[k], "models/",
			vertices[k], normals[k], vertex_map[k], material_map[k],
			attrib, shapes[k], materials[k], model_scales[k]);

		colors[k].resize(vertices[k].size());


		glActiveTexture(GL_TEXTURE0);
		is_tex_valid = load_tex("models/", texcoords[k],
			texmap[k], attrib.texcoords, shapes[k], materials[k],
			GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);

	}

	for (size_t k = 0; k < NUM_OF_MODELS; ++k)
	{
		
		for (size_t j = 0; j < 2; j++) {//program num
			
			
			
			if (j == 0) {
				glGenVertexArrays(1, &vao[k][j]);
				glBindVertexArray(vao[k][j]);
				glGenBuffers(4, vbo[k]);
				bind_buffer(vbo[k][0], vertices[k], program[j], "vPosition_mc", 3);
				bind_buffer(vbo[k][1], normals[k], program[j], "vNormal_mc", 3);
				bind_buffer(vbo[k][2], colors[k], program[j], "vColor",3);
				bind_buffer(vbo[k][3], texcoords[k], program[j], "vTexcoord", 2);
			}
			else {
				glBindVertexArray(vao[k][j]);
				bind_buffer(vbo[k][0],  program[j], "vPosition_mc", 3);
				bind_buffer(vbo[k][1],  program[j], "vNormal_mc", 3);
				bind_buffer(vbo[k][2],  program[j], "vColor", 3);
				bind_buffer(vbo[k][3],  program[j], "vTexcoord", 2);
			}

		}
	}
	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);

	dMatrix3 R;
	dMass m;
	dRSetIdentity(R);

	dInitODE2(0);
	ode_world = dWorldCreate();// Make a simulation world.
	ode_space = dSimpleSpaceCreate(0);// Make a collision space.
	ode_contactgroup = dJointGroupCreate(0);// Make a contact group.
	dWorldSetGravity(ode_world, 0, -9.8, 0);// Set gravity.

	// Generate the plane geometry.
	ode_plane_geom = dCreatePlane(ode_space, 0, 1, 0, 0);


	// Generate the rigid body and geometry objects for a trimesh.
	ode_trimesh_body[MODEL_MAZE] = dBodyCreate(ode_world);
	dBodySetPosition(ode_trimesh_body[MODEL_MAZE], 0, 0, 0);
	dBodySetRotation(ode_trimesh_body[MODEL_MAZE], R);
	dBodySetLinearVel(ode_trimesh_body[MODEL_MAZE], 0, 0, 0);
	dBodySetAngularVel(ode_trimesh_body[MODEL_MAZE], 0, 0, 0);
	int n = (int)(vertices[MODEL_MAZE].size() / 3);
	ode_trimesh_index[MODEL_MAZE].resize(n);
	for (int i = 0; i < n; ++i) ode_trimesh_index[MODEL_MAZE][i] = i;
	ode_trimesh_data[MODEL_MAZE] = dGeomTriMeshDataCreate();
	dGeomTriMeshDataBuildSingle(ode_trimesh_data[MODEL_MAZE], vertices[MODEL_MAZE].data(), 3 * sizeof(float), n,
		ode_trimesh_index[MODEL_MAZE].data(), n, 3 * sizeof(dTriIndex));
	ode_trimesh_geom[MODEL_MAZE] = dCreateTriMesh(ode_space, ode_trimesh_data[MODEL_MAZE], 0, 0, 0);
	dGeomSetBody(ode_trimesh_geom[MODEL_MAZE], ode_trimesh_body[MODEL_MAZE]);
	dMassSetTrimeshTotal(&m, 20, ode_trimesh_geom[MODEL_MAZE]);
	dGeomSetPosition(ode_trimesh_geom[MODEL_MAZE], -m.c[0], -m.c[1], -m.c[2]);
	dMassTranslate(&m, -m.c[0], -m.c[1], -m.c[2]);
	dBodySetMass(ode_trimesh_body[MODEL_MAZE], &m);

	// Generate the rigid body and geometry objects for a trimesh.
	ode_trimesh_body[MODEL_PLAYER1] = dBodyCreate(ode_world);
	dBodySetPosition(ode_trimesh_body[MODEL_PLAYER1], 0, 0, 0); //upside down..?
	dBodySetRotation(ode_trimesh_body[MODEL_PLAYER1], R);
	dBodySetLinearVel(ode_trimesh_body[MODEL_PLAYER1], 0, 0, 0);
	dBodySetAngularVel(ode_trimesh_body[MODEL_PLAYER1], 0, 0, 0);
	n = (int)(vertices[MODEL_PLAYER1].size() / 3);
	ode_trimesh_index[MODEL_PLAYER1].resize(n);
	for (int i = 0; i < n; ++i) ode_trimesh_index[MODEL_PLAYER1][i] = i;
	ode_trimesh_data[MODEL_PLAYER1] = dGeomTriMeshDataCreate();
	dGeomTriMeshDataBuildSingle(ode_trimesh_data[MODEL_PLAYER1], vertices[MODEL_PLAYER1].data(), 3 * sizeof(float), n,
		ode_trimesh_index[MODEL_PLAYER1].data(), n, 3 * sizeof(dTriIndex));
	ode_trimesh_geom[MODEL_PLAYER1] = dCreateTriMesh(ode_space, ode_trimesh_data[MODEL_PLAYER1], 0, 0, 0);
	dGeomSetBody(ode_trimesh_geom[MODEL_PLAYER1], ode_trimesh_body[MODEL_PLAYER1]);
	dMassSetTrimeshTotal(&m, 10, ode_trimesh_geom[MODEL_PLAYER1]);
	dGeomSetPosition(ode_trimesh_geom[MODEL_PLAYER1], -m.c[0], -m.c[1], -m.c[2]);
	dMassTranslate(&m, -m.c[0], -m.c[1], -m.c[2]);
	dBodySetMass(ode_trimesh_body[MODEL_PLAYER1], &m);

	
	// Generate the rigid body and geometry objects for a trimesh.
	ode_trimesh_body[MODEL_PLAYER2] = dBodyCreate(ode_world);
	dBodySetPosition(ode_trimesh_body[MODEL_PLAYER2], 1, 0, 0); //upside down..?
	dBodySetRotation(ode_trimesh_body[MODEL_PLAYER2], R);
	dBodySetLinearVel(ode_trimesh_body[MODEL_PLAYER2], 0, 0, 0);
	dBodySetAngularVel(ode_trimesh_body[MODEL_PLAYER2], 0, 0, 0);
	n = (int)(vertices[MODEL_PLAYER2].size() / 3);
	ode_trimesh_index[MODEL_PLAYER2].resize(n);
	for (int i = 0; i < n; ++i) ode_trimesh_index[MODEL_PLAYER2][i] = i;
	ode_trimesh_data[MODEL_PLAYER2] = dGeomTriMeshDataCreate();
	dGeomTriMeshDataBuildSingle(ode_trimesh_data[MODEL_PLAYER2], vertices[MODEL_PLAYER2].data(), 3 * sizeof(float), n,
		ode_trimesh_index[MODEL_PLAYER2].data(), n, 3 * sizeof(dTriIndex));
	ode_trimesh_geom[MODEL_PLAYER2] = dCreateTriMesh(ode_space, ode_trimesh_data[MODEL_PLAYER2], 0, 0, 0);
	dGeomSetBody(ode_trimesh_geom[MODEL_PLAYER2], ode_trimesh_body[MODEL_PLAYER2]);
	dMassSetTrimeshTotal(&m, 20, ode_trimesh_geom[MODEL_PLAYER2]);
	dGeomSetPosition(ode_trimesh_geom[MODEL_PLAYER2], -m.c[0], -m.c[1], -m.c[2]);
	dMassTranslate(&m, -m.c[0], -m.c[1], -m.c[2]);
	dBodySetMass(ode_trimesh_body[MODEL_PLAYER2], &m);
	


}

int button_pressed[3] = { GLUT_UP, GLUT_UP, GLUT_UP };// left, mid, right
int mouse_pos[2] = { 0,0 };//mouse position
vec3 before_mapview = camera.eye;


void mouse(int button, int state, int x, int y) {
	button_pressed[button] = state;
	mouse_pos[0] = x;
	mouse_pos[1] = y;
}


void toggle_ballview(float a1, float a2, float a3) {
	before_mapview = camera.eye;
	camera.center= vec3(a1,a2,a3);
	camera.eye = vec3(a1, a2+0.2, a3);
	camera.projection_mode = projection_mode;

	mat4 V = camera.get_viewing();
	mat4 P = camera.get_projection(projection_mode);

	glUniformMatrix4fv(2, 1, GL_FALSE, value_ptr(V));
	glUniformMatrix4fv(3, 1, GL_FALSE, value_ptr(P));

	glutPostRedisplay();
}

void toggle_mapview(bool see_map) {
	if (see_map) {

		before_mapview = camera.eye;
		camera.eye = vec3(0, 5, 0);
		camera.projection_mode = projection_mode;

		mat4 V = camera.get_viewing();
		mat4 P = camera.get_projection(1);

		glUniformMatrix4fv(2, 1, GL_FALSE, value_ptr(V));
		glUniformMatrix4fv(3, 1, GL_FALSE, value_ptr(P));

		glutPostRedisplay();
	}
	else {
		camera.eye = before_mapview;
		camera.projection_mode = projection_mode;

		mat4 V = camera.get_viewing();
		mat4 P = camera.get_projection(1);

		glUniformMatrix4fv(2, 1, GL_FALSE, value_ptr(V));
		glUniformMatrix4fv(3, 1, GL_FALSE, value_ptr(P));

		glutPostRedisplay();
	}

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
	mat4 P = camera.get_projection(camera.projection_mode);

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


int player =0;
//cube, sphere, cone, cyinder, torus, composition
void keyboard(unsigned char key, int x, int y)
{

	switch (key) {
	case '1':
		shading_mode = 0;//Phong shading mode
		printf("phong shading\n");
		glutPostRedisplay();
		break;

	case '2':
		shading_mode = 1;//gourad shading mode
		printf("gourad shading\n");
		glutPostRedisplay();
		break;
	case 'S':
		printf("toggle on off simulation");
		pause = !pause;
		glutPostRedisplay();

		break;
	case 'O':
		printf("Move teapot origin");
		dBodySetPosition(ode_trimesh_body[MODEL_MAZE], 0, 0.1, 0);
		glutPostRedisplay();

		break;
	case 'M':
		printf("move the ball over the top of the teapot");
		dBodySetPosition(ode_trimesh_body[MODEL_PLAYER1], 0, 0.5, 0);
		glutPostRedisplay();

		break;
	case 'm':
		printf("map view\n");
		see_map = !see_map;
		toggle_mapview(see_map);
		glutPostRedisplay();

		break;

	case 's':
		printf("switch player\n");
		player = !player;
		const dReal* pos = dBodyGetPosition(ode_trimesh_body[player+1]);
		toggle_ballview(pos[0], pos[1], pos[2]);
		//camera.center = vec3(0,0,0.1);
		glutPostRedisplay();
		break;


	}
	

}
	







void special(int key, int x, int y) {
	d_left_right = 0;//TODO
	dr_left_right = 0;//TODO

	switch (key) {
	case GLUT_KEY_HOME:
		r_left_right += 1;//TODO
		dr_left_right = 1;//TODO

		break;
	case GLUT_KEY_END:
		r_left_right -= 1;
		dr_left_right = -1;//TODO

		break;
	case GLUT_KEY_LEFT:
		left_right -= 1;
		d_left_right = -1;//TODO

		break;
	case GLUT_KEY_RIGHT:
		left_right += 1;
		d_left_right = 1;//TODO

		break;
	case GLUT_KEY_UP:
		up_down += 1;
		break;
	case GLUT_KEY_DOWN:
		up_down -= 1;
		break;
	}


}







void draw_obj_model(int model_idx, int color_mode, int object_code = 0)//1 : earth 2 : Moon
{
	glUniform1i(UVARS("ColorMode"), color_mode);
	glUniform1i(UVARS("ObjectCode"), object_code);

	glBindVertexArray(vao[model_idx][shading_mode]);
	using namespace glm;
	using namespace tinyobj;

	auto& _shapes = shapes[model_idx];
	auto& _materials = materials[model_idx];
	auto& _vertex_map = vertex_map[model_idx];
	auto& _material_map = material_map[model_idx];
	auto& _texmap = texmap[model_idx];

	for (size_t i = 0; i < _shapes.size(); ++i)
	{
		for (size_t j = 0; j < _material_map[i].size(); ++j)
		{
			// Set material properties for material group j of shape i.
			int m_id = _material_map[i][j];
			if (m_id < 0) {
				glUniform1f(UVARS("n"), 10.0f);
				glUniform3f(UVARS("Ka"), 0.3f, 0.3f, 0.3f);
				glUniform3f(UVARS("Kd"), 1.0f, 1.0f, 1.0f);
				glUniform3f(UVARS("Ks"), 0.8f, 0.8f, 0.8f);
			}
			else {
				glUniform1f(UVARS("n"), _materials[m_id].shininess);
				glUniform3fv(UVARS("Ka"), 1, _materials[m_id].ambient);
				glUniform3fv(UVARS("Kd"), 1, _materials[m_id].diffuse);
				glUniform3fv(UVARS("Ks"), 1, _materials[m_id].specular);
				auto texitem = _texmap.find(_materials[m_id].diffuse_texname);
				if (texitem != _texmap.end()) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texitem->second);
					glUniform1i(UVARS("sampler"), 0);
				}
			}
				glDrawArrays(GL_TRIANGLES, _vertex_map[i][j], _vertex_map[i][j + 1] - _vertex_map[i][j]);
		}
	}
}




glm::mat4 parallel(double r, double aspect, double n, double f) {
	double l = -r;
	double width = 2 * r;
	double height = width / aspect;
	double t = height / 2;
	double b = -t;
	return glm::ortho(l, r, b, t, n, f);
}

void toggle_projection() {
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	double aspect = 1.0 * width / height;

	mat4 V = lookAt(vec3(0, 0, 5), vec3(0, 0, 0), vec3(0, 1, 0)); //eye, center, upvector
	mat4 P(1.0);


	if (projection_mode == 0) {//orthographic
		P = parallel(1.2, aspect, 0.01, 10.0);
		glPolygonOffset(1, 1);
		printf("Orthographic\n");
	}
	else {
		P = perspective(M_PI / 180.0 * (30.0), aspect, 0.01, 10.0);
		glPolygonOffset(1, 1);
		printf("Perspective\n");

	}
	glUniformMatrix4fv(2, 1, GL_FALSE, value_ptr(V));//vertex shader
	glUniformMatrix4fv(3, 1, GL_FALSE, value_ptr(P));//vertex shader
}

glm::mat4 compute_modelling_transf(dBodyID body, float scalef)
{
	using namespace glm;
	mat4 M(1.0f);
	const dReal* pos = dBodyGetPosition(body);
	const dReal* rot = dBodyGetRotation(body);
	M[3] = vec4(pos[0], pos[1], pos[2], 1.0f);
	for (int i = 0; i < 3; ++i) // for each column
		for (int j = 0; j < 3; ++j) // for each row
			M[i][j] = rot[j * 4 + i];
	M = scale(M, vec3(scalef));
	return M;
}

static void nearCallback(void* data, dGeomID o1, dGeomID o2)
{
	const int N = 100;
	dContact contact[N];
	int n = dCollide(o1, o2, N, &(contact[0].geom), sizeof(dContact));
	if (n > 0)
	{
		for (int i = 0; i < n; i++)
		{
			contact[i].surface.mode = dContactSoftERP | dContactSoftCFM;
			contact[i].surface.mu = 0.8;
			contact[i].surface.soft_erp = 0.9;
			contact[i].surface.soft_cfm = 0.01;
			dJointID c = dJointCreateContact(ode_world, ode_contactgroup, &contact[i]);
			dBodyID body1 = dGeomGetBody(contact[i].geom.g1);
			dBodyID body2 = dGeomGetBody(contact[i].geom.g2);
			dJointAttach(c, body1, body2);
		}
	}
}



void render(int color_mode = 0)
{
	using namespace glm;
	using namespace std;
	using namespace tinyobj;
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (is_obj_valid) {
		for (int i = 0; i < NUM_OF_MODELS; ++i) {
			glBindVertexArray(vao[i][shading_mode]);
			mat4 T(1.0f);
			mat4 M(1.0f);
			M = compute_modelling_transf(ode_trimesh_body[i], model_scales[i]);
			glUniformMatrix4fv(1, 1, GL_FALSE, value_ptr(M));
			/*
			if (i == MODEL_MAZE) {
				M = compute_modelling_transf(ode_trimesh_body[MODEL_MAZE], model_scales[MODEL_MAZE]);
				glUniformMatrix4fv(1, 1, GL_FALSE, value_ptr(M));
				//T = mat4(mstate[MODEL_MAZE].get_transf());

			}
			else {
				M = compute_modelling_transf(ode_trimesh_body[MODEL_PLAYER1], model_scales[MODEL_PLAYER1]);
				glUniformMatrix4fv(1, 1, GL_FALSE, value_ptr(M));
				//T = mat4(mstate[MODEL_MOON].get_transf());
			}
			*/
			glUniformMatrix4fv(UVARS("T"), 1, GL_FALSE, value_ptr(T));
			draw_obj_model(i, color_mode, i + 1);
		}
	}
	if (color_mode != 2) {
		glutSwapBuffers();
	}
}


double dsElapsedTime()
{
	static double prev = 0.0;
	double curr = ::timeGetTime() / 1000.0;
	if (!prev)
		prev = curr;
	double retval = curr - prev;
	prev = curr;
	if (retval > 1.0) retval = 1.0;
	if (retval < dEpsilon) retval = dEpsilon;
	return retval;
}

//cube, sphere, cone, cyinder, torus, composition
void display(){
	double stepsize = 0.005; // 5m ssimulation step size
	double dt = dsElapsedTime();
	int no_of_steps = (int)ceilf(dt / stepsize);
	for (int i = 0; !pause && i < no_of_steps; ++i)
	{
		//printf("work?");
		dSpaceCollide(ode_space, 0, &nearCallback);
		dWorldQuickStep(ode_world, stepsize);
		dJointGroupEmpty(ode_contactgroup); // remove all contact joints
	}

	render();

	glFlush();

	glutPostRedisplay();

}

/*
void mouse(int button, int state, int x, int y)
{
	render(2);
	glFlush();
	if (state == GLUT_UP)
	{
		unsigned char res[4];
		int height = glutGet(GLUT_WINDOW_HEIGHT);
		glReadPixels(x, height - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);
		switch (res[0])
		{
		case 1: printf("The earth is clicked!\n"); break;
		case 2: printf("The moon is clicked!\n"); break;
		}
	}
}

*/