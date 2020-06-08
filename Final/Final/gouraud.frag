#version 430

out vec4 FragColor;
in vec4 fColorFront;
in vec4 fColorBack;
uniform int ColorMode;




void main()
{
	if(ColorMode== 1) {
		FragColor= vec4(0,0,1,1);
		return;
	}

	if( gl_FrontFacing)
		FragColor= fColorFront;
	else
		FragColor= fColorBack; 
}


