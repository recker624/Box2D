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

#define WIN_HEIGHT 1080
#define WIN_WIDTH 1920

float moveDis = 0;


void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, unsigned int shaderProgram);
void createGLObjects();
void createShaderProgram(unsigned int& shaderProgram);
std::string getShaderSource(std::string fileName);
void checkShaderCompileAndLinkErrors(unsigned int ID, std::string type);
void applyTextures(unsigned int& firstTexture, unsigned int& secondTexture);



int main() {
	//initialize glfw
	if (!glfwInit()) {
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
	if (window == NULL) {
		std::cout << "Window creation unsuccessful" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	//register callback functions
	//any callbacks should be registered after we have set a window context as "current"
	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	
	//initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	createGLObjects();

	unsigned int shaderProgram;
	createShaderProgram(shaderProgram);
	glUseProgram(shaderProgram);

	unsigned int firstTexture, secondTexture;
	applyTextures(firstTexture, secondTexture);

	//set the values of sampler2d variables in fragment shader
	glUniform1i(glGetUniformLocation(shaderProgram, "firstTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "secondTexture"), 1);

	//define the transformation matrices
	//----------------------------------
	glm::mat4 model;
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(50.0f), 16.0f/9.0f, 0.1f, 100.0f);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//enable depth buffer
	glEnable(GL_DEPTH_TEST);

	//render loop
	while (!glfwWindowShouldClose(window)) {

		processInput(window, shaderProgram);

		GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, firstTexture));
		GLCall(glActiveTexture(GL_TEXTURE1));
		GLCall(glBindTexture(GL_TEXTURE_2D, secondTexture));

		GLCall(glUseProgram(shaderProgram));
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}





//create vertex buffer, vertex array and element buffer objects
void createGLObjects() {
	float vertices[] = {
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
	unsigned int vao;

	VertexBuffer vbo(vertices, sizeof(vertices), vao);
	
	//tell opengl how to interpret the vertex data
	//position coordinates
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	//texture coordinates
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(1));
}

//creates the shader objects and use them to create shader program
void createShaderProgram(unsigned int& shaderProgram) {
	unsigned int vertexShader, fragmentShader;
	GLCall(vertexShader = glCreateShader(GL_VERTEX_SHADER));
	GLCall(fragmentShader = glCreateShader(GL_FRAGMENT_SHADER));

	//attach shader with the source code
	std::string vertexStrCode = getShaderSource("vertex.vs");
	std::string fragmentStrCode = getShaderSource("fragment.txt");
	const char* vShaderSourceCode = vertexStrCode.c_str();
	const char* fShaderSourceCode = fragmentStrCode.c_str();

	GLCall(glShaderSource(vertexShader, 1, &vShaderSourceCode, NULL));
	GLCall(glCompileShader(vertexShader));
	checkShaderCompileAndLinkErrors(vertexShader, "VERTEX");

	GLCall(glShaderSource(fragmentShader, 1, &fShaderSourceCode, NULL));
	GLCall(glCompileShader(fragmentShader));
	checkShaderCompileAndLinkErrors(fragmentShader, "FRAGMENT");

	//create the shader program by attaching the shaders 
	GLCall(shaderProgram = glCreateProgram());
	GLCall(glAttachShader(shaderProgram, vertexShader));
	GLCall(glAttachShader(shaderProgram, fragmentShader));
	GLCall(glLinkProgram(shaderProgram));
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
		GLCall(glGetShaderiv(ID, GL_COMPILE_STATUS, &success));
		if (!success) {
			GLCall(glGetShaderInfoLog(ID, 1024, NULL, infolog));
			std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED" << infolog << std::endl;
		}
	}
	else if (type == "PROGRAM") {
		GLCall(glGetProgramiv(ID, GL_LINK_STATUS, &success));
		if (!success) {
			GLCall(glGetProgramInfoLog(ID, 1024, NULL, infolog));
			std::cout << "ERROR::SHADER::PROGRAM" << type << "::LINKING_FAILED" << infolog << std::endl;
		}
	}
	
}

void applyTextures(unsigned int& firstTexture, unsigned int& secondTexture) {

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

void processInput(GLFWwindow* window, unsigned int shaderProgram) {
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

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
	//set dimensions for the viewport
	GLCall(glViewport(0, 0, width, height));
}