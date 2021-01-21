#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<iostream>

#define WIN_HEIGHT 600
#define WIN_WIDTH 800

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void createBufferObjects(unsigned int& VBO, unsigned int& VAO, unsigned int& EBO);

int main() {
	//initialise glfw
	if (!glfwInit()) {
		std::cout << "Failed to initialise GLFW" << std::endl;
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
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Box 2D", NULL, NULL);
	if (window == NULL) {
		std::cout << "Window creation unsuccessful" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	//register callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	
	//initialise GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//createBufferObjects(vbo, vao, ebo);
	unsigned int vbo, vao, ebo;
	createBufferObjects(vbo, vao, ebo);

	//render loop
	while (!glfwWindowShouldClose(window)) {

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void createBufferObjects(unsigned int& vbo, unsigned int& vao, unsigned int& ebo) {
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	//generate a buffer object
	glGenBuffers(1, &vbo);
	//bind the buffer object to a buffer type
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//copy the vertex data into the buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);

	

}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
	//set dimensions for the viewport
	glViewport(0, 0, width, height);
}