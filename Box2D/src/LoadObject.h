#pragma once

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<glm/glm.hpp>

//takes in a file and returns a float vector containing all the vertex attributes of the object
//current support is limited to only OBJ wavefront files

struct VertexAttributes
{
	std::vector <glm::vec3> position;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texture;
};

struct Elements
{
	std::vector<int> postion;
	std::vector<int> normals;
	std::vector<int> texture;
};

class LoadObject
{
private:
	std::string fileName;
	VertexAttributes vertexAttributes;
	Elements elements;

public:
	LoadObject(std::string fileName);
	~LoadObject();

	float* getVertexAttributes();
	std::vector<glm::vec3> getPosition();
	std::vector<glm::vec2> getTexture();
	std::vector<glm::vec3> getNormal();
	int getVertexAttributeSize();
private:
	void readVertexAttribute();
};
