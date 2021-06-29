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
	std::vector <glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texture;
};

struct Elements
{
	std::vector<int> vertices;
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

	std::vector<float> getVertexAttributes();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec2> getTexture();
	std::vector<glm::vec3> getNormal();
private:
	void readVertexAttribute();
};
