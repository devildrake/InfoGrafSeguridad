#pragma once
#include <string>

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "Shader.h"
#include <GL\glew.h>
#include <gtc\matrix_transform.hpp>
#include<glm.hpp>
#include <assimp\types.h>

class Mesh {
public:
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};
	struct Texture {
		GLuint id;
		string type;
		aiString path;
	};
	Mesh(vector<Vertex> , vector<GLuint> , vector<Texture> );
	void setupMesh();
	void Draw(Shader , GLint );
private:

	GLuint VBO, EBO, VAO;
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
};