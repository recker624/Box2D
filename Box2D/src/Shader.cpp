#include "Shader.h"
#include "Renderer.h"

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

/*
* Takes in a file with shader source code and creates a shader program object using it
*/

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

//Compile and link the individual shaders and combine them to make shader program
void Shader::createShaderProgram()
{
	unsigned int vertexShader, fragmentShader;
	GLCall(vertexShader = glCreateShader(GL_VERTEX_SHADER));
	GLCall(fragmentShader = glCreateShader(GL_FRAGMENT_SHADER));

	//attach shader with the source code
	std::string vertexStrCode = getShaderSourceCode(filePath_1);
	std::string fragmentStrCode = getShaderSourceCode(filePath_2);
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

//returns the source code of the shader files as strings
std::string Shader::getShaderSourceCode(std::string fileName)
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

unsigned int& Shader::GetShaderID() { return m_ShaderID; }

void Shader::SetUniform4f(std::string varName, glm::mat4 matrix)
{
	GLCall(glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, varName.c_str()), 1, GL_FALSE, glm::value_ptr(matrix)));
}

void Shader::SetUniformSampler2D(std::string varName, float value)
{
	GLCall(glUniform1i(glGetUniformLocation(m_ShaderID, varName.c_str()), value));
}

void Shader::SetUniform3f(std::string varName, glm::vec3 value)
{
	GLCall(glUniform3f(glGetUniformLocation(m_ShaderID, varName.c_str()), value.x, value.y, value.z));
}

void Shader::SetFloat(std::string varName, float val)
{
	GLCall(glUniform1f(glGetUniformLocation(m_ShaderID, varName.c_str()), val));
}