#include "Simulator.h"

Simulator::Simulator(char* pathFile) {
	this->loadFileProp(pathFile);
};

void Simulator::launchSim() {
	glm::vec3 eye = this->init();
	this->shaderOpenGL();

	for (int i = 0; i < this->LayerNum; i++) {
		this->buildDisplay(i);
	}

	this->loadTexture();

	glClearColor(1.0f, 1.0f, 1.0f, 0.0);

	while (!glfwWindowShouldClose(this->window))
	{
		//Input
		eye = this->processInput(window, eye);

		//Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);

		glUseProgram(shaderProgram);
		for (int i = this->LayerNum-1; i >= 0; i--) {
			//this->activeObject(i);
			this->applyTexture(i);
			this->activateShader(eye, i);
			this->draw(i);
		}
		glDisable(GL_BLEND);
		glFlush();
		glDisable(GL_DEPTH_TEST);

		this->nowFrame = (this->nowFrame + 1) % this->Frame;
		//Events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	for (int i = 0; i < this->LayerNum; i++){
		glDeleteBuffers(1, EBO + i);
		glDeleteVertexArrays(1, VAO+i);
		glDeleteBuffers(1, VBO+i);
	}
}

void Simulator::activeObject(int layerHandled) {
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(*(VAO + layerHandled));
	glBindBuffer(GL_ARRAY_BUFFER, *(VBO + layerHandled));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(EBO + layerHandled));
	const GLint posLoc = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(posLoc);

	// texture coord attribute
	const GLint texLoc = glGetAttribLocation(shaderProgram, "texCoords");
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(texLoc);
}

void Simulator::buildDisplay(int depth) {
	float infoDisplay[] = {
		// Vertices									//Tex Coords		
		-0.8f, -0.8f, - this->offsetDepth * depth,	0.0f,0.0f,
		0.8f,-0.8f, - this->offsetDepth * depth,		1.0f,0.0f,
		0.8f,0.8f,- this->offsetDepth * depth,		1.0f,1.0f,
		-0.8f,0.8f,- this->offsetDepth * depth,		0.0f,1.0f
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 2,  // first Triangle
		0, 2, 3   // second Triangle
	};

	glGenVertexArrays(1, VAO+depth);
	glGenBuffers(1, VBO + depth);
	glGenBuffers(1, EBO + depth);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(*(VAO + depth));

	glBindBuffer(GL_ARRAY_BUFFER, *(VBO + depth));
	glBufferData(GL_ARRAY_BUFFER, sizeof(infoDisplay), infoDisplay, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(EBO + depth));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	const GLint posLoc = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(posLoc);

	// texture coord attribute
	const GLint texLoc = glGetAttribLocation(shaderProgram, "texCoords");
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(texLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Simulator::draw(int layerHandled) {
	glBindVertexArray(*(this->VAO + layerHandled));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Simulator::activateShader(glm::vec3 eye,int layerHandled) {
	if (MLoc == NULL || VLoc == NULL || texLoc == NULL || PLoc == NULL) {
		MLoc = glGetUniformLocation(shaderProgram, "M");
		VLoc = glGetUniformLocation(shaderProgram, "V");
		PLoc = glGetUniformLocation(shaderProgram, "P");
		texLoc = glGetUniformLocation(shaderProgram, "tex");
	}
	//Camera
	glm::vec3 center = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 camera = glm::lookAt(eye, center, up);
	//Model
	glUniformMatrix4fv(MLoc, 1,false, &(glm::mat4(1.0f)[0][0]));
	glUniformMatrix4fv(VLoc, 1, false, &(camera[0][0]));
	glUniformMatrix4fv(PLoc, 1, false, &(getProjection( 45.0,1.0, 0.01,100.0)[0][0]));

	//Texture
	glUniform1i(texLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[layerHandled]);
}

glm::mat4 Simulator::getProjection(float fov, float ratio, float near, float far) {
	glm::mat4 projection = glm::mat4();
	projection = glm::perspective(fov, ratio, near, far);
	return projection;
}

void Simulator::loadTexture(){
	for(int i = 0; i< this->LayerNum;i++){
		this->texImg[i] = NULL;
		glGenTextures(1, texture+i);
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		// GL, by default, expects rows of pixels to be padded to a multiple of 4 bytes.A 1366 wide texture with 1 byte or 3 byte wide pixels, will not be naturally 4 byte aligned.
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		this->applyTexture(i);
	}
}

void Simulator::applyTexture(int layerHandled) {
	char* pathToLoad = this->constructStringPath(layerHandled);
	texImg[layerHandled] = imread(pathToLoad);
	if (!texImg[layerHandled].empty()) {
		flip(texImg[layerHandled], texImg[layerHandled], 0);
		cvtColor(texImg[layerHandled], texImg[layerHandled], COLOR_BGR2RGB);
		glBindTexture(GL_TEXTURE_2D, texture[layerHandled]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImg[layerHandled].size[1], texImg[layerHandled].size[0], 0, GL_RGB, GL_UNSIGNED_BYTE, texImg[layerHandled].data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		//Error load
		printf("Error: load img... ");
		printf(pathToLoad);
		printf("\n");
	}
	delete(pathToLoad);
}

char* Simulator::constructStringPath(int layerHandled) {
	char* filename = new char[252];
	sprintf(filename, "%s\\layer-%d-%d.png", this->pathToLayerImg, layerHandled, this->nowFrame);
	return filename;
}

void Simulator::loadFileProp(char* pathFileProp) {
	FILE* fp;
	char s[256];

	fp = fopen(pathFileProp, "r");

	int mode, varnum, hornum, iteration;
	char loadfilename[128];
	int result;

	result = fscanf(fp, "%s\t%d", s, &mode);
	result = fscanf(fp, "%s\t%d", s, &varnum);
	result = fscanf(fp, "%s\t%d", s, &hornum);
	result = fscanf(fp, "%s\t%d", s, &(this->LayerNum));
	result = fscanf(fp, "%s\t%d", s, &(this->Frame));
	result = fscanf(fp, "%s\t%lf", s, &(this->Bright));
	result = fscanf(fp, "%s\t%d", s, &iteration);
	result = fscanf(fp, "%s\t%s", s, loadfilename);
	this->pathToLayerImg = new char[128];
	result = fscanf(fp, "%s\t%s", s, this->pathToLayerImg);

	fclose(fp);
}

Simulator::~Simulator() {
	delete pathToLayerImg;
}

void Simulator::shaderOpenGL() {
	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

	GLchar* pointerV[1];
	pointerV[0] = vertexShaderSource;
	glShaderSource(vertexShader, 1,pointerV, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("Error: shaders: ");
		printf(infoLog);
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
		printf("Error: shaders: ");
		printf(infoLog);
	}
	// link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Error: shaders: ");
		printf(infoLog);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

glm::vec3 Simulator::init() {
	if (!glfwInit()) {
		printf("Error: while init glfw...");
		return glm::vec3();
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//Launch window
	this->window = glfwCreateWindow(winsize, winsize, "Simulation of Tensor Display", NULL, NULL);
	if (window == NULL) {
		return glm::vec3();
	}
	glfwMakeContextCurrent(window);
	//Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return glm::vec3();
	}
	glViewport(0, 0, winsize, winsize);
	//Resize window when user did
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	VBO = new unsigned int [this->LayerNum]; 
	VAO = new unsigned int [this->LayerNum];
	EBO = new unsigned int [this->LayerNum]; 

	texture = new unsigned int [this->LayerNum];

	this->texImg = new Mat [this->LayerNum];
	
	glm::vec3 eye = glm::vec3(0,0,2.5);

	return eye;
}

glm::vec3 Simulator::processInput(GLFWwindow* window,glm::vec3 eye)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(this->window, &x, &y);
		x = (x - this->winsize / 2);
		y = (y - this->winsize / 2);
		printf("prev Cursor: (%f,%f)\n", px, py);
		printf("Cursor: (%f,%f)\n", x, y);
		if (x != px || y != py) {
			//Move eye
			yaw += ((x - px) * this->mouseSensitivity);
			pitch += ((y - py) * this->mouseSensitivity);
			if (yaw > maxYaw)
				yaw = maxYaw;
			else if (yaw < -maxYaw)
				yaw = -maxYaw;
			if (pitch > maxPitch)
				pitch = maxPitch;
			else if (pitch < -maxPitch)
				pitch = -maxPitch;

			printf("Angle: (%f,%f)\n", yaw, pitch);
			px = x;
			py = y;

			glm::vec3 direction;
			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			eye = glm::normalize(direction);
			eye.z = eye.z + 2.5;

			//eye = this->rotateY(eye, glm::vec3(0, 0, 0), -yaw);
			//eye = this->rotateX(eye, glm::vec3(0, 0,0), -pitch);
			printf("eye pos: (%f,%f,%f)\n", eye.x, eye.y, eye.z);
			//eye = glm::vec3(cos(pitch) * sin(yaw), sin(pitch), cos(pitch) * cos(yaw));
		}
	}
	return eye;
}

glm::vec3 Simulator::rotateX(glm::vec3 v, glm::vec3 origin, double angle) {
	//Translate point to the origin
	double px = v.x - origin.x;
	double py = v.y - origin.y;
	double pz = v.z - origin.z;
	//perform rotation
	double rx = px;
	double ry = py * cos(angle) - pz * sin(angle);
	double rz = py * sin(angle) + pz * cos(angle);
	//translate to correct position
	double outx = rx + origin.x;
	double outy = ry + origin.y;
	double outz = rz + origin.z;
	return glm::vec3(outx, outy, outz);
}

glm::vec3 Simulator::rotateY(glm::vec3 v,glm::vec3 origin, double angle) {
	//Translate point to the origin
	double px = v.x - origin.x;
	double py = v.y - origin.y;
	double pz = v.z - origin.z;
	//perform rotation
	double rx = pz * sin(angle) + px * cos(angle);
	double ry = py;
	double rz = pz * cos(angle) - px * sin(angle);
	//translate to correct position
	double outx = rx + origin.x;
	double outy = ry + origin.y;
	double outz = rz + origin.z;
	return glm::vec3(outx,outy,outz);
}