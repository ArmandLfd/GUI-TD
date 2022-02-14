#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/core/core.hpp> //Opencv
#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace cv;

ref class Simulator
{
public:
	Simulator(char* pathFile);
	~Simulator();

	void launchSim();

private:
	glm::vec3 processInput(GLFWwindow* windows,glm::vec3 eye);
	void buildDisplay(int depth);
	void activateShader(glm::vec3 eye,int layerHandled);
	void loadTexture();
	glm::vec3 init();
	void loadFileProp(char* pathFileProp);
	void shaderOpenGL();
	glm::mat4 getProjection(float fov, float ratio, float near, float far);
	void applyTexture(int layerHandled);
	void draw(int layerHandled);

	char* constructStringPath(int layerHandled);

	glm::vec3 rotateX(glm::vec3 v, glm::vec3 origin, double angle);
	glm::vec3 rotateY(glm::vec3 v, glm::vec3 origin, double angle);


	GLFWwindow* window;
	int winsize = 512;

	unsigned int *texture;
	GLint MLoc, VLoc, PLoc, texLoc = NULL;
	Mat* texImg;

	GLchar* fragmentShaderSource = "#version 330 core\n"
		"uniform sampler2D tex;\n"
		"out vec4 FragColor;\n"
		"in vec2 textCoords;"
		"void main()\n"
		"{\n"
		"   FragColor = texture(tex, textCoords);\n"
		"   //FragColor = vec4(0.13,0.35,0.45,1.0);\n"
		"}\n\0";
	GLchar* vertexShaderSource = "#version 330 core\n"
		"uniform mat4 M;\n"
		"uniform mat4 V;\n"
		"uniform mat4 P;\n"
		"layout (location=0) in vec3 position;\n"
		"layout (location=1) in vec2 texCoords;\n"
		"out vec2 textCoords;\n"
		"void main()\n"
		"{\n"
		"	//vec4 frag_coord = M*vec4(position, 1.0);\n"
		"	gl_Position = P * V * M * vec4(position, 1.0); \n"
		"	//gl_Position = vec4(position, 1.0); \n"
		"	textCoords = texCoords;\n"	
		"}\0"; 

	float offsetDepth = 0.1;
	unsigned int shaderProgram;
	unsigned int *VBO, *VAO, *EBO;

	double px, py = 0;
	double mouseSensitivity = 0.5;

	int Frame;
	int LayerNum;
	char* pathToLayerImg;
	int nowFrame = 0;

	double maxPitch = 160.0;
	double maxYaw = 180;
	double yaw = 0;
	double pitch = 0;
};