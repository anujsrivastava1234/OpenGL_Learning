#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLint WIDTH = 800, HEIGHT = 600;
GLuint VAO, VBO, EBO, shader;

const char* vShaderSource = R"(
#version 330 core
layout (location = 0 ) in vec3 pos;

void main()
{
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}
)";

const char* fShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.0f, 0.2f, 1.0f);
} 
)";

void CreateTriangles()
{
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f, //left 
		0.5f, -0.5f, 0.0f,	//right
		-0.5f, 0.5f, 0.0f,	//top
		0.5f, 0.5f, 0.0f,
	};

	GLint indices[] = {
		0, 1, 2,
		1, 2, 3
	};
	
	//generate the VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//generate Vbo 
	glGenBuffers(1, &VBO);

	//generate EBO
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//set the vertex array attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



}

GLuint compileShader(const char* shaderSource, GLenum shaderType)
{
	GLuint shader =  glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return shader;
}

void createShader()
{
	shader = glCreateProgram();
	GLuint vertexShader, fragmentShader;

	vertexShader = compileShader(vShaderSource, GL_VERTEX_SHADER);
	fragmentShader = compileShader(fShaderSource, GL_FRAGMENT_SHADER);

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);

	int success;
	char infoLog[1024];

	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::LINKING::PIPELINE::FAILED" << infoLog << std::endl;
	}
}

int main(int argc, char* argv[])
{
	if (!glfwInit())
	{
		std::cout << "GLFW Intialization Failed" << std::endl;
		glfwTerminate();
		return 1;
	}

	//Set the glfwWindow Hint
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "TEST Window", NULL, NULL);

	if (!mainWindow)
	{
		std::cout << "GLFW Window is not intialized" << std::endl;
		glfwTerminate();
		return 1;
	}

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	glfwMakeContextCurrent(mainWindow);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "could not intialzie the glew context";
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//set the viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	CreateTriangles();
	createShader();

	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glfwSwapBuffers(mainWindow);
	}
	return 0;
}