#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>

//1. create GLFW window
//2. CreateTrianle
//3. CompileShader
//4. AddShader

const GLint WIDTH = 800, HEIGHT = 600;
GLuint VAO, VBO, shader, uniformXMove, uniformYMove, uniformColor;

bool direction = false;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;
float timeHandle ;
float color_value;

static const char* vShader = R"(
#version 330
layout (location = 0 ) in vec3 pos;
uniform float xMove;
uniform float yMove;
void main()
{
	gl_Position = vec4(0.4 * pos.x + xMove, 0.4 * pos.y + yMove, 0.4 * pos.z, 1.0f);
}
)";

static const char* fShader = R"(
#version 330
out vec4 color;
uniform vec4 handleColor;
void main()
{
	color = handleColor;
}
)";

void CreateTriangle()
{
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glGenVertexArrays(0, &VAO);
}

void AddShader(GLuint theProgram,const char* shaderSource, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar *theCode[1];
	theCode[0] = shaderSource;

	GLint theCodeLength[1];
	theCodeLength[0] = strlen(shaderSource);

	glShaderSource(theShader, 1, theCode, theCodeLength);
	glCompileShader(theShader);
	
	int result = { 0 };
	char eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		std::cout << "ERROR::COMPILING::SHADER::PROGRAM" << eLog << std::endl;
		return;
	}

	glAttachShader(theProgram,theShader);

	return;
}

void CompileShader()
{
	shader = glCreateProgram();

	if (!shader)
	{
		std::cout << "Failed to created the shader program" << std::endl;
		return;
	}

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
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		std::cout << "ERROR::VALIDATING::PROGRAM" << eLog << std::endl;
		return;
	}

	uniformXMove = glGetUniformLocation(shader, "xMove");
	uniformYMove = glGetUniformLocation(shader, "yMove");
	uniformColor = glGetUniformLocation(shader, "handleColor");
}

int main(int argc, char* argv[])
{
	if (!glfwInit())
	{
		std::cout << "GLFW CONTEXT Not Intialized" << std::endl;
		glfwTerminate();
		return 1;
	}

	//set the windowhints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "TEST", NULL, NULL);

	if (!mainWindow )
	{
		std::cout << "GLFW Window not intialized" << std::endl;
		glfwTerminate();
		return 1;
	}

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Could not intialize the GLEW Context" << std::endl;
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	CreateTriangle();
	CompileShader();

	//set the viewport
	glViewport(0, 0, bufferWidth, bufferHeight);

	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		timeHandle = glfwGetTime();
		color_value = sin((timeHandle) / 0.2f) + 0.5f;

		if (direction)
		{
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glUniform1f(uniformXMove, triOffset);
		glUniform1f(uniformYMove, triOffset);
		glUniform4f(uniformColor, 1.0f, color_value, 0.0f, 1.0f);

		glBindVertexArray(VAO);

		
		glDrawArrays(GL_TRIANGLES, 0, 3);


		glfwSwapBuffers(mainWindow);
	}

	return 0;
}