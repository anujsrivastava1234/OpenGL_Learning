#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <glm/mat4x4.hpp> //glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLuint WIDTH = 800, HEIGHT = 600;
GLuint VAO, VBO, shader, uniformModel ;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

static const char* vShader = R"(
#version 330
layout (location = 0 ) in vec3 pos;
uniform float xMove;
void main()
{
	gl_Position = vec4(0.4 * pos.x + xMove,0.4 * pos.y, 0.4 * pos.z, 1.0f);
}
)";

static const char* fShader = R"(
#version 330
out vec4 color;
void main()
{
	color = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

void CreateTriangle()
{
	GLfloat vertices[] =
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void AddShader(GLuint theProgram, const char* shaderSource, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const char* theCode[1];
	theCode[0] = shaderSource;

	int theCodeLength[1];
	theCodeLength[0] = strlen(shaderSource);

	glShaderSource(theShader, 1, theCode, theCodeLength);
	glCompileShader(theShader);

	int result = { 0 };
	char eLog[1024] = { 0 };
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		std::cout << "ERROR::COMPILING::SHADER::CODE " << eLog << std::endl;
		return;
	}

	glAttachShader(theProgram, theShader);
	return;
}

void CompileShader()
{
	shader = glCreateProgram();

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	int result = { 0 };
	char eLog[1024] = { 0 };
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		std::cout << "ERROR::LINKING::PROGRAM" << eLog << std::endl;
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		std::cout << "ERROR::VALIDATING::PROGRAM" << eLog << std::endl;
		return;
	}
	 
	uniformModel = glGetUniformLocation(shader, "xMove");
}

int main(int argc, char* argv[])
{
	//define the glfw intialization
	if (!glfwInit())
	{
		std::cout << "GLFW Not intialized" << std::endl;
		glfwTerminate();
		return 1;
	}

	//set the window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "OpenGLCourseApp", NULL, NULL);

	if (!mainWindow)
	{
		std::cout << "Could not intialize the mainWindow " << std::endl;
		glfwTerminate();
		return 1;
	}

	//intialize the buffer width and bufferheight
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow);

	//enable the glew experimental
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Could not intialize the GLEW context" << std::endl;
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShader();

	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		if (direction)
		{
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) > triMaxOffset)
		{
			direction = !direction;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader);

		glm::mat4 model(1.0f); //identity matrix;

		glUniform1f(uniformModel, triOffset);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		
		glfwSwapBuffers(mainWindow);
	}
	return 0;
}