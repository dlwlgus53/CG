#version 430



in vec4 vPosition_mc; // vertex position in modeling coordinates
in vec4 vNormal_mc; // normal in modeling coordinates
in vec4 vColor; // vertex color
in vec2 vTexcoord; // texture coordinates
layout(location=1) uniform mat4 M;//
layout(location=2) uniform mat4 V;//
layout(location=3) uniform mat4 P;//
layout(location=4) uniform mat4 T;//

out vec3 LightPos_ec;
out vec3 vPosition_ec;
out vec3 vNormal_ec; // output texture coordinates
out vec4 fColor;
out vec4 fPosition;
out vec4 fNormal;
out vec2 fTexcoord;

vec4 LightPos_wc  = vec4(5,5,3,1); 



vec3 normalize( vec3 vector )
{
    float w = sqrt( vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
    vector[0] /= w;
    vector[1] /= w;
    vector[2] /= w;
	return vector;
}

void main()
{	
	mat4 VMT = V*M*T;
	mat4 U = transpose(inverse(VMT));
	
	vNormal_ec = vec3(normalize(U*vNormal_mc));
	vPosition_ec = vec3(VMT * vPosition_mc);
	LightPos_ec = vec3(V * LightPos_wc);
	
	gl_Position = P * vec4(vPosition_ec, 1);
	fColor = vColor;
	fNormal = vNormal_mc; 
	fPosition = vPosition_mc;
	fTexcoord= vTexcoord;
}