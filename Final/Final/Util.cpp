#define _USE_MATH_DEFINES
#include<GL/glew.h>
#include <vector>
#include <math.h>
#include <fstream>

typedef std::vector<GLfloat> GLvec;
using namespace std;

#define FPUSH_VTX3(p, vx, vy,vz)\
do{ \
	p.push_back((float)vx);\
	p.push_back((float)vy);\
	p.push_back((float)vz);\
}while(0)

#define FPUSH_VTX3_AT(p, i, vx,vy,vz)\
do{\
	size_t i3 = 3*(i);\
	p[i3+0] = (float)(vx); \
	p[i3+1] = (float)(vy); \
	p[i3+2] = (float)(vz); \
}while(0)

//들어온걸 float 로 만들어서 vx,vy, vz에 저장
#define FSET_VTX3(vx,vy,vz, valx, valy, valz)\
do{\
	vx = (float)(valx);\
	vy = (float)(valy);\
	vz = (float)(valz);\
}while(0)



void get_cube_3d(std::vector<GLfloat> & p, std::vector<GLfloat> & normals, GLfloat lx, GLfloat ly, GLfloat lz)
{
	static const GLfloat box_vertices[] = {
		0.5f, 0.5f, -0.5f,	-0.5f, -0.5f, -0.5f,	-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,	0.5f, -0.5f, -0.5f,	-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,	-0.5f, -0.5f, 0.5f,	-0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,	-0.5f, 0.5f, 0.5f,	-0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,	-0.5f, -0.5f, -0.5f,	0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,	-0.5f, -0.5f, 0.5f,	-0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, 0.5f,	-0.5f, -0.5f, 0.5f,	0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,	-0.5f, 0.5f, 0.5f,	0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,	0.5f, -0.5f, -0.5f,	0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,	0.5f, 0.5f, 0.5f,	0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,	0.5f, 0.5f, -0.5f,	-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,	-0.5f, 0.5f, -0.5f,	-0.5f, 0.5f, 0.5f,
	};
	p.resize(sizeof(box_vertices) / sizeof(GLfloat));
	memcpy(p.data(), box_vertices, sizeof(box_vertices));
	size_t n = p.size() / 3;

	//크기조정
	for (int i = 0; i < n; ++i) {
		p[3 * i + 0] *= lx;
		p[3 * i + 1] *= ly;
		p[3 * i + 2] *= lz;

	}


	//compute normals

	normals.resize(n * 3);
	float* cursor = normals.data();

	//For vertices on the side z = -0.5
	for (int i = 0; i < 6; i++, cursor += 3) { cursor[0] = 0; cursor[1] = 0; cursor[2] = -1; }
	// For vertices on the side at x = -0.5
	for (int i = 0; i < 6; i++, cursor += 3) { cursor[0] = -1; cursor[1] = 0; cursor[2] = 0; }
	// For vertices on the side at y = -0.5
	for (int i = 0; i < 6; i++, cursor += 3) { cursor[0] = 0; cursor[1] = -1; cursor[2] = 0; }
	// For vertices on the side at z = -0.5
	for (int i = 0; i < 6; i++, cursor += 3) { cursor[0] = 0; cursor[1] = 0; cursor[2] = 1; }
	// For vertices on the side at x = -0.5
	for (int i = 0; i < 6; i++, cursor += 3) { cursor[0] = 1; cursor[1] = 0; cursor[2] = 0; }
	// For vertices on the side at y = -0.5
	for (int i = 0; i < 6; i++, cursor += 3) { cursor[0] = 0; cursor[1] = 1; cursor[2] = 0; }

	};

//get cone 
/*
void get_cone_3d(std::vector<GLfloat>& p, std::vector<GLfloat>& normal,
	std::vector<size_t>& side_idx, std::vector<size_t>& bottom_idx, GLfloat radius,
	GLfloat height, GLint n) {

	GLfloat half_height = height / 2;
	GLfloat theta, x, z;
	FPUSH_VTX3(p, 0, half_height, 0);//top vertex
	FPUSH_VTX3(normal, 0, 1, 0);//top vertex

	side_idx.push_back(0);
	for (int i = 0; i <= n; ++i) {
		theta = (GLfloat)(2.0 * M_PI * i / n);
		x = radius * sin(theta);
		z = radius * cos(theta);
		FPUSH_VTX3(p, x, -half_height, z);
		FPUSH_VTX3(normal, x/radius, 0, z/radius);//top vertex

		side_idx.push_back(i + 1);
		bottom_idx.push_back(n + 2 - i);
	}
	FPUSH_VTX3(p, 0, -half_height, 0);// bottom-center vertex
	FPUSH_VTX3(normal, 0, -1, 0);//top vertex
	bottom_idx.push_back(1);
};
*/ 


//get cone 
void get_cone_3d(std::vector<GLfloat>& p, std::vector<GLfloat>& normal,
	std::vector<size_t>& side_idx, std::vector<size_t>& bottom_idx, GLfloat radius,
	GLfloat height, GLint n) {

	GLfloat half_height = height / 2;
	GLfloat theta, x, z;

	FPUSH_VTX3(p, 0, half_height, 0);//top vertex
	FPUSH_VTX3(normal, 0, 1, 0);//top vertex
	side_idx.push_back(0);

	for (int i = 0; i <= n; ++i) {
		theta = (GLfloat)(2.0 * M_PI * i / n);
		x = radius * sin(theta);
		z = radius * cos(theta);
		FPUSH_VTX3(p, x, -half_height, z);
		FPUSH_VTX3(normal, x / radius, 0, z / radius);//top vertex

		side_idx.push_back(i + 1);
	}

	FPUSH_VTX3(p, 0, -half_height, 0);// bottom-center vertex
	FPUSH_VTX3(normal, 0, -1, 0);//bottom
	bottom_idx.push_back(n+2);


	for (int i = n; i >= 0; --i) {
		theta = (GLfloat)(2.0 * M_PI * i / n);
		x = radius * sin(theta);
		z = radius * cos(theta);
		FPUSH_VTX3(p, x, -half_height, z);
		FPUSH_VTX3(normal, 0,-1,0);//top vertex

		bottom_idx.push_back(2*n + 3 - i);
	}


	
};




//cylinder
/*
void get_cylinder_3d(std::vector<GLfloat>& p, std::vector<GLfloat>& normal, std::vector<size_t>& side_idx,
	std::vector<size_t>& top_idx, std::vector<size_t>& bottom_idx, GLfloat radius,
	GLfloat height, GLint n) {
	GLfloat half_height = height / 2;
	GLfloat theta, x, z;
	p.resize(3 * (2 * n + 4));
	normal.resize(3 * (2 * n + 4));

	FPUSH_VTX3_AT(p, 0, 0, half_height, 0);//top center
	FPUSH_VTX3_AT(normal, 0, 0, 1, 0);//top center

	top_idx.push_back(0);
	bottom_idx.push_back(2 * n + 3);
	for (int i = 0;i <= n; i++) {
		theta = (GLfloat)(2.0 * M_PI * i / n);
		x = radius * sin(theta);
		z = radius * cos(theta);
		FPUSH_VTX3_AT(p, 2 * i + 1, x, half_height, z);
		FPUSH_VTX3_AT(p, 2 * i + 2, x, -half_height, z);
		FPUSH_VTX3_AT(normal,2 * i + 1, x/radius,0,z/radius);
		FPUSH_VTX3_AT(normal, 2 * i + 1, x / radius, 0, z / radius);

		side_idx.push_back(2 * i + 1);
		side_idx.push_back(2 * i + 2);
		top_idx.push_back(2 * i + 1);
		bottom_idx.push_back(2 * n + 2 - 2 * i);
	}
	FPUSH_VTX3_AT(p, 2 * n + 3, 0, -half_height, 0);
};*/


//cylinder
void get_cylinder_3d(std::vector<GLfloat>& p, std::vector<GLfloat>& normal, std::vector<size_t>& side_idx,
	std::vector<size_t>& top_idx, std::vector<size_t>& bottom_idx, GLfloat radius,
	GLfloat height, GLint n) {
	GLfloat half_height = height / 2;
	GLfloat theta, x, z;
	p.resize(3*(4*n+5+1));
	normal.resize(3*(4 * n + 5+1));

	FPUSH_VTX3_AT(p,  0,     0, half_height, 0);//top center
	FPUSH_VTX3_AT(normal, 0, 0,  1, 0);//top center

	top_idx.push_back(0); //until n+1



	for (int i = 0;i <= n; i++) {
		theta = (GLfloat)(2.0 * M_PI * i / n);
		x = radius * sin(theta);
		z = radius * cos(theta);
		FPUSH_VTX3_AT(p,i+1, x, half_height, z);
		FPUSH_VTX3_AT(normal,i+1, 0, 1, 0);

		top_idx.push_back(i + 1); //until n+1
		printf("%d ", i + 1);

	}
	
	for (int i = 0;i <= n; i++) {
		theta = (GLfloat)(2.0 * M_PI * i / n);
		x = radius * sin(theta);
		z = radius * cos(theta);
		FPUSH_VTX3_AT(p, n + 2 * (i + 1), x, half_height, z);
		FPUSH_VTX3_AT(p, n + 2 * (i + 1) + 1, x, -half_height, z);
		FPUSH_VTX3_AT(normal, n + 2 * (i + 1),  x / radius, 0, z / radius);
		FPUSH_VTX3_AT(normal, n + 2 * (i + 1) + 1,  x / radius, 0, z / radius);

		side_idx.push_back(n + 2 * (i + 1));//n+2 n+4
		side_idx.push_back(n + 2 * (i + 1) + 1);//n+3 n+5
		printf("%d ", n + 2 * (i + 1));
		printf("%d ", n + 2 * (i + 1) + 1);


	}
	
	FPUSH_VTX3_AT(p, 3 * n + 4 ,0, -half_height, 0);
	FPUSH_VTX3_AT(normal, 3 * n + 4, 0, -1, 0);

	bottom_idx.push_back(3 * n + 4); //until n+1


	for (int i = n;i >=0 ; i--) {
		theta = (GLfloat)(2.0 * M_PI * i / n);
		x = radius * sin(theta);
		z = radius * cos(theta);
		FPUSH_VTX3_AT(p, 3 * n + 5 + i, x, -half_height, z);
		FPUSH_VTX3_AT(normal, 3 * n + 5 + i, 0, -1,0);
		printf("%d ", 3 * n + 5 + i);

		bottom_idx.push_back(3 * n + 5 + i); //until n+1

	}
};


//subh suba, 구를 분할어떻게 할건지
void get_sphere_3d(GLvec& p, GLvec& normals, GLfloat r, GLint subh, GLint suba) {
	//높이축 움직임
	double y0, rst0, y1, rst1;
	for (int i = 1; i <= subh; ++i) {

		//solve redundancy
		double theta1 = M_PI * i / subh;
		if (i == 1) {
			double theta0 = M_PI * ((double)i - 1) / subh;
			y0 = r * cos(theta0);
			rst0 = r * sin(theta0);
			y1 = r * cos(theta1);
			rst1 = r * sin(theta1);
		}
		else {
			y0 = y1;
			rst0 = rst1;
			y1 = r * cos(theta1);
			rst1 = r * sin(theta1);
		}



		//평면
		float vx0, vy0, vz0, vx1, vy1, vz1;
		float vx2, vy2, vz2, vx3, vy3, vz3;

		double cp0, sp0, cp1, sp1;

		for (int j = 1; j <= suba; ++j) {

			double phi1 = 2 * M_PI * j / suba;

			if (j == 1) {
				double phi0 = 2 * M_PI * (0) / suba;
				cp0 = cos(phi0);
				sp0 = sin(phi0);
				cp1 = cos(phi1);
				sp1 = sin(phi1);
			}
			else {// solve redundancy
				cp0 = cp1;
				sp0 = sp1;
				cp1 = cos(phi1);
				sp1 = sin(phi1);
			}

			//하나의 사각형을 만들기 위한 점 4개 (각각의 좌표)
			// 이전의 v3 v4 가 지금의 v0 v1
			//별건 아니고 float 형식으로 바꾸는 중

			if (j == 1) {
				FSET_VTX3(vx0, vy0, vz0, sp0 * rst0, y0, cp0 * rst0);
				FSET_VTX3(vx1, vy1, vz1, sp0 * rst1, y1, cp0 * rst1);
				FSET_VTX3(vx2, vy2, vz2, sp1 * rst0, y0, cp1 * rst0);
				FSET_VTX3(vx3, vy3, vz3, sp1 * rst1, y1, cp1 * rst1);
			}
			else {// solve redundancy
				vx0 = vx2;
				vy0 = vy2;
				vz0 = vz2;
				vx1 = vx3;
				vy1 = vy3;
				vz1 = vz3;
				FSET_VTX3(vx2, vy2, vz2, sp1 * rst0, y0, cp1 * rst0);
				FSET_VTX3(vx3, vy3, vz3, sp1 * rst1, y1, cp1 * rst1);
			}
			//사각형 만드는순서에 맞춰서 섬세하게 넣어주는 과정.
			if (i < subh) {

				//first triangle (v0, v1, v3)
				FPUSH_VTX3(p, vx0, vy0, vz0);
				FPUSH_VTX3(p, vx1, vy1, vz1);
				FPUSH_VTX3(p, vx3, vy3, vz3);

				FPUSH_VTX3(normals, vx0 / r, vy0 / r, vz0 / r);
				FPUSH_VTX3(normals, vx1 / r, vy1 / r, vz1 / r);
				FPUSH_VTX3(normals, vx2 / r, vy2 / r, vz2 / r);

			}
			if (1 < i) {
				//second triangle
				//first triangle (v3, v2, v0)

				FPUSH_VTX3(p, vx3, vy3, vz3);
				FPUSH_VTX3(p, vx2, vy2, vz2);
				FPUSH_VTX3(p, vx0, vy0, vz0);

				FPUSH_VTX3(normals, vx3 / r, vy3 / r, vz3 / r);
				FPUSH_VTX3(normals, vx2 / r, vy2 / r, vz2 / r);
				FPUSH_VTX3(normals, vx0 / r, vy0 / r, vz0 / r);

			}
		}
	}
}

//torus

void get_torus_3d(std::vector<GLfloat>& p,std::vector<GLfloat>& normal, std::vector<std::vector<size_t>>& side_idx,
	GLfloat r0, GLfloat r1, GLint na, GLint nh) {

	GLfloat theta0, theta1, x, z, y, nx,ny,nz;
	p.resize(3 * (na + 1) * (nh + 1));
	normal.resize(3 * (na + 1) * (nh + 1));

	side_idx.resize(nh);


	for (int i = 0;i <= (nh); i++) {//0 1 2 3 4 5 6 
		theta0 = (GLfloat)(2.0 * M_PI - 2.0 * M_PI * i / nh); //높이(파이)

		if (i != nh) { side_idx[i].resize(2 * (na + 1)); }
		for (int j = 0; j <= na; j++) {//0 1 2 3 4 5 6 

			theta1 = (GLfloat)(2.0 * M_PI * j / na); //넓이(세타)

			x = (r0 + r1) * sin(theta1) + r1 * cos(theta0) * sin(theta1);
			z = (r0 + r1) * cos(theta1) + r1 * cos(theta0) * cos(theta1);

			y = r1 * sin(theta0);
			nx = (r1 * cos(theta0)) * sin(theta1) / r1;
			ny = r1 * sin(theta0) / r1;
			nz = (r1 * cos(theta0)) * cos(theta1) / r1;



			FPUSH_VTX3_AT(p, i * (na + 1) + j, x, y, z);// 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15...
			FPUSH_VTX3_AT(normal, i * (na + 1) + j, nx, ny, nz);

			
			if (i != nh) {
				side_idx[i].push_back(i * (na + 1) + j);//0,1,2,3,4,5,6,
				side_idx[i].push_back((i + 1) * (na + 1) + j);
				printf(" %d ", i * (na + 1) + j);
				printf(" %d ", (i + 1) * (na + 1) + j);
			}

		}
		printf("\n");
	}
};


void get_grid(std::vector<GLfloat>& p, GLfloat w, GLfloat h, int m, int n) {
	GLfloat x0 = -0.5f * w;
	GLfloat x1 = +0.5f * w;
	GLfloat z0 = -0.5f * h;
	GLfloat z1 = +0.5f * h;

	for (int i = 0; i <= m; ++i) {
		GLfloat x = x0 + w * i / m;
		FPUSH_VTX3(p, x, 0, z0);
		FPUSH_VTX3(p, x, 0, z1);
	}


	for (int i = 0; i <= n; ++i) {
		GLfloat z = z0 + h * i / n;
		FPUSH_VTX3(p, x0, 0, z);
		FPUSH_VTX3(p, x1, 0, z);
	}
}



//색을 칠하자
void get_color_3d_by_pos(GLvec& c, GLvec& p, GLfloat offset)
{
	GLfloat max_val[3] = { -INFINITY, -INFINITY, -INFINITY };
	GLfloat min_val[3] = { INFINITY, INFINITY, INFINITY };

	//x, y z 각각 최대 최소 구하기
	int n = (int)(p.size() / 3);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < 3; ++j) {
			GLfloat val = p[i * 3 + j];
			if (max_val[j] < val)max_val[j] = val;
			else if (min_val[j] > val) min_val[j] = val;
		}
	}
	//최대와 최소 넓이 구하기(이번 과제는 구라서 모두값이 같을거라 예상됨
	GLfloat width[3] = {
		max_val[0] - min_val[0],
		max_val[1] - min_val[1],
		max_val[2] - min_val[2]

	};

	//색 buffer 를 꼭지점 buffer 와 같은 크기로
	c.resize(p.size());

	//위치값을 0과 1사이로 조정 후 offset추가해서 위치값에 따른 색상 만들기
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < 3; ++j) {
			int k = i * 3 + j;
			c[k] = std::fminf((p[k] - min_val[j]) / width[j] + offset, 1.5f);
		}
	}

};


/*


void bind_buffer(GLint buffer, GLvec& vec, int program,
	const GLchar* attri_name, GLint attri_size)
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vec.size(),
		vec.data(), GL_STATIC_DRAW);
	GLuint location = glGetAttribLocation(program, attri_name);
	glVertexAttribPointer(location, attri_size, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(location);

};

void bind_buffer(GLint buffer, std::vector<GLuint>& vec, int program) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vec.size(), vec.data(), GL_STATIC_DRAW);
}

*/