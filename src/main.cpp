//GLEW
#define GLEW_STATIC
#include <GL\glew.h>
//GLFW
#include <GLFW\glfw3.h>
#include <iostream>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "..\Shader.h"
#include <SOIL.h>
#include "..\Mesh.h"
#include "..\Model.h"
#include "..\Object.h"

using namespace glm;
using namespace std;
const GLint WIDTH = 800, HEIGHT = 600;
bool WIDEFRAME = false;
float rotacionY = 0.0f;
float rotacionX = 0.0f;
//float gradosRot = 0;
//float aumentoRot;
bool aumentarRotRight, aumentarRotLeft, aumentarUp, aumentarDown;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_callback(GLFWwindow* window,double xPos, double yPos);
void scroll_callback(GLFWwindow* window,double xOffset, double yOffset);
int modelToDraw = 1;

#pragma region Clase Camera

class Camera
{
public:
	Camera(vec3 position, vec3 direction, GLfloat sensitivity, GLfloat fov);
	void DoMovement(GLFWwindow * window);
	void MouseMove(GLFWwindow* window, double xpos, double ypos);
	void MouseScroll(GLFWwindow* window, double xScroll, double yScroll);
	mat4 LookAt();
	GLfloat GetFOV();


private:
	vec3 cameraPos;
	vec3 cameraFront;
	vec3 cameraUp;
	GLfloat Deltatime;
	GLfloat Lastframe;
	GLfloat LastMx;
	GLfloat LastMy;
	GLfloat Sensitivity;
	GLboolean firstMouse;
	GLfloat PITCH;
	GLfloat YAW;
	GLfloat FOV;
};

Camera::Camera(vec3 position, vec3 direction, GLfloat sensitivity, GLfloat fov)
{
	Lastframe = 0;
	cameraPos = position;	
	cameraFront = direction;
	Sensitivity = sensitivity;
	FOV = fov;

	vec3 directionX = normalize(vec3(0,direction.y,direction.z));
	vec3 z(0,0,1);
	vec3 directionY = normalize(vec3(direction.x, 0, direction.z));


	PITCH = 90 - degrees(acos(dot(vec3(0,1,0),directionX)));
	YAW = 90 - degrees(acos(dot(directionY, z)));
}

void Camera::DoMovement(GLFWwindow * window) {

	Deltatime = glfwGetTime() - Lastframe;
	Lastframe = glfwGetTime();

	float cameraSpeed = 0.5f;

		bool backwards = glfwGetKey(window, GLFW_KEY_W);
		bool forward = glfwGetKey(window, GLFW_KEY_S);
		bool left = glfwGetKey(window, GLFW_KEY_A);
		bool right = glfwGetKey(window, GLFW_KEY_D);
		bool goFaster = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);

		if (goFaster) {
			cameraSpeed = 20;
		}

		if (forward) {
			cameraPos += normalize(cameraFront)*cameraSpeed*Deltatime;
		}
		else if (backwards) {
			cameraPos -= normalize(cameraFront)*cameraSpeed*Deltatime;
		}
		if (left) {
			cameraPos -= (cross(vec3(0, 1, 0), (normalize(cameraFront))))*cameraSpeed*Deltatime;
		}
		else if (right) {
			cameraPos += (cross(vec3(0, 1, 0), (normalize(cameraFront))))*cameraSpeed*Deltatime;
		}
}

void Camera::MouseMove(GLFWwindow* window, double xpos, double ypos) {
		double offsetX, offsetY;
		if (!firstMouse) {
			firstMouse = true;
			LastMx = xpos;
			LastMy= ypos;
		}
		offsetX = xpos - LastMx;
		offsetY = ypos - LastMy;

		LastMx = xpos;
		LastMy = ypos;



		offsetX *= Sensitivity;
		offsetY *= Sensitivity;

		//cout << "ClaseOffsetX " << offsetX << endl;
		//cout << "ClaseOffsetY " << offsetY << endl;

		YAW += offsetX;
		PITCH += offsetY;

		PITCH = clamp(PITCH, -89.9f, 89.9f);
		YAW = mod(YAW, 360.0f);


		vec3 dir;

		dir.x = cos(glm::radians(YAW)) * cos(glm::radians(PITCH));
		dir.y = sin(glm::radians(PITCH));
		dir.z = sin(glm::radians(YAW)) * cos(glm::radians(PITCH));

		cameraFront = normalize(dir);
	}

void Camera::MouseScroll(GLFWwindow* window, double xScroll, double yScroll) {
			FOV -= yScroll ;
			FOV = clamp(FOV, 1.f, 45.f);
}
mat4 Camera::LookAt() {
		vec3 zaxis = normalize(cameraFront);
		vec3 xaxis = normalize(cross(vec3(0, 1, 0), zaxis));
		vec3 yaxis = cross(zaxis, xaxis);
		mat4 rotation, translation;
		rotation[0][0] = xaxis.x;
		rotation[1][0] = xaxis.y;
		rotation[2][0] = xaxis.z;
		rotation[0][1] = yaxis.x;
		rotation[1][1] = yaxis.y;
		rotation[2][1] = yaxis.z;
		rotation[0][2] = zaxis.x;
		rotation[1][2] = zaxis.y;
		rotation[2][2] = zaxis.z;
		rotation[3][3] = 1;

		translation[3][0] = -cameraPos.x;
		translation[3][1] = -cameraPos.y;
		translation[3][2] = -cameraPos.z;
		translation[3][3] = 1;

		mat4 res = rotation*translation;

		return res;

}
GLfloat Camera::GetFOV() {
	return FOV;
}
#pragma endregion

void DrawVao(GLuint programID, GLuint VAO) {
	//establecer el shader
	glUseProgram(programID);

	//pitar el VAO
	glBindVertexArray(VAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	

}

void PrintAMatrix4(mat4 a) {

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << "a[" << i << "][" << j << "] = " << a[i][j] << endl;

		}
	}
}

void PrintAndCompareMatrix4(mat4 a, mat4 b) {

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << "a[" << i << "][" << j << "] = " << a[i][j] << endl;
			cout << "b[" << i << "][" << j << "] = " << b[i][j] << endl;
		}
	}
}

mat4 GenerateModelMatrix(vec3 aTranslation, vec3 aRotation, vec3 CubesPosition, float aRot) {
	mat4 temp;
	temp = translate(temp, aTranslation);
	temp = translate(temp, CubesPosition);
	temp = rotate(temp, radians(aRot), aRotation);

	return temp;
}

Camera camara(vec3(0, 0, 3), normalize(vec3(0, 0, -3)), 0.04f, 45.0f);

void main() {



	//initalize GLFW

	if (!glfwInit())
		exit(EXIT_FAILURE);

	//set GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);


	//create a window


	GLFWwindow* window;

	window = glfwCreateWindow(WIDTH, HEIGHT, "Primera ventana", nullptr, nullptr);
	if (!window) {
		cout << "Error al crear la ventana" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	//set GLEW and inicializate

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit()) {
		std::cout << "Error al inicializar glew" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	int screenWithd, screenHeight;
	glfwGetFramebufferSize(window, &screenWithd, &screenHeight);

	//set function when callback

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_callback); 
	glfwSetScrollCallback(window, scroll_callback);




	//set windows and viewport
	glViewport(0, 0, screenWithd, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT);

	//fondo azul
	glClearColor(0.0, 0.0, 1.0, 1.0);

	//cargamos los shader

	Shader shader = Shader("./src/textureVertex3d.vertexshader", "./src/textureFragment3d.fragmentshader");

	GLfloat VertexBufferObject[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f , -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f ,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f ,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f , -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f ,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f ,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f ,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f ,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f , -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f , -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f , -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f ,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f , -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f , -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f , -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f ,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f ,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f ,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	vec3 CubesPositionBuffer[] = {
		vec3(0.0f ,  0.0f,  0.0f),
		vec3(2.0f ,  5.0f, -15.0f),
		vec3(-1.5f, -2.2f, -2.5f),
		vec3(-3.8f, -2.0f, -12.3f),
		vec3(2.4f , -0.4f, -3.5f),
		vec3(-1.7f,  3.0f, -7.5f),
		vec3(1.3f , -2.0f, -2.5f),
		vec3(1.5f ,  2.0f, -2.5f),
		vec3(1.5f ,  0.2f, -1.5f),
		vec3(-1.3f,  1.0f, -1.5f)
	};

	Object objectInstance(vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), Object::cube);

	//Numbuffer = cantidad de buffers a generar;

	//Borrado
	//glDeleteBuffers(GLsizei n, const GLuint* ids);

	//-VBO

	//EBO

	//GLuint IndexBufferObject[]{
	//	0,1,2,
	//	1,3,0 };

	//GLuint IndexBufferObject[]{
	//	2,0,3,
	//	2,1,0
	//};




	//-EBO


	// Definir el EBO

	// Crear los VBO, VAO y EBO

	//GLuint VAO, /*EBO,*/ VBO;
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO); {

	//	glGenBuffers(1, &VBO);
	//	//Se enlaza el buffer para poder usarlo
	//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//	//Se pasan los datos
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBufferObject), VertexBufferObject, GL_DYNAMIC_DRAW);


	///*	glGenBuffers(1, &EBO);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IndexBufferObject), IndexBufferObject, GL_DYNAMIC_DRAW);
	//	*/

	//	//Propiedades

	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
	//	glEnableVertexAttribArray(0);

	//	glVertexAttribPointer(1, 0, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	//	glEnableVertexAttribArray(1);

	//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	//	glEnableVertexAttribArray(2);

	//	//LIMPIA LOS BUFFERS DE VERTICES

	//	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//}glBindVertexArray(0);



	//reservar memoria para el VAO, VBO y EBO



	//Establecer el objeto
	//Declarar el VBO y el EBO

	//Enlazar el buffer con openGL

	//Establecer las propiedades de los vertices

	//liberar el buffer

	//liberar el buffer de vertices


	//GLint loc = glGetUniformLocation(shader.Program, "time");

	//Se carga una textura
	//GLuint texture1;


	//glGenTextures(1, &texture1);
	//glBindTexture(GL_TEXTURE_2D, texture1);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//int widthTex, heightTex;
	//unsigned char* image = SOIL_load_image("./src/texture.png", &widthTex, &heightTex, 0, SOIL_LOAD_RGB);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthTex, heightTex, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//SOIL_free_image_data(image);

	////Se libera el cargador de texturas
	//glBindTexture(GL_TEXTURE_2D, 0);

//	aumentoRot = 0.05f;
	
	//Se habilita el 3D
	glEnable(GL_DEPTH_TEST);


	//Se cargan los modelos 3D
	//Model ara�aModel;
	//ara�aModel.loadModel("./src/spider/spider.obj");

	//Model boxModel;
	//boxModel.loadModel("./src/spider/box.obj");

	//Model wusonModel("./src/spider/WusonObj.obj");

	GLint matrizDefID;

	//bucle de dibujado

	while (!glfwWindowShouldClose(window))
	{
		camara.DoMovement(window);

		mat4 cam; //Vista
		mat4 proj; //Proyeccion

//		if (gradosRot > 360 || gradosRot < -360) {
//			gradosRot = 0;
//		}

		glfwPollEvents();

		matrizDefID = glGetUniformLocation(shader.Program, "matrizDefinitiva");

		//Establecer el color de fondo
		//glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//GLint locTex = glGetUniformLocation(shader.Program, "ourTexture");
		//GLint locTex2 = glGetUniformLocation(shader.Program, "ourTexture2");
		//GLint mixID = glGetUniformLocation(shader.Program, "mixStuff");



		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture1);
		//glUniform1i(locTex,0);

	/*	glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(locTex2, 1);*/
		
		//glBindVertexArray(VAO);

		//glBindVertexArray(objectInstance.VAO);


		//if (aumentarRotLeft) {
		//	rotacionY-=aumentoRot;
		//}
		//else if (aumentarRotRight) {
		//	rotacionY+= aumentoRot;
		//}

		//if (aumentarUp) {
		//	rotacionX -= aumentoRot;
		//}
		//else if (aumentarDown) {
		//	rotacionX += aumentoRot;
		//}

		proj = perspective(radians(camara.GetFOV()), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		cam = camara.LookAt();

		//Model Matrix

		mat4 modelMatrix;

		//float rotot = 0;
		//modelMatrix = GenerateModelMatrix(vec3(0.0f), vec3(1, 0.5f, 0), CubesPositionBuffer[0], rotot);
		//modelMatrix = scale(modelMatrix, glm::vec3(0.2f));

		modelMatrix = objectInstance.GetModelMatrix();

			mat4 matrizDefinitiva;
			
			matrizDefinitiva = proj*camara.LookAt()*modelMatrix;
			
			glUniformMatrix4fv(matrizDefID, 1, GL_FALSE, glm::value_ptr(matrizDefinitiva));

			shader.USE();

			objectInstance.Draw();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	// liberar la memoria de los VAO, EBO y VBO

	// Terminate GLFW, clearing any resources allocated by GLFW.
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
mat4 miLookAt(vec3 camPos, vec3 whereToLook, vec3 cameraUp) {

	vec3 zaxis = normalize(  camPos- whereToLook);

	vec3 xaxis = normalize(cross(vec3(0, 1, 0), zaxis));
	vec3 yaxis = cross(zaxis, xaxis);

	mat4 rotation, translation;

	rotation[0][0] = xaxis.x;
	rotation[1][0] = xaxis.y;
	rotation[2][0] = xaxis.z;
	rotation[0][1] = yaxis.x;
	rotation[1][1] = yaxis.y;
	rotation[2][1] = yaxis.z;
	rotation[0][2] = zaxis.x;
	rotation[1][2] = zaxis.y;
	rotation[2][2] = zaxis.z;
	rotation[3][3] = 1;

	translation[3][0] = -camPos.x;
	translation[3][1] = -camPos.y;
	translation[3][2] = -camPos.z;
	translation[3][3] = 1;

	return rotation*translation;
}

void cursor_callback(GLFWwindow* window, double xPos, double yPos){
		camara.MouseMove(window, xPos, yPos);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camara.MouseScroll(window, xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	//TODO, comprobar que la tecla pulsada es escape para cerrar la aplicaci�n y la tecla w para cambiar a modo widwframe
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_UP&&action == GLFW_PRESS) {
		aumentarUp = true;
	}

	if (key == GLFW_KEY_UP&&action == GLFW_RELEASE) {
		aumentarUp = false;
	}

	if (key == GLFW_KEY_DOWN&&action == GLFW_PRESS) {
		aumentarDown = true;
	}

	if (key == GLFW_KEY_DOWN&&action == GLFW_RELEASE) {
		aumentarDown = false;
	}

	if (key == GLFW_KEY_RIGHT&&action == GLFW_PRESS) {
		aumentarRotRight = true;
	}
	else if (key == GLFW_KEY_RIGHT&&action == GLFW_RELEASE) {
		aumentarRotRight = false;
	}

	if (key == GLFW_KEY_LEFT&&action == GLFW_PRESS) {
		aumentarRotLeft = true;
	}
	else if (key == GLFW_KEY_LEFT&&action == GLFW_RELEASE) {
		aumentarRotLeft = false;
	}

	if (key == GLFW_KEY_1&&action == GLFW_PRESS) {
		modelToDraw = 1;
	}
	else if (key == GLFW_KEY_2&&action == GLFW_PRESS) {
		modelToDraw = 2;
	}
	else if (key == GLFW_KEY_3&&action == GLFW_PRESS) {
		modelToDraw = 3;
	}

}