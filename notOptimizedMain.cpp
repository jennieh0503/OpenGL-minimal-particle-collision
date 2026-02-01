#include<iostream>
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>
void drawShape(GLFWwindow* window, float offset);

int main() {

	//initialize glfw
	glfwInit();

	//initialize window, specify version (3.3 or higher in this case)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create window and check errors
	//glfw creation functions requires width, length and name as parameters
	//there are two extra parameters that are monitor and window, that I am going to ignore for the scope of this project
	GLFWwindow* window = glfwCreateWindow(800, 600, "minimalRender", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//set window as context
	glfwMakeContextCurrent(window);

	//start glad
	gladLoadGL();

	//set working or display area
	//the function requires coordinates for bottom left corner and top left corner of the interested area
	//In this experiment I used the entire window
	glViewport(0, 0, 800, 600);

	//set window color:
	//back buffer with new color and front buffer with the current one
	//swap buffers to display the set color
	glClearColor(0.840, 0.840, 0.840, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);


	
	
	

	//let window run until closure, added logic for a "bouncy" shape
	while (!glfwWindowShouldClose(window)) {
		
		float offset = 0.0;
		float speed = 1.0;
		drawShape(window, offset);
		while (speed>0.15 && !glfwWindowShouldClose(window)) {
			offset += 0.01*speed;
			speed -= 0.008;
			drawShape(window, offset);
		}
		while (offset >= 0 && !glfwWindowShouldClose(window)) {
			offset -= 0.01*speed;
			speed += 0.05;
			drawShape(window, offset);
		}

		glfwPollEvents();
	}


	//clean used resources
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


//added offset to make the shape move, offset not checked, it could end out the visible window
void drawShape(GLFWwindow* window, float offset) {
	//create vertices array, the array contains coordinates in normalized form
	//z axis kept to a constant to represent a flat 2d object
	float vertices[] = {
		-0.5f, -1.0f+offset, 0.0f,
		0.5f, -1.0f+offset, 0.0f,
		-0.5f, 0.0f+offset, 0.0f,
		0.5f, -1.0f+offset, 0.0f,
		0.5f, 0.0f+offset, 0.0f,
		-0.5f, 0.0f+offset, 0.0f
	};


	//shaders source code in glsl
	//based on LearnOpenGL basic shaders
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\0";


	//creating shaders
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	unsigned int shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertexShader);
	glAttachShader(shaderProg, fragmentShader);
	glLinkProgram(shaderProg);

	//shaders now passed to the gl program, they can be deleted
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//vertices need to be passed to the gpu, buffers are used to send large amounts without having to consume more time
	//creating a vertix buffer (not using an array because of only one object present to send)
	//a vertix array object is also needed to switch between objects faster
	unsigned int vArray, vBuffer;
	glGenVertexArrays(1, &vArray);
	glGenBuffers(1, &vBuffer);
	glBindVertexArray(vArray);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//configuration of VAO to interpret the data sent to the buffer.
	//the vertices were saved as sets of coordinates, all in one single array, therefore each 3 belond to a vertex
	//(starting from element 0 of the array vertices)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//draw shape
	//changed int0 a rectangle, obtained as two triangles so the number of vertices became 6
	glClearColor(0.840, 0.840, 0.840, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProg);
	glBindVertexArray(vArray);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glfwSwapBuffers(window);

	//cleaning
	glDeleteVertexArrays(1, &vArray);
	glDeleteBuffers(1, &vBuffer);
	glDeleteProgram(shaderProg);
}