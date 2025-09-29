#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string.h>
#include <cmath>

//Window dimension
const GLuint WIDTH = 800;
const GLuint HEIGHT = 600;
GLuint VAO, VBO, shader, uniformXMove;

bool direction = true; //left direction
float triOffset = 0.0f; /// left side (starting point)
float triMaxOffset = 0.7f; //right side (ending point)
float triIncrement = 0.005f; //speed of the movement

// Vertex Shader
static const char* vShader = R"(
#version 330

layout (location = 0) in vec3 pos;

uniform float xMove;

void main()
{
	gl_Position = vec4(0.4 * pos.x + xMove, 0.4 * pos.y, 0.4 * pos.z, 1.0f);
}
)";

//Fragment shader
static const char* fShader = R"(
#version 330

out vec4 color;

void main()
{
	color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
)";

void CreateTriangle()
{
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &VAO); //define the memomryand give the memory
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		std::cout << "Error compiling "  << shaderType <<"  shader program " << eLog << std::endl;
		return;
	}

	glAttachShader(theProgram, theShader);
	return;
}

void CompileShader()
{
	shader = glCreateProgram();

	if (!shader)
	{
		std::cout << "Error creating shader program" << std::endl;
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		std::cout << "Error Linking program " << eLog << std::endl;
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		std::cout << "Error validate program " << eLog << std::endl;
		return;
	}

	uniformXMove = glGetUniformLocation(shader, "xMove");
}

int main(int argc, char* argv[])
{
	//Initialize glfw init
	if (!glfwInit())
	{
		std::cout << "GLFW Intialization failed!!" << std::endl;
		glfwTerminate();
		return 1;
	}

	//Setup GLFW Window Properties
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Core profile = No backward compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);

	if (!mainWindow)
	{
		std::cout << "GLFW Window creation failed" << std::endl;
		glfwTerminate();
		return 1;
	}

	//get the buffer size dimension
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//Set the context glew to use
	glfwMakeContextCurrent(mainWindow);

	//Allow modern extension feature g
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW Intialization failed" << std::endl;
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	} 

	//Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);


	CreateTriangle();
	CompileShader();

	//loop until window close
	while (!glfwWindowShouldClose(mainWindow))
	{
		//Get + Handle user input events
		glfwPollEvents();


		if (direction)
		{
			triOffset += triIncrement;
			std::cout << triOffset << std::endl;
		}
		else {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
			std::cout << "direction" << direction;
		}

		//Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glUniform1f(uniformXMove, triOffset);

		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
		
	}
	return 0;
}