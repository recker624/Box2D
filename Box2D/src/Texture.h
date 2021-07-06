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
	std::vector<textureInfo> vec;
public:

	void Bind(unsigned int num) const;
	void UnBind(unsigned int num) const;

	void push(std::string path);
	void CreateTexture();
	std::vector<textureInfo> getTexture() const;
};