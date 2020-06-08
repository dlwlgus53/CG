#version 430

out vec4 FragColor;

in vec3 LightPos_ec;
in vec3 vPosition_ec;
in vec3 vNormal_ec; // output texture coordinates
in vec4 fColor;
in vec4 fPosition;
in vec4 fNormal;
in vec2 fTexcoord;


uniform int ColorMode;// color mode
uniform int ObjectCode;// object code
uniform sampler2D sampler;// sampler2D


vec3 Ia = vec3(1.0, 1.0, 1.0);
vec3 Il= vec3(1.0, 1.0, 1.0);
float c[3] = {0.01,0.001,0.0};//fatt coefficients, arbitary constants
vec4 LightPos_wc = vec4(10,10,3,1); 

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
	if(ColorMode == 1) 
		FragColor= vec4(0,0,1,1);
	else if(ColorMode == 2){
		float nc = ObjectCode /255.0;
		FragColor = vec4(nc,nc,nc,1);
	}else{
		if( gl_FrontFacing) FragColor= shading(LightPos_ec, vPosition_ec, vNormal_ec);
		else FragColor= shading(LightPos_ec, vPosition_ec, -vNormal_ec);
		
		FragColor*= texture(sampler, fTexcoord);
	}
}


