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


	VertexArray va;
	VertexBuffer vbo(vertices, sizeof(vertices));
	VertexBufferLayout layout;
	layout.Push<float>(3);		//position coordinates
	layout.Push<float>(2);	//texture coordinates
	va.AddBuffers(vbo, layout);
	

	//--------------------
	//dealing with shaders
	Shader shaderProgram("vertex.vs", "fragment.txt");
	shaderProgram.Bind();

	unsigned int firstTexture, secondTexture;
	applyTextures(firstTexture, secondTexture);

	//set the values of sampler2d variables in fragment shader
	glUniform1i(glGetUniformLocation(shaderProgram.GetShaderID(), "firstTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram.GetShaderID(), "secondTexture"), 1);

	//define the transformation matrices
	//----------------------------------
	glm::mat4 model;
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(50.0f), 16.0f/9.0f, 0.1f, 100.0f);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.GetShaderID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.GetShaderID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.GetShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//enable depth buffer
	glEnable(GL_DEPTH_TEST);

	//render loop
	while (!glfwWindowShouldClose(window)) 
	{
		processInput(window, shaderProgram.GetShaderID());

		GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, firstTexture));
		GLCall(glActiveTexture(GL_TEXTURE1));
		GLCall(glBindTexture(GL_TEXTURE_2D, secondTexture));

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

void applyTextures(unsigned int& firstTexture, unsigned int& secondTexture) 
{

	GLCall(glGenTextures(1, &firstTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, firstTexture));
	// set the texture wrapping parameters
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));	// set texture wrapping to GL_REPEAT (default wrapping method)
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	// set texture filtering parameters
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); 

	//convert the image pixel date to bytes
	unsigned char* data = stbi_load("C:\\Users\\sahil\\OneDrive\\Desktop\\container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	// texture 2
	GLCall(glGenTextures(1, &secondTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, secondTexture));
	// set the texture wrapping parameters
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));	// set texture wrapping to GL_REPEAT (default wrapping method)
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	// set texture filtering parameters
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	// load image, create texture and generate mipmaps
	data = stbi_load("C:\\Users\\sahil\\OneDrive\\Desktop\\awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

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