#version 330

attribute vec3 position;
attribute vec3 normal;
attribute vec4 Ka;
attribute vec4 Kd;
attribute vec4 Ks;
attribute vec2 texcoord;

out vec2 texCoord0;
out vec3 normal0;
out vec3 color0;
out vec3 position0;
out float r;
out float sigma;
out vec2 texResolution;

uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;

void main()
{
	gl_Position = Proj *View * Model* vec4(position, 1.0); //you must have gl_Position
	r = 10;
    texResolution = vec2(256,256);  
	texCoord0 = texcoord;    
	color0 = vec3(Ka);
	sigma = 6;
	normal0 = (Model  * vec4(normal, 0.0)).xyz;
	position0 = vec3(Proj *View *Model * vec4(position, 1.0));
	
