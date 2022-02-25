#include "Visualizator.h"

Visualizator::Visualizator(int nbOfMonitors,GLFWmonitor** listMonitors,bool isDebuggingMode, char* pathFileProp,double** colorChosenList) {
	this->nbMonitors = nbOfMonitors;
	this->listMonitors = listMonitors;
	this->isDebuggingMode = isDebuggingMode;
	this->pathFileProp = pathFileProp;
	this->colorChosenList = colorChosenList;
}

void Visualizator::buildLayer(int nbMonitor) {
	float infoDisplay[] = {
		// Vertices			//Tex Coords		
		-1.0f, -1.0f, 0.0,	0.0f,0.0f,
		1.0f,-1.0f, 0.0,	1.0f,0.0f,
		1.0f,1.0f,0.0,		1.0f,1.0f,
		-1.0f,1.0f,0.0,		0.0f,1.0f
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 2,  // first Triangle
		0, 2, 3   // second Triangle
	};

	glGenVertexArrays(1, VAO + nbMonitor);
	glGenBuffers(1, VBO + nbMonitor);
	glGenBuffers(1, EBO + nbMonitor);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(*(VAO + nbMonitor));

	glBindBuffer(GL_ARRAY_BUFFER, *(VBO + nbMonitor));
	glBufferData(GL_ARRAY_BUFFER, sizeof(infoDisplay), infoDisplay, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(EBO + nbMonitor));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	const GLint posLoc = glGetAttribLocation(shaderProgram[nbMonitor], "position");
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(posLoc);

	// texture coord attribute
	const GLint texLoc = glGetAttribLocation(shaderProgram[nbMonitor], "texCoords");
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(texLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Visualizator::loadTexture(int nbMonitor) {
	this->texImg[nbMonitor] = Mat();
	glGenTextures(1, texture + nbMonitor);
	glBindTexture(GL_TEXTURE_2D, texture[nbMonitor]);
	// GL, by default, expects rows of pixels to be padded to a multiple of 4 bytes.A 1366 wide texture with 1 byte or 3 byte wide pixels, will not be naturally 4 byte aligned.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	try {
		this->applyTexture(nbMonitor);
	}
	catch (std::invalid_argument e) {
		throw e;
	}
}

void Visualizator::applyTexture(int nbMonitor) {
	if (!texImg[nbMonitor].empty())
		this->texImg->release();

	if (this->isDebuggingMode) {
		if (this->colorChosenList == NULL)
			throw std::invalid_argument("Impossible to debug with no chosen color...");
		this->texImg[nbMonitor] = Mat(500, 500, CV_8UC3, Scalar(colorChosenList[nbMonitor][0], colorChosenList[nbMonitor][1], colorChosenList[nbMonitor][2]));
	}
	else {
		char* pathToLoad = this->constructStringPath(nbMonitor);
		texImg[nbMonitor] = imread(pathToLoad);
		if (!texImg[nbMonitor].empty()) {
			flip(texImg[nbMonitor], texImg[nbMonitor], 0);
			cvtColor(texImg[nbMonitor], texImg[nbMonitor], COLOR_BGR2RGB);
		}
		else {
			//Error load
			char errorMsg[252];
			sprintf(errorMsg, "Impossible to load image in Simualtor: %s", pathToLoad);
			delete(pathToLoad);
			throw std::invalid_argument(errorMsg);
		}
		delete(pathToLoad);
	}
	glBindTexture(GL_TEXTURE_2D, texture[nbMonitor]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImg[nbMonitor].size[1], texImg[nbMonitor].size[0], 0, GL_RGB, GL_UNSIGNED_BYTE, texImg[nbMonitor].data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

char* Visualizator::constructStringPath(int nbMonitor) {
	char* filename = new char[252];
	sprintf(filename, "%s\\layer-%d-%d.png", this->pathToLayerImg, nbMonitor, this->nowFrame);
	return filename;
}

void Visualizator::loadFileProp(char* pathFileProp) {
	FILE* fp;
	const int numberArgs = 9;
	const char* listOption[] = { "MODE","VERTICAL_VIEWNUM","HORIZONTAL_VIEWNUM","LAYER_NUM","FRAME","BRIGHTNESS","ITERATION","LOAD_FILENAME","SAVE_FILENAME" };
	char* listArgs[numberArgs];
	for (int i = 0; i < numberArgs; i++)
		listArgs[i] = new char[128];

	fp = fopen(pathFileProp, "r");
	if (fp == NULL) {
		char errorMsg[252];
		sprintf(errorMsg, "Impossible to load property file: %s.", pathFileProp);
		throw std::invalid_argument(errorMsg);
	}

	int result;
	char argFromFile[128];
	for (int i = 0; i < numberArgs; i++) {
		result = fscanf(fp, "%s\t%s", argFromFile, listArgs[i]);
		if (result < 2 || std::strcmp(argFromFile, listOption[i]) != NULL) {
			char errorMsg[252];
			sprintf(errorMsg, "Property file not correct: %s.", pathFileProp);
			throw std::invalid_argument(errorMsg);
		}
	}
	this->LayerNum = std::strtol(listArgs[3], NULL, 10);
	this->Frame = std::strtol(listArgs[4], NULL, 10);
	this->pathToLayerImg = listArgs[numberArgs - 1];

	for (int i = 0; i < numberArgs - 1; i++)
		delete listArgs[i];

	fclose(fp);
}

void Visualizator::makeContext(int nbMonitor) {
		glfwMakeContextCurrent(windows[nbMonitor]);
}

void Visualizator::launchSim() {
	try {
		if (!isDebuggingMode)
			this->loadFileProp(pathFileProp);
		this->initEnv();
		for (int i = 0; i < this->nbMonitors; i++) {
			this->makeContext(i);
			this->shaderOpenGL(i);
			this->buildLayer(i);
			this->loadTexture(i);
		}
	}
	catch (std::runtime_error e) {
		throw e;
	}
	catch (std::invalid_argument e) {
		throw e;
	}

	glClearColor(1.0f, 1.0f, 1.0f, 0.0);

	while (true)
	{
		//Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		for (int i = this->nbMonitors - 1; i >= 0; i--) {
			this->makeContext(i);
			glUseProgram(shaderProgram[i]);
			try {
				this->applyTexture(i);
			}
			catch (std::invalid_argument e) {
				throw e;
			}
			this->activateShader(i);
			this->draw(i);
			glfwSwapBuffers(windows[i]);
		}
		
		this->nowFrame = (this->nowFrame + 1) % this->Frame;
		//Events
		glfwPollEvents();
	}
	for (int i = 0; i < this->nbMonitors; i++) {
		glDeleteBuffers(1, EBO + i);
		glDeleteVertexArrays(1, VAO + i);
		glDeleteBuffers(1, VBO + i);
	}
	this->~Visualizator();
}

Visualizator::~Visualizator() {
	delete[] shaderProgram;
	delete[] VBO;
	delete[] VAO;
	delete[] EBO;
	delete[] texture;
	delete[] texImg;
	delete[] windows;
}

void Visualizator::activateShader(int nbMonitor) {
	if (texLoc == NULL) {
		texLoc = glGetUniformLocation(this->shaderProgram[nbMonitor], "tex");
	}
	//Texture
	glUniform1i(texLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[nbMonitor]);
}

void Visualizator::draw(int nbMonitor) {
	glBindVertexArray(*(this->VAO + nbMonitor));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void Visualizator::initEnv() {
	if (!glfwInit()) {
		throw std::runtime_error("Impossible to initialize GLFW in simultor.");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//Launch nb of windows
	windows = new GLFWwindow *[nbMonitors];
	int xPos, yPos;
	for (int i = 0; i < this->nbMonitors; i++) {
		const GLFWvidmode* mode = glfwGetVideoMode(this->listMonitors[i]);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		if (i >= 1)
			this->windows[i] = glfwCreateWindow(mode->width, mode->height, "Balec du nom", this->listMonitors[i], this->windows[i - 1]);
		else
			this->windows[i] = glfwCreateWindow(mode->width, mode->height, "Balec du nom", this->listMonitors[i], NULL);
		this->makeContext(i);
		glfwGetMonitorPos(this->listMonitors[i], &xPos, &yPos);
		glfwSetWindowMonitor(this->windows[i], NULL, xPos, yPos, mode->width, mode->height, mode->refreshRate);
		if (this->windows[i] == NULL) {
			throw std::runtime_error("Impossible to create a GLFW window for visualization.");
		}
		//Glad
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			throw std::runtime_error("Impossible to initialize GLAD for visualization.");
		}
	}

	shaderProgram = new unsigned int[this->nbMonitors];

	VBO = new unsigned int[this->nbMonitors];
	VAO = new unsigned int[this->nbMonitors];
	EBO = new unsigned int[this->nbMonitors];
	
	texture = new unsigned int[this->nbMonitors];
	this->texImg = new Mat[this->nbMonitors];
}


void Visualizator::setColor(double** newColorList) {
	if (!isDebuggingMode)
		throw std::runtime_error("Impossible to setColor if not in debugging mode...");
	this->colorChosenList = newColorList;
}

void Visualizator::shaderOpenGL(int nbMonitor) {
	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

	GLchar* pointerV[1];
	pointerV[0] = vertexShaderSource;
	glShaderSource(vertexShader, 1, pointerV, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	GLchar* pointerF[1];
	pointerF[0] = fragmentShaderSource;
	glShaderSource(fragmentShader, 1, pointerF, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}
	// link shaders
	shaderProgram[nbMonitor] = glCreateProgram();
	glAttachShader(shaderProgram[nbMonitor], vertexShader);
	glAttachShader(shaderProgram[nbMonitor], fragmentShader);
	glLinkProgram(shaderProgram[nbMonitor]);
	// check for linking errors
	glGetProgramiv(shaderProgram[nbMonitor], GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram[nbMonitor], 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}