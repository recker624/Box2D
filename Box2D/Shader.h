#pragma once
#include<string>



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
	std::string getShaderSource(std::string file);
	void checkShaderCompileAndLinkErrors(unsigned int ID, std::string type);
	unsigned int& GetShaderID();
};