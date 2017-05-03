#pragma once
#include <GL\glew.h>
#include<glm.hpp>
#include <gtc\matrix_transform.hpp>
using namespace glm;
#include "Shader.h"

class Object {

public:
	enum FigureType {
		cube = 0,
	};

	Object(vec3 scale, vec3 rotation, vec3 position, FigureType typef);
	~Object();

	void Draw();
	void Move(vec3 translation);
	void Rotate(vec3 rota);
	void Scale(vec3 scal);
	void Update(GLFWwindow * window);
	void Delete();
	mat4 GetModelMatrix();
	vec3 GetPosition();
	float prevFrame;
	bool start;
private:
	GLuint VBO, VAO; //EBO;
	vec3 position;
	vec3 scale;
	float incrementoRot;
	float rotacionX, rotacionY;
};
