// A simple vertex shader
#version 110

varying vec3 normal;

void main()
{
	normal = gl_NormalMatrix * gl_Normal;
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}