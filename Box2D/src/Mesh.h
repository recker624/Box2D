#pragma once

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<string>
#include<vector>
#include "Shader.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture {
	unsigned int ID;
	std::string type;
	std::string path;
};

class Mesh {
public:
	std::vector<Vertex>			vertices;
	std::vector<unsigned int>	indices;
	std::vector<Texture>		textures;

private:
	unsigned int VAO, VBO, EBO;

public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader& shader);

private:
	void setupMesh();
};