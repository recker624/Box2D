#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

#include "stb_image.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#define WIN_HEIGHT 1080
#define WIN_WIDTH 1920

float moveDis = 0;

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, unsigned int shaderProgram);
void applyTextures(unsigned int& firstTexture, unsigned int& secondTexture);


int main() 
{
	//initialize glfw
	if (!glfwInit()) 
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	//set window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	//create window
	// use glfwGetPrimaryMonitor() for fullscreen mode
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Box 2D", NULL, NULL);
	if (window == NULL) 
	{
		std::cout << "Window creation unsuccessful" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	//register the callback functions
	//any callbacks should be registered after we have set a window context as "current"
	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	
	//initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	float vertices[] = 
	{
		// positions          // texture coords
		 -0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f
	};

	//create buffer objects
	VertexArray va;
	VertexBuffer vbo(vertices, sizeof(vertices));
	VertexBufferLayout layout;
	layout.Push<float>(3);		//position coordinates
	layout.Push<float>(2);	//texture coordinates
	va.AddBuffers(vbo, layout);
	
	//--------------------
	//SHADERS
	Shader shaderProgram("vertex.vs", "fragment.txt");
	shaderProgram.Bind();

	//--------
	//TEXTURES
	Texture texture;
	texture.push("C:\\Users\\sahil\\OneDrive\\Desktop\\awesomeface.jpg");
	texture.push("C:\\Users\\sahil\\OneDrive\\Desktop\\container.jpg");
	texture.CreateTexture();

	//set the values of sampler2d variables in fragment shader
	shaderProgram.SetUniformSampler2D("firstTexture", 0);
	shaderProgram.SetUniformSampler2D("secondTexture", 1);

	//define the transformation matrices
	//----------------------------------
	glm::mat4 model;
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(50.0f), 16.0f/9.0f, 0.1f, 100.0f);

	shaderProgram.SetUniform4f("model", model);
	shaderProgram.SetUniform4f("view", view);
	shaderProgram.SetUniform4f("projection", projection);

	//enable depth buffer
	GLCall(glEnable(GL_DEPTH_TEST));

	//render loop
	while (!glfwWindowShouldClose(window)) 
	{
		processInput(window, shaderProgram.GetShaderID());

		GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		texture.Bind(0);
		//texture.Bind(1);

		va.Bind();
		shaderProgram.Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}


void processInput(GLFWwindow* window, unsigned int shaderProgram)
{
	glm::mat4 view;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		GLCall(glfwSetWindowShouldClose(window, GL_TRUE));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		moveDis -= 0.01f;
		view = glm::translate(view, glm::vec3(moveDis, 0.0f, -3.0f));
		GLCall(glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view)));
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		moveDis += 0.01f;
		view = glm::translate(view, glm::vec3(moveDis, 0.0f, -3.0f));
		GLCall(glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view)));
	}

}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	//set dimensions for the viewport
	GLCall(glViewport(0, 0, width, height));
}