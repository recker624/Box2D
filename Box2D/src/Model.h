#pragma once

#include"Mesh.h"
#include "Shader.h"

#include<assimp/scene.h>
#include<assimp/Importer.hpp>
#include<assimp/postprocess.h>

#include<string>
#include<vector>
#include<iostream>

class Model {
private:
	//model data
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

public:
	Model(char* path);
	void Draw(Shader& shader);

private:
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* str, std::string directory)
};