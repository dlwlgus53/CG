#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<GL/glew.h>
#include<GL/glut.h>
struct PlanetState
{
	GLfloat theta;
	GLfloat orbit_theta;
	GLfloat orbit_radius;
	PlanetState() :
		theta(0),
		orbit_theta(0),
		orbit_radius(0) {}
	glm::mat4 get_transf()
	{
		using namespace glm;
		mat4 M(1.0f);
		M = rotate(M, orbit_theta,
		vec3(0.f, 1.f, 0.f));
		M = translate(M, vec3(orbit_radius, 0, 0));
		M = rotate(M, theta, vec3(0.f, 1.f, 0.f));
		return M;
	}
};