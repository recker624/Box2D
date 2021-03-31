#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<iostream>

#include "stb_image.h"
#include "Renderer.h"
//#include "IndexBuffer.h"
#include"VertexBufferLayout.h"
#include "Texture.h"
#include"camera.h"

#define WIN_HEIGHT 576
#define WIN_WIDTH 1024

float moveDis = 0;

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//GLOBAL VARIABLES
bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

camera camera;

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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

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

	//enable blending
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	//---------------------
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
	//texture.push("C:\\Users\\sahil\\OneDrive\\Desktop\\awesomeface.png");
	texture.push("C:\\Users\\sahil\\OneDrive\\Desktop\\container.jpg");
	texture.CreateTexture();

	//set the values of sampler2d variables in fragment shader
	shaderProgram.SetUniformSampler2D("firstTexture", 0);
	//shaderProgram.SetUniformSampler2D("secondTexture", 1);

	//----------------------------------
	//define the transformation matrices
	//glm::mat4 model;
	//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 view;

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(50.0f), 16.0f/9.0f, 0.1f, 100.0f);

	//shaderProgram.SetUniform4f("model", model);
	//shaderProgram.SetUniform4f("view", view);
	shaderProgram.SetUniform4f("projection", projection);

	//enable depth buffer
	GLCall(glEnable(GL_DEPTH_TEST));

	Renderer renderer;
	
	float currentFrame;
	//render loop
	while (!glfwWindowShouldClose(window)) 
	{
		//for hardware-independent movement speed
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		renderer.Clear();

		texture.Bind(0);
		//texture.Bind(1);

		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		view = camera.GetViewMatrix();

		shaderProgram.SetUniform4f("projection", projection);
		shaderProgram.SetUniform4f("view", view);

		renderer.Draw(shaderProgram, va);
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}


void processInput(GLFWwindow* window)
{
	const float cameraSpeed = 2.5 * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
	{
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	}
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	//set dimensions for the viewport
	GLCall(glViewport(0, 0, width, height));
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}