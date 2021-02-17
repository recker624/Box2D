#include "Shader.h"
#include "Renderer.h"

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>


Shader::Shader(std::string file_1, std::string file_2)
	: filePath_1(file_1), filePath_2(file_2), m_ShaderID(0) 
{
	createShaderProgram();
}

Shader::~Shader() 
{
	GLCall(glDeleteProgram(m_ShaderID));
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_ShaderID));
}

void Shader::UnBind() const
{
	GLCall(glUseProgram(0));
}

void Shader::createShaderProgram()
{
	unsigned int vertexShader, fragmentShader;
	GLCall(vertexShader = glCreateShader(GL_VERTEX_SHADER));
	GLCall(fragmentShader = glCreateShader(GL_FRAGMENT_SHADER));

	//attach shader with the source code
	std::string vertexStrCode = getShaderSource(filePath_1);
	std::string fragmentStrCode = getShaderSource(filePath_2);
	const char* vShaderSourceCode = vertexStrCode.c_str();
	const char* fShaderSourceCode = fragmentStrCode.c_str();

	GLCall(glShaderSource(vertexShader, 1, &vShaderSourceCode, NULL));
	GLCall(glCompileShader(vertexShader));
	checkShaderCompileAndLinkErrors(vertexShader, "VERTEX");

	GLCall(glShaderSource(fragmentShader, 1, &fShaderSourceCode, NULL));
	GLCall(glCompileShader(fragmentShader));
	checkShaderCompileAndLinkErrors(fragmentShader, "FRAGMENT");

	//create the shader program by attaching the shaders 
	GLCall(m_ShaderID = glCreateProgram());
	GLCall(glAttachShader(m_ShaderID, vertexShader));
	GLCall(glAttachShader(m_ShaderID, fragmentShader));
	GLCall(glLinkProgram(m_ShaderID));
	checkShaderCompileAndLinkErrors(m_ShaderID, "PROGRAM");
}

std::string Shader::getShaderSource(std::string fileName)
{
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

void Shader::checkShaderCompileAndLinkErrors(unsigned int ID, std::string type)
{
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

unsigned int& Shader::GetShaderID(){ return m_ShaderID; }