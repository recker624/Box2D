#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<iostream>

#include"Texture.h"
#include"Renderer.h"
#include "stb_image.h"

void Texture::Bind(unsigned int num) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + num));
	GLCall(glBindTexture(GL_TEXTURE_2D, vec[num].m_TextureID));
}

void Texture::UnBind(unsigned int num) const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::push(std::string path)
{
	if(vec.empty() == false)
		vec.push_back({path, (unsigned int)vec.size()});
	else
		vec.push_back({ path, 0});
}

std::vector<textureInfo> Texture::getTexture() const
{
	return vec;
}

void Texture::CreateTexture()
{
	for (int i=0; i<vec.size(); i++)
	{
		GLCall(glGenTextures(1, &vec[i].m_TextureID));
		Bind(i);
		// set the texture wrapping parameters
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));	// set texture wrapping to GL_REPEAT (default wrapping method)
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		// set texture filtering parameters
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		// load image, create texture and generate mipmaps
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);

		unsigned char* data = stbi_load(vec[i].path.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
			GLCall(glGenerateMipmap(GL_TEXTURE_2D));
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}
}
