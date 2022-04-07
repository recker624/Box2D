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

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void processInput(GLFWwindow *window);
static void toogleCursor(GLFWwindow* window, int key, int scancode, int action, int mods);
static GLFWwindow* setupGLFW();

static float moveDis = 0;

//to enable disable cursor
static bool cursorActive = false;

static bool firstMouseMove = true;
static float lastX = WIN_WIDTH / 2.0;
static float lastY = WIN_HEIGHT / 2.0;

// timing
static float deltaTime = 0.0f;
static  float lastFrameTime = 0.0f;

//transform for main model
static glm::vec3 moveVec(0.0f, 0.0f, 0.0f);
static glm::vec3 scaleVec(1.0f);
static float rotateX = 0;
static float rotateY = 0;
static float rotateZ = 0;
static bool reset = false;

//x y z coordinates of the lightCube
static float xPosLight = -0.2f;
static float yPosLight = -1.0f;
static float zPosLight = -0.3f;

//direction for directional light
static float xDirLight = -0.2f;
static float yDirLight = -1.0f;
static float zDirLight = -0.3f;

//position for lampLight
static glm::vec3 moveVec_2(2.0f, 1.0f, 2.0f);

static bool lampActive = false;
static bool directionalActive = true;

static glm::vec3 cameraPosition(-12.0f, 10.0f, 12.0f);
static float yaw = -45.0f;
static float pitch = -35.0f;
static Camera camera(cameraPosition, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch);

int main()
{
	GLFWwindow *window = setupGLFW();

	//initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	//enable blending
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	char path[] = "3d Models/backpack/backpack.obj";
	Model model(path);

	//--------------------
	//SHADERS (for cube)
	Shader shaderProgram("Resources/Shaders/vertex.vs", "Resources/Shaders/fragment.txt");
	shaderProgram.Bind();

	//For the point light model
	char lampPath[] = "3d Models/Sphere/lightSphere.obj";
	Model lampModel(lampPath);

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

	//render loop
	while (!glfwWindowShouldClose(window)) 
	{
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
		//for the main model
		shaderProgram.Bind();

		glm::mat4 modelMatrix(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 projection(1.0f);
		
		modelMatrix = glm::translate(modelMatrix, moveVec);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotateX), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotateZ), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::scale(modelMatrix, scaleVec);

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), 16.0f/9.0f, 0.1f, 100.0f);
		
		shaderProgram.SetUniform4f("model", modelMatrix);
		shaderProgram.SetUniform4f("view", view);
		shaderProgram.SetUniform4f("projection", projection);

		shaderProgram.SetFloat("lampActive", lampActive);
		shaderProgram.SetFloat("directionalActive", directionalActive);

		shaderProgram.SetUniform3f("cameraPos", camera.Position);

		if (directionalActive) {
			shaderProgram.SetUniform3f("sunLight.ambient", glm::vec3(0.4f, 0.4f, 0.4f));
			shaderProgram.SetUniform3f("sunLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
			shaderProgram.SetUniform3f("sunLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
			shaderProgram.SetUniform3f("sunLight.direction", glm::vec3(xDirLight, yDirLight, zDirLight));
		}

		if (lampActive) {
			shaderProgram.SetUniform3f("lampLightPos", moveVec_2);
			shaderProgram.SetUniform3f("lampLight.ambient", glm::vec3(0.4f, 0.4f, 0.4f));
			shaderProgram.SetUniform3f("lampLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
			shaderProgram.SetUniform3f("lampLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		}

		//render the object
		model.Draw(shaderProgram);

		//----------------
		//for adding GUI to the window
		reset = ImGui::Button("Reset");
		ImGui::Text("Translate");
		ImGui::SliderFloat("translateX", &moveVec.x, -10.0f, 10.0f);
		ImGui::SliderFloat("translateY", &moveVec.y, -10.0f, 10.0f);
		ImGui::SliderFloat("translateZ", &moveVec.z, -10.0f, 10.0f);
		ImGui::Text("------------------------------------");
		ImGui::Text("Rotate");
		ImGui::SliderFloat("rotateX", &rotateX, -90.0f, 90.0f);
		ImGui::SliderFloat("rotateY", &rotateY, -90.0f, 90.0f);
		ImGui::SliderFloat("rotateZ", &rotateZ, -90.0f, 90.0f);
		ImGui::Text("------------------------------------");
		ImGui::Text("Scale");
		ImGui::SliderFloat("scaleX", &scaleVec.x, 1.0f, 5.0f);
		ImGui::SliderFloat("scaleY", &scaleVec.y, 1.0f, 5.0f);
		ImGui::SliderFloat("scaleZ", &scaleVec.z, 1.0f, 5.0f);

		if (reset) {
			moveVec = glm::vec3(0.0f);
			scaleVec = glm::vec3(1.0f);
			rotateX = 0.0f;
			rotateY = 0.0f;
			rotateZ = 0.0f;
		}
		ImGui::Text("------------------------------------");
		ImGui::Checkbox("Enable Directional Light", &directionalActive);
		ImGui::SliderFloat("SunLight_X", &xDirLight, -1.00f, 1.00f);
		ImGui::SliderFloat("SunLight_Y", &yDirLight, -1.00f, 1.00f);
		ImGui::SliderFloat("SunLight_Z", &zDirLight, -1.00f, 1.00f);	
		ImGui::Text("------------------------------------");
		ImGui::Checkbox("Enable Lamp", &lampActive);
		ImGui::SliderFloat("LampLight_X", &moveVec_2.x, -5.00f, 5.00f);
		ImGui::SliderFloat("LampLight_Y", &moveVec_2.y, -5.00f, 5.00f);
		ImGui::SliderFloat("LampLight_Z", &moveVec_2.z, -5.00f, 5.00f);
		ImGui::Text("------------------------------------");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		//for the lamp model (if active)
		if (lampActive) {
			lampShaderProgram.Bind();

			modelMatrix = glm::translate(modelMatrix, moveVec_2);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

			lampShaderProgram.SetUniform4f("model", modelMatrix);
			lampShaderProgram.SetUniform4f("view", view);
			lampShaderProgram.SetUniform4f("projection", projection);

			lampModel.Draw(lampShaderProgram);
		}


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

// enable/disable cursor during camera mode
void toogleCursor(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		if (cursorActive) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(window, mouse_callback);
			cursorActive = false;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPosCallback(window, NULL);
			cursorActive = true;
			firstMouseMove = true;
		}
	}
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	//set dimensions for the viewport
	GLCall(glViewport(0, 0, width, height));
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

GLFWwindow* setupGLFW() {
	//initialize glfw
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	//register the callback functions
	//any callbacks should be registered after we have set a window context as "current"
	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, toogleCursor);

	return window;
}
