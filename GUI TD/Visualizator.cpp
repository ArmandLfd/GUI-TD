#include "Visualizator.h"

Visualizator::Visualizator(int nbOfMonitors,GLFWmonitor** listMonitors,bool isDebuggingMode, char* pathFileProp,double** colorChosenList,double factorRmAdd,int width,int height,bool isVideoMode) {
	this->nbMonitors = nbOfMonitors;
	this->sizeOfMonitors = sizeOfMonitors;
	this->listMonitors = listMonitors;
	this->isDebuggingMode = isDebuggingMode;
	this->pathFileProp = pathFileProp;
	this->colorChosenList = colorChosenList;
	if (factorRmAdd == -1){
		//this->factorRmAdd = (double)nbOfMonitors;
		this->factorRmAdd = 1.0;
	}
	else
		this->factorRmAdd = factorRmAdd;
	this->widthWin = width;
	this->heightWin = height;
	this->isVideoMode = isVideoMode;

	mustRun = true;
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
	//this->texImg[getIndexInTexImg(nbMonitor)] = Mat();
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

int Visualizator::getIndexInTexImg(int nbMonitor) {
	return (nbOfFramesCanBeLoaded == 0) ? nbMonitor : nbMonitor + nbMonitors * isVideoMode * (frameVideo%nbOfFramesCanBeLoaded);
}

void Visualizator::applyTexture(int nbMonitor) {
	int idxForTexImg = getIndexInTexImg(nbMonitor);
	if ( !texImg[idxForTexImg].empty() && 
		(this->Frame > 1 || 
			(idxForTexImg == 0 && (frameVideo+nbMonitor) != 0))) {
		for (int i = 0; i < nbMonitors * (1 + isVideoMode * nbOfFramesCanBeLoaded); i++) {
			this->texImg[i].release();
		}
	}

	if (this->isDebuggingMode) {
		if (this->colorChosenList == NULL)
			throw std::invalid_argument("Impossible to debug with no chosen color...");
		this->texImg[idxForTexImg] = Mat(500, 500, CV_8UC3, Scalar(colorChosenList[nbMonitor][0], colorChosenList[nbMonitor][1], colorChosenList[nbMonitor][2]));
	}
	else {
		if (this->Frame > 1 || texImg[idxForTexImg].empty()) {
			char* pathToLoad = this->constructStringPath(nbMonitor);
			texImg[idxForTexImg] =  imread(pathToLoad, IMREAD_COLOR);
			delete(pathToLoad);
			if (!texImg[idxForTexImg].empty()) {
				flip(texImg[idxForTexImg], texImg[idxForTexImg], 0);
				cvtColor(texImg[idxForTexImg], texImg[idxForTexImg], COLOR_BGR2RGB);
			}
			else {
				//Error load
				char errorMsg[252];
				sprintf(errorMsg, "Impossible to load image in visualizator:\n %s", pathToLoad);
				throw std::invalid_argument(errorMsg);
			}
			if (resolutionImage) {
				this->setWindowsSize(-2,-2);
				resolutionImage = false;
			}
		}
	}
	glBindTexture(GL_TEXTURE_2D, texture[nbMonitor]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImg[idxForTexImg].size[1], texImg[idxForTexImg].size[0], 0, GL_RGB, GL_UNSIGNED_BYTE, texImg[idxForTexImg].data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

char* Visualizator::constructStringPath(int nbMonitor) {
	char* filename = new char[252];
	if(!isVideoMode)
		sprintf(filename, "%s\\layer-%d-%d.png", this->pathToLayerImg, nbMonitor, this->nowFrame);
	else
		sprintf(filename, "%s\\frameVideo-%d\\layer-%d-%d.png", this->pathToFrameDir, this->frameVideo,nbMonitor, this->nowFrame);
	return filename;
}

void Visualizator::loadFileProp(char* pathFileProp) {
	FILE* fp;
	const int numberArgs = 11;
	const char* listOption[] = { "MODE","VERTICAL_VIEWNUM","HORIZONTAL_VIEWNUM","LAYER_NUM","FRAME","BRIGHTNESS","ITERATION","LOAD_FILENAME","SAVE_FILENAME","FRAME_DIR","FRAME_VIDEO"};
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
	for (int i = 0; i < ((isVideoMode) ? numberArgs : numberArgs-2); i++) {
		result = fscanf(fp, "%s\t%s", argFromFile, listArgs[i]);
		if (result < 2 || std::strcmp(argFromFile, listOption[i]) != NULL) {
			char errorMsg[252];
			sprintf(errorMsg, "Property file not correct: %s.", pathFileProp);
			throw std::invalid_argument(errorMsg);
		}
	}
	this->LayerNum = std::strtol(listArgs[3], NULL, 10);
	this->Frame = std::strtol(listArgs[4], NULL, 10);
	this->pathToLayerImg = listArgs[8];
	this->pathToFrameDir = (isVideoMode) ? listArgs[9] : NULL;
	this->framesVideo = (isVideoMode) ? std::strtol(listArgs[10],NULL,10) : 1;

	for (int i = 0; i < numberArgs; i++) {
		if(i != 8 && i != 9)
			delete listArgs[i];
	}

	fclose(fp);
}

void Visualizator::setFactorRmAdd(double newFact) {
	this->factorRmAdd = newFact;
}

void Visualizator::makeContext(int nbMonitor) {
	glfwMakeContextCurrent(windows[nbMonitor]);
}

void Visualizator::printFps(int* frameCount,double* previousTime) {
	// Measure speed
	double currentTime = glfwGetTime();
	// If a second has passed.
	if (currentTime - *previousTime >= 1.0)
	{
		// Display the frame count here any way you want.
		printf("FPS: %i\n", *frameCount);

		*frameCount = 0;
		*previousTime = currentTime;
	}
}

void Visualizator::setRunFalse() {
	mustRun = false;
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

	int frameCount = 0;
	double initTime = glfwGetTime();
	while (mustRun)
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
		frameCount++;
		this->printFps(&frameCount,&initTime);
		this->nowFrame = (this->nowFrame + 1) % this->Frame;
		this->frameVideo = (this->frameVideo + 1) % this->framesVideo;
		//Events
		glfwPollEvents();
	}
	this->~Visualizator();
}

Visualizator::~Visualizator() {
	for (int i = 0; i < this->nbMonitors; i++) {
		this->makeContext(i);
		glDeleteBuffers(1, EBO + i);
		glDeleteVertexArrays(1, VAO + i);
		glDeleteBuffers(1, VBO + i);
		texImg[i].release();
		glfwDestroyWindow(this->windows[i]);
		if (isDebuggingMode)
			delete colorChosenList[i];
	}

	delete listMonitors;
	if (isDebuggingMode)
		delete[] colorChosenList;

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
		rmAddLoc = glGetUniformLocation(this->shaderProgram[nbMonitor], "removeAdditive");
	}
	//Texture
	glUniform1i(texLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[nbMonitor]);
	//RemoveAdditive
	glUniform1f(rmAddLoc, (GLfloat)this->factorRmAdd);
}

void Visualizator::draw(int nbMonitor) {
	glBindVertexArray(*(this->VAO + nbMonitor));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Visualizator::setWindowsSize(int width, int height) {
	int xPos, yPos;
	const GLFWvidmode* mode;
	if (width == -2 && height == -2) {
		width = texImg[0].size[1];
		height = texImg[0].size[0];
	}
	for (int i = 0; i < this->nbMonitors; i++) {
		this->makeContext(i);
		glfwSetWindowSize(windows[i], width, height);
		glViewport(0, 0, width, height);
		mode = glfwGetVideoMode(this->listMonitors[i]);
		glfwGetMonitorPos(this->listMonitors[i], &xPos, &yPos);
		glfwSetWindowMonitor(this->windows[i], NULL, xPos + (mode->width / 2 - width / 2), yPos + (mode->height / 2 - height / 2), width, height, mode->refreshRate);
	}
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
	windows = new GLFWwindow * [nbMonitors];
	int xPos, yPos;
	for (int i = 0; i < this->nbMonitors; i++) {
		const GLFWvidmode* mode = glfwGetVideoMode(this->listMonitors[i]);
		if (mode == NULL)
			throw std::runtime_error("Impossible to get VideoMode...");
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		
		int width = (widthWin >= 0) ? widthWin : mode->width;
		int height = (heightWin >= 0) ? heightWin : mode->height;
		resolutionImage = (widthWin == -2 || heightWin == -2);

		printf("Name of the screen: %s\n", glfwGetMonitorName(listMonitors[i]));

		try{
			if (i >= 1)
				this->windows[i] = glfwCreateWindow(mode->width,mode->height, glfwGetMonitorName(listMonitors[i]), this->listMonitors[i], this->windows[i - 1]);
			else
				this->windows[i] = glfwCreateWindow(mode->width, mode->height, glfwGetMonitorName(listMonitors[i]), this->listMonitors[i], NULL);
		}
		catch (Exception e) {
			printf(e.what());
			throw std::runtime_error("Impossible to create a GLFW window for visualization.");
		}
		if (this->windows[i] == NULL) {
			throw std::runtime_error("Impossible to create a GLFW window for visualization.");
		}
		this->makeContext(i);
		glfwSetWindowSize(windows[i], width, height);
		glfwGetMonitorPos(this->listMonitors[i], &xPos, &yPos);
		glfwSetWindowMonitor(this->windows[i], NULL, xPos + (mode->width/2 - width/2), yPos + (mode->height / 2 - height / 2), width, height, mode->refreshRate);
		//Glad
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			throw std::runtime_error("Impossible to initialize GLAD for visualization.");
		}
		glViewport(0, 0, width, height);
	}

	shaderProgram = new unsigned int[this->nbMonitors];

	VBO = new unsigned int[this->nbMonitors];
	VAO = new unsigned int[this->nbMonitors];
	EBO = new unsigned int[this->nbMonitors];

	texture = new unsigned int[this->nbMonitors];
	nbOfFramesCanBeLoaded = (framesVideo < nbOfFramesCanBeLoaded) ? framesVideo : nbOfFramesCanBeLoaded;
	this->texImg = new Mat[this->nbMonitors *(1+nbOfFramesCanBeLoaded*isVideoMode)];
}


void Visualizator::setNbOfFramesDisplayed(int framesDisplayed) {
	this->nbOfFramesCanBeLoaded = framesDisplayed;
	delete[] texImg;
	nbOfFramesCanBeLoaded = (framesVideo < nbOfFramesCanBeLoaded) ? framesVideo : nbOfFramesCanBeLoaded;
	this->texImg = new Mat[this->nbMonitors * (1 + nbOfFramesCanBeLoaded * isVideoMode)];
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