#include"LoadObject.h"

LoadObject::LoadObject(std::string fileName) : fileName(fileName) 
{
	readVertexAttribute();
}

LoadObject::~LoadObject()
{

}

void LoadObject::readVertexAttribute()
{
	
	std::ifstream in(fileName, std::ifstream::in);
	if (!in)
	{
		std::cerr << "ERROR OPENING FILE : " << fileName << std::endl;
		exit(1);
	}

	//read line by line and storre in a string
	std::string line;
	while (getline(in, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			std::istringstream in_str(line.substr(2));
			glm::vec3 temp;
			in_str >> temp.x >> temp.y >> temp.z;
			vertexAttributes.vertices.push_back(temp);

		}
		else if (line.substr(0, 2) == "vt")
		{
			std::istringstream in_str(line.substr(2));
			glm::vec2 temp;
			in_str >> temp.x >> temp.y;
			vertexAttributes.texture.push_back(temp);
		}
		else if (line.substr(0, 2) == "vn")
		{
			std::istringstream in_str(line.substr(2));
			glm::vec3 temp;
			in_str >> temp.x >> temp.y >> temp.z;
			vertexAttributes.normals.push_back(temp);
		}
		else if (line.substr(0, 2) == "f ")
		{
			std::string tempStr = line.substr(2);
			std::string str;
			int temp[9]{ 0 };

			for (char ch : tempStr)
			{
				if (ch != '/')
					str.push_back(ch);
				else
					str.push_back(' ');
			}

			std::istringstream in_str(str);

			for (int i = 0; i < 9; i++)
			{
				in_str >> temp[i];

				if (i % 3 == 0)
					elements.vertices.push_back(temp[i] - 1);
				else if (i % 3 == 1)
					elements.texture.push_back(temp[i] - 1);
				else if (i % 3 == 2)
					elements.normals.push_back(temp[i] - 1);
			}
		}
	}
}

//returns a normal array of floats to the main application containing all the vertex attributes 
std::vector<float> LoadObject::getVertexAttributes()
{
	std::vector<float> masterArray;

	std::vector<glm::vec3> vertices = getVertices();
	std::vector<glm::vec3> normals = getNormal();
	std::vector<glm::vec2> textures = getTexture();

	for (int i = 0; i < elements.vertices.size(); i++)
	{
		masterArray.push_back(vertices[i].x);
		masterArray.push_back(vertices[i].y);
		masterArray.push_back(vertices[i].z);
		masterArray.push_back(normals[i].x);
		masterArray.push_back(normals[i].y);
		masterArray.push_back(normals[i].z);
		masterArray.push_back(textures[i].x);
		masterArray.push_back(textures[i].y);
	}
	return masterArray;
}

std::vector<glm::vec3> LoadObject::getVertices()
{
	std::vector<glm::vec3> temp_vertiecs;
	for (int i = 0; i < elements.vertices.size(); i++)
	{
		temp_vertiecs.push_back(vertexAttributes.vertices[elements.vertices[i]]);
	}
	return temp_vertiecs;
}

std::vector<glm::vec2> LoadObject::getTexture()
{
	std::vector<glm::vec2> temp_texture;
	for (int i=0; i<elements.texture.size(); i++)
	{
		temp_texture.push_back(vertexAttributes.texture[elements.texture[i]]);
	}
	return temp_texture;
}

std::vector<glm::vec3> LoadObject::getNormal()
{
	std::vector<glm::vec3> temp_normal;
	for (int i = 0; i < elements.normals.size(); i++)
	{
		temp_normal.push_back(vertexAttributes.normals[elements.normals[i]]);
	}
	return temp_normal;
}