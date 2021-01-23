#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

#define WIN_HEIGHT 600
#define WIN_WIDTH 800

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, unsigned int shaderProgram, unsigned int uniformVarLocation, GLfloat& moveDisX, GLfloat& moveDisY);
void createGLObjects(unsigned int& VBO, unsigned int& VAO, unsigned int& EBO);
void createShaderProgram(unsigned int& shaderProgram);
std::string getShaderSource(std::string fileName);
void checkShaderCompileAndLinkErrors(unsigned int ID, std::string type);

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

	unsigned int uniformVarLocation = glGetUniformLocation(shaderProgram, "updatePos");
	glUniform3f(uniformVarLocation, 0, 0, 0);
	float moveDisX = 0;
	float moveDisY = 0;

	//render loop
	while (!glfwWindowShouldClose(window)) {

		processInput(window, shaderProgram, uniformVarLocation, moveDisX, moveDisY);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}






void createGLObjects(unsigned int& vbo, unsigned int& vao, unsigned int& ebo) {
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
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
		catch (std::ifstream::failure& e) {
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

void processInput(GLFWwindow* window, unsigned int shaderProgram, unsigned int uniformVarLocation, GLfloat& moveDisX, GLfloat& moveDisY) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		moveDisY += 0.01;
		glUniform3f(uniformVarLocation, moveDisX, moveDisY, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		moveDisY -= 0.01;
		glUniform3f(uniformVarLocation, moveDisX, moveDisY, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		moveDisX += 0.01;
		glUniform3f(uniformVarLocation, moveDisX, moveDisY, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		moveDisX -= 0.01;
		glUniform3f(uniformVarLocation, moveDisX, moveDisY, 0);
	}
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
	//set dimensions for the viewport
	glViewport(0, 0, width, height);
}