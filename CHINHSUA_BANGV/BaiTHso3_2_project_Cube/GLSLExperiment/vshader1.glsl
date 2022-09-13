#version 400
in vec4 vPosition;
in vec4 vColor;
out vec4 color;

uniform mat4 Model;
void main()
{
    gl_Position = Model * vPosition;
	color=vColor;
}//