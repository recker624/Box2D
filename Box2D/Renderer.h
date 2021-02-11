#pragma once
#include<glad/glad.h>

//if any error occurs then insert a breakpoint at that line
#define ASSERT(x) if((x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* func, const char* file, int line);