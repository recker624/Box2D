#include<glad/glad.h>
#include<GLFW/glfw3.h>

//for matrix calculations
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

//IMGUI (for GUI)
#include"vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include<iostream>

#include "stb_image.h"
#include "Renderer.h"
#include"VertexBufferLayout.h"
#include"camera.h"

#define WIN_HEIGHT 1080
#define WIN_WIDTH 1920

float moveDis = 0;

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void no_mouse_callback(GLFWwindow* window, double xpos, double ypos);

//GLOBAL VARIABLES
bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

// timing
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

/// for lighting
float ambientStrength = 0.1f;
float shininess = 256.0f;
float specularStrength = 0.5f;

glm::vec3 cameraPosition(-8.0f, 6.0f, 8.0f);
float yaw = -45.0f;
float pitch = -35.0f;
camera camera(cameraPosition, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch);

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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glfwSetCursorPosCallback(window, mouse_callback);

	//glfwSetScrollCallback(window, scroll_callback);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	float vertices[] = 
	{
		// positions          // normal vectors
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	float verticesLamp[] =
	{
		// positions       
		 -0.5f, -0.5f, -0.5f,	
		 0.5f, -0.5f, -0.5f,	
		 0.5f,  0.5f, -0.5f,	
		 0.5f,  0.5f, -0.5f,	
		-0.5f,  0.5f, -0.5f,	
		-0.5f, -0.5f, -0.5f,	

		-0.5f, -0.5f,  0.5f,	
		 0.5f, -0.5f,  0.5f,	
		 0.5f,  0.5f,  0.5f,	
		 0.5f,  0.5f,  0.5f,	
		-0.5f,  0.5f,  0.5f,	
		-0.5f, -0.5f,  0.5f,	

		-0.5f,  0.5f,  0.5f,	
		-0.5f,  0.5f, -0.5f,	
		-0.5f, -0.5f, -0.5f,	
		-0.5f, -0.5f, -0.5f,	
		-0.5f, -0.5f,  0.5f,	
		-0.5f,  0.5f,  0.5f,	

		 0.5f,  0.5f,  0.5f,	
		 0.5f,  0.5f, -0.5f,	
		 0.5f, -0.5f, -0.5f,	
		 0.5f, -0.5f, -0.5f,	
		 0.5f, -0.5f,  0.5f,	
		 0.5f,  0.5f,  0.5f,	

		-0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f,  
		 0.5f, -0.5f,  0.5f,  
		 0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f, -0.5f, 

		-0.5f,  0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 
		 0.5f,  0.5f,  0.5f,  
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,  
		-0.5f,  0.5f, -0.5f
	};

	//enable blending
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	//---------------------
	//create buffer objects (for cube)
	VertexArray va;
	VertexBuffer VBO(vertices, sizeof(vertices));
	VertexBufferLayout layout;
	layout.Push<float>(3);		//position coordinates
	//layout.Push<float>(2);	//texture coordinates
	layout.Push<float>(3);	//normal vectors for diffuse lighting
	va.AddBuffers(VBO, layout);
	
	//------------
	//buffer objects (for light source)
	VertexArray lampVAO;
	VertexBuffer lampVBO(verticesLamp, sizeof(verticesLamp));
	VertexBufferLayout lampLayout;
	lampLayout.Push<float>(3);
	lampVAO.AddBuffers(lampVBO, lampLayout);

	//--------------------
	//SHADERS (for cube)
	Shader shaderProgram("Resources/Shaders/vertex.vs", "Resources/Shaders/fragment.txt");
	shaderProgram.Bind();

	//-------------------
	//Shader (for light source/lamp)
	Shader lampShaderProgram("Resources/Shaders/lampVertex.vs", "Resources/Shaders/lampFragment.txt");

	//enable depth buffer
	GLCall(glEnable(GL_DEPTH_TEST));

	Renderer renderer;

	//Create imgui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init((char*)glGetString(330));

	float currentFrameTime;

	//render loop
	while (!glfwWindowShouldClose(window)) 
	{
		//camera values
		camera.Pitch = pitch;
		camera.Yaw = yaw;
		camera.updateCameraVectors();

		//for hardware-independent movement speed
		currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		processInput(window);
		renderer.Clear();

		//Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//for first cube
		shaderProgram.Bind();

		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 projection(1.0f);
		glm::vec3 moveVec(0.0f, 0.0f, 0.0f);

		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

		glm::vec3 moveVec_2(2 * sin(glfwGetTime()), 0.0f, 2 * cos(glfwGetTime()));

		model = glm::translate(model, moveVec);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), 1920 / 1080.0f, 0.1f, 100.0f);
		
		shaderProgram.SetUniform4f("model", model);
		shaderProgram.SetUniform4f("view", view);
		shaderProgram.SetUniform4f("projection", projection);

		shaderProgram.SetUniform3f("lightColor", lightColor);
		shaderProgram.SetUniform3f("objectColor", objectColor);

		shaderProgram.SetUniform3f("lightPos", moveVec_2);
		shaderProgram.SetUniform3f("viewPos", camera.Position);
		
		shaderProgram.SetFloat("ambientStrength", ambientStrength);
		shaderProgram.SetFloat("specularStrength", specularStrength);
		shaderProgram.SetFloat("shininess", shininess);

		renderer.Draw(shaderProgram, va);

		//----------------
		//the light source
		lampShaderProgram.Bind();

		glm::mat4 model_2(1.0f);

		model_2 = glm::translate(model_2, moveVec_2);
		model_2 = glm::scale(model_2, glm::vec3(0.2f));
		lampShaderProgram.SetUniform4f("model", model_2);
		lampShaderProgram.SetUniform4f("view", view);
		lampShaderProgram.SetUniform4f("projection", projection);

		renderer.Draw(lampShaderProgram, lampVAO);

		//for adding GUI to the window
		ImGui::SliderFloat("Ambient", &ambientStrength, 0.0f, 1.0f);           
		ImGui::SliderFloat("Specular", &specularStrength, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &shininess, 2.0f, 2048.0f);

		ImGui::SliderFloat("Yaw", &yaw, -89.0f, 89.0f);
		ImGui::SliderFloat("Pitch", &pitch, -45.0f, 45.0f);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//terminate imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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

	//INPUT FOR THE CAMERA
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
	}
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	//set dimensions for the viewport
	GLCall(glViewport(0, 0, width, height));
}

void no_mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//use this function when we don't want to register camera movement when we move cursor
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
