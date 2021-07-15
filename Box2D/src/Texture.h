#pragma once

#include<string>
#include<vector>

struct textureInfo
{
	std::string path;
	unsigned int m_TextureID;
	std::string type;
};

class Texture {
private:
	textureInfo vec;
public:
	Texture(std::string filePath);
	~Texture();

	void Bind() const;
	void UnBind() const;
	unsigned int CreateTexture();
};