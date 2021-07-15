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

//Model loading
#include"Model.h"

#include "stb_image.h"
#include "Renderer.h"
#include"camera.h"
#include"Texture.h"

#include<iostream>

#define WIN_HEIGHT 768
#define WIN_WIDTH 1366

float moveDis = 0;

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

//GLOBAL VARIABLES
bool firstMouse = true;
float lastX = WIN_WIDTH / 2.0;
float lastY = WIN_HEIGHT / 2.0;

// timing
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

//x and z coordinates of the lightCube
float xPosLight = -0.2f;
float yPosLight = -1.0f;
float zPosLight = -0.3f;

/// for lighting
glm::vec3 specularStrength = glm::vec3(0.7f, 0.7f, 0.7f);
float shininess = 32.0f;
//for attenuation
float linear = 0.09f;
float quadratic = 0.032f;

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

	//initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	//enable blending
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	//---------------------
	//Load the model
	char path[] = "C:/Users/sahil/OneDrive/Desktop/backpack/backpack.obj";
	Model model(path);

	//--------------------
	//SHADERS (for cube)
	Shader shaderProgram("Resources/Shaders/vertex.vs", "Resources/Shaders/fragment.txt");

	//buffer objects (for lamp)

	shaderProgram.Bind();

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

	//render loop
	while (!glfwWindowShouldClose(window)) 
	{
		//camera values
		camera.Pitch = pitch;
		camera.Yaw = yaw;
		camera.updateCameraVectors();

		//for hardware-independent movement speed
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		processInput(window);
		renderer.Clear();

		//Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//---------------
		//for the main cube
		shaderProgram.Bind();

		glm::mat4 modelMatrix(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 projection(1.0f);
		glm::vec3 moveVec(0.0f, 0.0f, 0.0f);
		glm::vec3 moveVec_2(2.0f, 1.0f, 2.0f);

		modelMatrix = glm::translate(modelMatrix, moveVec);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), 16.0f/9.0f, 0.1f, 100.0f);
		
		shaderProgram.SetUniform4f("model", modelMatrix);
		shaderProgram.SetUniform4f("view", view);
		shaderProgram.SetUniform4f("projection", projection);

		shaderProgram.SetUniform3f("lightPos", moveVec_2);
		shaderProgram.SetUniform3f("viewPos", camera.Position);

		shaderProgram.SetUniform3f("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shaderProgram.SetUniform3f("light.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
		shaderProgram.SetUniform3f("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shaderProgram.SetUniform3f("light.direction", glm::vec3(xPosLight, yPosLight, zPosLight));

		//render the object
		model.Draw(shaderProgram);

		//----------------
		//for adding GUI to the window
		ImGui::SliderFloat("Yaw", &yaw, -89.0f, 89.0f);
		ImGui::SliderFloat("Pitch", &pitch, -45.0f, 45.0f);
		ImGui::SliderFloat("Light_X", &xPosLight, -1.00f, 1.00f);
		ImGui::SliderFloat("Light_Y", &yPosLight, -1.00f, 1.00f);
		ImGui::SliderFloat("Light_Z", &zPosLight, -1.00f, 1.00f);
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