#version 330 core

in vec3 vColor;
in vec3 vPosition;

out vec3 color;

uniform mat4 matrix; 

void main()
{
    gl_Position = matrix * vec4(vPosition, 1.0);
    color = vColor;
}

