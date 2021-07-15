#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<iostream>

#include"Texture.h"
#include"Renderer.h"
#include "stb_image.h"

Texture::Texture(std::string filePath) {
	vec.path = filePath;
}

Texture::~Texture() {

}

void Texture::Bind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, vec.m_TextureID));
}

void Texture::UnBind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

//call this after all you have added all the textures using push()
unsigned int Texture::CreateTexture()
{
	GLCall(glGenTextures(1, &vec.m_TextureID));
	Bind();

	// set the texture wrapping parameters
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));	// set texture wrapping to GL_REPEAT (default wrapping method)
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	// set texture filtering parameters
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(vec.path.c_str(), &width, &height, &nrChannels, 0);
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

	return vec.m_TextureID;
}
