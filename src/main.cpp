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
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_callback(GLFWwindow* window,double xPos, double yPos);
void scroll_callback(GLFWwindow* window,double xOffset, double yOffset);

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
	glClearColor(0.0f,0.0f,0.0f,1.0f);

	//cargamos los shader

	//Shader shader = Shader("./src/textureVertex3d.vertexshader", "./src/textureFragment3d.fragmentshader");

	Shader shaderSimple = Shader("./src/textureVertex3d.vertexshader", "./src/textureFragment3d.fragmentshader");
	Shader shaderPhong = Shader("./src/textureVertex3d.vertexshader", "./src/textureFragment3d.fragmentshader");

	Object cajaControlable(vec3(0.5f,0.5f,0.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), Object::cube);
	Object cajaFija(vec3(0.1f, 0.1f, 0.1f), vec3(0.0f, 0.0f, 0.0f), vec3(4.0f, 0.0f, 0.0f), Object::cube);

	
	//Se habilita el buffer de profundidad
	glEnable(GL_DEPTH_TEST);

	GLint matrizDefID;

	//Bucle de dibujado

	while (!glfwWindowShouldClose(window))
	{
		camara.DoMovement(window);

		mat4 cam; //Vista
		mat4 proj; //Proyeccion

		glfwPollEvents();

		//Establecer el color de fondo (establecido mas arriba)
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

		//Se calcula la matriz Proyeccion
		proj = perspective(radians(camara.GetFOV()), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		//Se calcula la matriz vista con camara.LookAt();
		cam = camara.LookAt();

		//Model Matrix

		mat4 modelMatrix;

		//Matriz que tendra todas las transformaciones
		mat4 matrizDefinitiva;



		//Se actualiza la caja controlable
		cajaControlable.Update(window);
		
		//Se busca la matriz definitiva dentro del shader de Phong
		matrizDefID = glGetUniformLocation(shaderPhong.Program, "matrizDefinitiva");

		//Se asigna la matriz moedlo de la caja controlable a la variable modelMatrix
		modelMatrix = cajaControlable.GetModelMatrix();

		//Se usa el programa del shader de phong
			shaderPhong.USE();

		//Se genera la matriz definitiva utilizando la model matrix del cubo controlable
			matrizDefinitiva = proj*camara.LookAt()*modelMatrix;
			
			//Se actualiza la matriz Definitiva del shader
			glUniformMatrix4fv(matrizDefID, 1, GL_FALSE, glm::value_ptr(matrizDefinitiva));

			//Se dibuja el cubo controlable
			cajaControlable.Draw();

			//Se repite el proceso para el cubo fijo, pero usando el shader simple y haciendo uso de la matriz modelo del cubo fijo, el cual no se actualiza
			matrizDefID = glGetUniformLocation(shaderSimple.Program, "matrizDefinitiva");
			shaderSimple.USE();
			modelMatrix = cajaFija.GetModelMatrix();
			matrizDefinitiva = proj*camara.LookAt()*modelMatrix;
			glUniformMatrix4fv(matrizDefID, 1, GL_FALSE, glm::value_ptr(matrizDefinitiva));
			cajaFija.Draw();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	// liberar la memoria de los VAO, EBO y VBO

	// Terminate GLFW, clearing any resources allocated by GLFW.
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	cajaFija.Delete();
	cajaControlable.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

void cursor_callback(GLFWwindow* window, double xPos, double yPos){
		camara.MouseMove(window, xPos, yPos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camara.MouseScroll(window, xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	//TODO, comprobar que la tecla pulsada es escape para cerrar la aplicación y la tecla w para cambiar a modo widwframe
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
