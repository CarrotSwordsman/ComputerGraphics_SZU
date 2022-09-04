#version 330 core

in vec4 vPosition;
in vec4 vColor;
out vec4 color;

uniform int xsize;
uniform int ysize;

void main() 
{
	mat4 scale = mat4(2.0/xsize, 0.0, 0.0, 0.0,
					  0.0,  2.0/ysize, 0.0, 0.0,
					  0.0, 0.0, 1.0, 0.0,
					  0.0, 0.0, 0.0, 1.0 );

	mat4 transl = mat4( 1, 0, 0, -xsize/2,
						0, 1, 0, -ysize/2,
						0, 0, 1, 0,
						0, 0, 0, 1);

    // First, center the image by translating each vertex by half of
	// the original window size (since the vertices originally have x
	// component in (0, 560) and y component in (0, 724).
	//
	// Then, multiply by the scale matrix to maintain size after the
	// window is reshaped
	
	gl_Position = vPosition * transl * scale; 
	
    color = vColor;
}