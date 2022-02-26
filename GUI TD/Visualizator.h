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
#include <opencv2/core/types.hpp>

using namespace cv;

class Visualizator
{
public:
	Visualizator(int nbOfMonitors, GLFWmonitor** listMonitors, bool isDebuggingMode, char* pathFileProp = "", double** colorChosenList = NULL);
	~Visualizator();

	void launchSim();
private:
	void buildLayer(int nbMonitor);
	void initEnv();
	void shaderOpenGL(int nbMonitor);
	void draw(int nbMonitor);
	void activateShader(int nbMonitor);
	void loadTexture(int nbMonitor);
	void loadFileProp(char* pathFileProp);
	void applyTexture(int nbMonitor);
	char* constructStringPath(int nbMonitor);
	void setColor(double** newColorList);
	void makeContext(int nbMonitor);
	void printFps(int* frameCount, double* previousTime);

	char* pathFileProp;
	GLFWwindow** windows;
	GLFWmonitor** listMonitors;
	int nbMonitors;
	unsigned int* shaderProgram;

	double** colorChosenList;
	int Frame;
	int LayerNum;
	char* pathToLayerImg;
	int nowFrame = 0;

	GLint texLoc, rmAddLoc = NULL;
	unsigned int* texture;
	Mat* texImg;
	unsigned int* VBO, * VAO, * EBO;

	bool isDebuggingMode;

	GLchar* vertexShaderSource = "#version 330 core\n"
		"layout (location=0) in vec3 position;\n"
		"layout (location=1) in vec2 texCoords;\n"
		"out vec2 textCoords;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(position, 1.0); \n"
		"	textCoords = texCoords;\n"
		"}\0";

	GLchar* fragmentShaderSource = "#version 330 core\n"
		"uniform sampler2D tex;\n"
		"out vec4 FragColor;\n"
		"in vec2 textCoords;\n"
		"uniform float removeAdditive;"
		"void main()\n"
		"{\n"
		"	FragColor = (1.0/removeAdditive)*texture(tex, textCoords);\n"
		"}\n\0";
};