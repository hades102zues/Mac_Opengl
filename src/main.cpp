#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <filesystem>

#include "shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//TODO
// (1) FIND A WAY TO PROGRAMMATICALLY CAPTURE THE LOCATION OF THE SHADER FILES

// Window Variables
int windowHeight = 600;
int windowWidth = 800;


// Camera Variables
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 arbitraryStepFromCamera = glm::vec3(0.0f, 0.0f, -1.0f);  //take note that the value here is negative. such that we are then moving in front of the camera
glm::vec3 arbitraryCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


// Movement Speed Correction Variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;


//Mouse movement variables
float yaw = -90.0f;
float pitch = 0.0f;
float firstMouseMove = true;
float lastX = 400, lastY = 300; //default mouse placement



// lighting
glm::vec3 lightPos(1.2f, 1.0f, 1.0f);




void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	const float cameraSpeed = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * arbitraryStepFromCamera;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * arbitraryStepFromCamera;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(arbitraryStepFromCamera, arbitraryCameraUp)) * cameraSpeed;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(arbitraryStepFromCamera, arbitraryCameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {


	//Mouse move jump correction
	if (firstMouseMove)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}

	// FIRST DEAL WITH SCREEN COORDINATES
	// BEGIN THE MOUSE AT SOME POINT ON THE SCREEN
	// TRACK HOW MUCH THE MOUSE MOVED ALONG X,Y (aka offset)
	// RECORD THE LASTEST POSITION
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	//std::cout << xpos << "," << ypos << "," << xoffset << "," << yoffset << "," << std::endl;
	const float sensitivity = 0.03f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;


	//add the offsets to the global pitch and yaw values
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	//std::cout << pitch << "," << yaw << std::endl;
	// translates the movement in 2d to movements in a 3d space
	// by somehow treating the changes as angles .-.
	// applying the mathematical hullabalu we get the amounts move along x,y,z
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));


	//store those values in our camera stepper vector
	arbitraryStepFromCamera = glm::normalize(direction);
}

unsigned int produceTexture(const char* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	    int width, height, nrComponents;
	    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	    if (data)
	    {
		GLenum format;
		if (nrComponents == 1)
		    format = GL_RED;
		else if (nrComponents == 3)
		    format = GL_RGB;
		else if (nrComponents == 4)
		    format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	    }
	    else
	    {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	    }

	    return textureID;
}


int main(int argc, char** argv) {

	// SETUP Window and OpenGl Functions
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Game", NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//get the actual buffer size of the window and resize viewport
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	std::cout << "PROGRAM DIRECTORY: " << std::filesystem::current_path().string() << std::endl;

	// VERTEX DATA
	  float vertices[] = {
        // positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	// ====SETUP CUBE OBJECT=========
	Shader cubeShader("src/cube_shader.vs", "src/cube_shader.fs");

	
	unsigned int cubeVao;
	glGenVertexArrays(1, &cubeVao);
	glBindVertexArray(cubeVao);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	unsigned int diffuseTexture = produceTexture("src/container2.png");
	unsigned int specularTexture = produceTexture("src/container2_specular.png");

	// HOW TO MOVE DATA INTO THE VERTEX SHADER ATTRIBUTE(S)
	// provide the location of target attribute       //an attribute is an input variable defined in the vertex shader
	// provide details as to how the data should be parsed out of the buffer/texture object
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	
	// ===SETUP LIGHT CUBE=====
	Shader lightCubeShader("src/light_shader.vs", "src/light_shader.fs");

	unsigned int lightVao;
	glGenVertexArrays(1, &lightVao);
	glBindVertexArray(lightVao);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// SUPER LOOP
	while (!glfwWindowShouldClose(window)) {

		//timing
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//event management
		processInput(window);
		


		//clear buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //(0.075f, 0.075f, 0.075f, 1.0f)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		// ===DRAW THE CUBE============
		cubeShader.use();
		cubeShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
		cubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		cubeShader.setVec3("lightPos", lightPos);
		cubeShader.setVec3("cameraPos", cameraPos);
		
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularTexture);
		//cubeShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		cubeShader.setFloat("material.shinny", 32.0f);
		
		cubeShader.setVec3("lightProperties.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		cubeShader.setVec3("lightProperties.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		cubeShader.setVec3("lightProperties.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view;
		glm::mat4 projection;
		
		model = glm::scale(model, glm::vec3(1.5f));
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		view = glm::lookAt(cameraPos, cameraPos + arbitraryStepFromCamera, arbitraryCameraUp);
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		
		cubeShader.setMat4("model", model);
		cubeShader.setMat4("view", view);
		cubeShader.setMat4("projection", projection);

		glBindVertexArray(cubeVao);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDrawArrays(GL_TRIANGLES, 0, 36);



		// ===DRAW LIGHT CUBE=======
		lightCubeShader.use();

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lightCubeShader.setMat4("model", model);
		lightCubeShader.setMat4("view", view);
		lightCubeShader.setMat4("projection", projection);

		lightCubeShader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
		lightCubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

		glBindVertexArray(lightVao);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDrawArrays(GL_TRIANGLES, 0, 36);




		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}
