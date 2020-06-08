#version 430

out vec4 fColorFront;
out vec4 fColorBack;
in vec4 vPosition;
in vec4 vNormal;

vec3 Ia = vec3(1.0, 1.0, 1.0);
vec3 Il= vec3(1.0, 1.0, 1.0);
float c[3] = {0.01,0.001,0.0};//fatt coefficients, arbitary constants
vec4 LightPos_wc = vec4(5,5,3,1);


layout(location=1) uniform mat4 M;
layout(location=2) uniform mat4 V;
layout(location=3) uniform mat4 P;

uniform vec3 Ka; 
uniform vec3 Ks ;
uniform vec3 Kd;
uniform float n ;


vec3 normalize( vec3 vector )
{
    float w = sqrt( vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2] );
    vector[0] /= w;
    vector[1] /= w;
    vector[2] /= w;

	return vector;
}



vec4 shading(vec3 LightPos_ec, vec3 vPosition_ec, vec3 vNormal_ec){
	vec3 N = normalize(vNormal_ec); // normal vector
	vec3 L = LightPos_ec - vPosition_ec; // light vector
	float d = length(L); L= L/d; 
	vec3 V = normalize(vec3(0.0)-vPosition_ec); //view vector
	vec3 R = reflect(-L, N); // reflection vector

	//As light moves from a light source, its intensity is diminished
	//d means distance

	float fatt = min(1.0/ (c[0] + c[1] *d + c[2]*d*d),1.0);

	float cos_theta = max(dot(N,L),0);//dot prodcut between Normal and reflection
	float cos_alpha = max(dot(V,R),0); //dot prodcut between view and reflection
	//I = IA(ambient reflection = Ia*Ka ) +ID(diffuse reflection) + IS(specular reflection)
	// = Ia*Ka + fatt*Il(kd*cos theta + ks(cos_theta)^n)
	vec3 I = Ia * Ka + fatt * Il * (Kd * cos_theta + Ks * pow(cos_alpha,n));

	return vec4(I, 1);
}



void main()
{

	mat4 VM = V*M;
	mat4 U = transpose(inverse(VM));


	vec3 vNormal_ec = vec3(normalize(U*vNormal));
	vec3 vPosition_ec = vec3(VM*vPosition);


	vec3 LightPos_ec = vec3(V * LightPos_wc);
	gl_Position= P * vec4(vPosition_ec, 1);

	
	fColorFront= shading(LightPos_ec, vPosition_ec, vNormal_ec);
	fColorBack= shading(LightPos_ec, vPosition_ec, -vNormal_ec);
	

}