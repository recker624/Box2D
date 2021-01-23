#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec3 updatePos;


void main(){
	//an inbuilt variable in glsl to take in the position data of the vertices
	gl_Position = vec4((aPos/10) + updatePos, 1.0);
}