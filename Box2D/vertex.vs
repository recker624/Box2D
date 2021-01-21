#version 330 core
layout (location = 0) in vec3 aPos;

void main(){
	//an inbuilt variable in glsl to take in the position data of the vertices
	gl_Position = vec4(aPos, 1.0);
}