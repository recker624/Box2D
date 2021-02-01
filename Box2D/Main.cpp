#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

#define WIN_HEIGHT 600
#define WIN_WIDTH 800

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, unsigned int shaderProgram);
void createGLObjects(unsigned int& VBO, unsigned int& VAO, unsigned int& EBO);
void createShaderProgram(unsigned int& shaderProgram);
std::string getShaderSource(std::string fileName);
void checkShaderCompileAndLinkErrors(unsigned int ID, std::string type);
void applyTextures(unsigned int& firstTexture, unsigned int& secondTexture);

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
	createGLObjects(vbo, vao, ebo);

	unsigned int shaderProgram;
	createShaderProgram(shaderProgram);
	glUseProgram(shaderProgram);

	unsigned int firstTexture, secondTexture;
	applyTextures(firstTexture, secondTexture);

	//set the values of sampler2d variables in fragment shader
	glUniform1i(glGetUniformLocation(shaderProgram, "firstTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "secondTexture"), 1);

	//render loop
	while (!glfwWindowShouldClose(window)) {

		processInput(window, shaderProgram);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, firstTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, secondTexture);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	glfwTerminate();
	return 0;
}





void createGLObjects(unsigned int& vbo, unsigned int& vao, unsigned int& ebo) {
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
	};

	unsigned int indices[] = { 
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	//bind the object to a type
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//copy the vertex data into the buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//tell opengl how to interpret the vertex data
	//position coordinates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//color values
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void createShaderProgram(unsigned int& shaderProgram) {
	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//attach shader with the source code
	std::string vertexStrCode = getShaderSource("vertex.vs");
	std::string fragmentStrCode = getShaderSource("fragment.txt");
	const char* vShaderSourceCode = vertexStrCode.c_str();
	const char* fShaderSourceCode = fragmentStrCode.c_str();

	glShaderSource(vertexShader, 1, &vShaderSourceCode, NULL);
	glCompileShader(vertexShader);
	checkShaderCompileAndLinkErrors(vertexShader, "VERTEX");

	glShaderSource(fragmentShader, 1, &fShaderSourceCode, NULL);
	glCompileShader(fragmentShader);
	checkShaderCompileAndLinkErrors(fragmentShader, "FRAGMENT");

	//create the shader program by attaching the shaders 
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkShaderCompileAndLinkErrors(shaderProgram, "PROGRAM");

}

//reads the shader text file and returns a string containing the source code
std::string getShaderSource(std::string fileName) {
	std::string sourceCode;
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open()) {
		//ensure ifstream objects can throw exceptions
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			//create stringstreams and read the file data into raw buffer
			std::stringstream shaderStream;
			shaderStream << shaderFile.rdbuf();
			//close the files
			shaderFile.close();
			//read the buffer contents into a string
			sourceCode = shaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
	}
	return sourceCode;
}

void checkShaderCompileAndLinkErrors(unsigned int ID, std::string type) {
	int success;
	char infolog[1024];
	//check what kind of object it is
	if (type == "VERTEX" || type == "FRAGMENT") {
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(ID, 1024, NULL, infolog);
			std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED" << infolog << std::endl;
		}
	}
	else if (type == "PROGRAM") {
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 1024, NULL, infolog);
			std::cout << "ERROR::SHADER::PROGRAM" << type << "::LINKING_FAILED" << infolog << std::endl;
		}
	}
	
}

void applyTextures(unsigned int& firstTexture, unsigned int& secondTexture) {
	//first texture
	glGenTextures(1, &firstTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, firstTexture);

	//set the texture parameters (wrapping/filtering options)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width_1, height_1, nrChannels_1; 
	//convert the image to an array of bytes
	unsigned char* imgData_1 = stbi_load("C:\\Users\\sahil\\OneDrive\\Desktop\\container.jpg", &width_1, &height_1, &nrChannels_1, 0);
	if (imgData_1) {
		//generate the texture using the image data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_1, height_1, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData_1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture_1" << std::endl;
	}

	//second Texture
	glGenTextures(1, &secondTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, secondTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width_2, height_2, nrChannels_2;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* imgData_2 = stbi_load("C:\\Users\\sahil\\OneDrive\\Desktop\\awesomeface.png", &width_2, &height_2, &nrChannels_2, 0);
	if (imgData_2) {
		//generate the texture using the image data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_2, height_2, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData_2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture_2" << std::endl;
	}

	stbi_image_free(imgData_1);
	stbi_image_free(imgData_2);
}

void processInput(GLFWwindow* window, unsigned int shaderProgram) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
	//set dimensions for the viewport
	glViewport(0, 0, width, height);
}