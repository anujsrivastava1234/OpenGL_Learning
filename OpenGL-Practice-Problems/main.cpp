/*
* Objective 1 -> Draw a rectangle
* //Make a cross and try to reduce the glow 
*/
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
const GLint WIDTH = 800, HEIGHT = 600;
GLuint VAO, VBO, EBO, shader;
GLuint resHandle, timeHandle, ringColorHandle;
float timeValue = 0.0f;

static const char* vShader = R"(
#version 330
layout (location = 0) in vec3 pos;
out vec2 fragCoord;

void main()
{
    gl_Position = vec4(pos, 1.0);
    fragCoord = pos.xy * 0.5 + 0.5; // map [-1,1] → [0,1]
}
)";

static const char* fShader = R"(
#version 330
out vec4 FragColor;
in vec2 fragCoord;

uniform vec3 iResolution;
uniform float iTime;
uniform vec3 ringColor;

void main()
{
    vec2 uv = 1.5 * (2.0 * fragCoord.xy * iResolution.xy - iResolution.xy) / iResolution.y;

    // offset only depends on time now
    vec2 offset = vec2(cos(iTime), sin(iTime));
	vec2 offset1 = vec2(sin(iTime), cos(iTime));

    vec3 light_color = ringColor;
    float light = 0.1 / distance(normalize(uv), uv);

    if(length(uv) < 1.0){
        light *= 0.1 / distance(normalize(uv - offset), uv - offset );
		light *= 0.1 / distance(normalize(uv - offset1), uv - offset1);    
}

    FragColor = vec4(light * light_color, 1.0);
}

)";


void CreateTriangle()
{
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, //bottom left
		1.0f, -1.0f, 0.0f, //bottom right
		-1.0f, 1.0f, 0.0f, //top left
		1.0f, 1.0f, 0.0f,  //top right
	};


	GLuint indices[] = {
		0, 1, 2,
		1, 2 ,3
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

}

void AddShader(GLuint theProgram,const char* shaderSource, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const char *theCode[1];
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
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << eLog << std::endl;
		return;
	}

	glAttachShader(theProgram, theShader);
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
		std::cout << "ERROR::COMPILE::STATUS " << eLog << std::endl;
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		std::cout << "ERROR::VALIDATE::STATUS " << eLog << std::endl;
		return;
	}

	resHandle = glGetUniformLocation(shader, "iResolution");
	timeHandle = glGetUniformLocation(shader, "iTime");
	ringColorHandle = glGetUniformLocation(shader, "ringColor");
}

int main(int argc, char* argv[])
{
	if (!glfwInit())
	{
		std::cout << "GLFW window not intialized" << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Practice Pipeline", NULL, NULL);

	if (!mainWindow)
	{
		std::cout << "Could not intialize GLFW " << std::endl;
		glfwTerminate();
		return 1;
	}

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Could not intialize GLEW context" << std::endl;
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	CreateTriangle();
	CompileShader();

	glViewport(0, 0, bufferWidth, bufferHeight);


	//IMGUI Intialization
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//IMGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glUseProgram(shader);
		glBindVertexArray(VAO);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		  // resolution
		glUniform3f(resHandle, (float)bufferWidth, (float)bufferHeight, 1.0f);

		// time
		
		timeValue += 0.016f; 
		glUniform1f(timeHandle, timeValue);
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		glUniform3f(ringColorHandle, 0.9f, greenValue, 0.5f);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		ImGui::Begin("ImGUI Window");
		ImGui::Text("Hello There");
		float f = 0.0f;
		ImGui::SliderFloat("Float", &f, 0.0f, 1.0f);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		
		glfwSwapBuffers(mainWindow);
	}


	return 0;
}