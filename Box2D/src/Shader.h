#pragma once
#include<string>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

class Shader {
private:
	unsigned int m_ShaderID;
	std::string filePath_1;
	std::string filePath_2;

public:
	Shader(std::string file_1, std::string file_2);
	~Shader();

	void Bind() const;
	void UnBind() const;

	void createShaderProgram();
	std::string getShaderSourceCode(std::string file);
	void checkShaderCompileAndLinkErrors(unsigned int ID, std::string type);
	unsigned int& GetShaderID();

	//set the uniform variables
	void SetUniform4f(std::string varName, glm::mat4 matrix);
	void SetUniformSampler2D(std::string varName, float value);
	void SetUniform3f(std::string varName, glm::vec3 vec);
	void SetFloat(std::string varName, float val);
};