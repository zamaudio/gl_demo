#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "gldemo.h"

#define WINDOW_TITLE "OpenGL Demo"

namespace Video {

void GL_base::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void GL_base::init(void)
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	params.window = glfwCreateWindow(params.res_width, params.res_height, WINDOW_TITLE, glfwGetPrimaryMonitor(), NULL);
	glfwMakeContextCurrent(params.window);

	// Set the required callback functions
	glfwSetKeyCallback(params.window, GL_base::key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(params.window, &width, &height);
	glViewport(0, 0, width, height);
}

void GL_base::set_params(struct parameters p)
{
	params = p;
}

void GL_base::compile_shaders(void)
{
	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &(params.vertex_shader_source), NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &(params.fragment_shader_source), NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	params.shader_program = glCreateProgram();
	glAttachShader(params.shader_program, vertexShader);
	glAttachShader(params.shader_program, fragmentShader);
	glLinkProgram(params.shader_program);
	// Check for linking errors
	glGetProgramiv(params.shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(params.shader_program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void GL_base::set_vertices(GLfloat vertex[], GLuint count)
{
	GLuint i;
	for (i = 0; i < count; i++) {
		vertices[i] = vertex[i];
	}
	vertex_count = count;
}

void GL_base::load_vertices()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
}

void GL_base::cleanup(void)
{
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}

void GL_base::render_loop(void)
{
	while (!glfwWindowShouldClose(params.window))
	{
		glfwPollEvents();

		// Render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw our first triangle
		glUseProgram(params.shader_program);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(params.window);
	}
}
} /* namespace */

int main()
{
	// Shaders
	const GLchar* vertex_shader = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
		"}\0";
	const GLchar* fragment_shader = "#version 330 core\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";

	struct parameters p = {
		vertex_shader,
		fragment_shader,
		0,
		1366,
		768,
	};

	// Vertices
	GLfloat vs[] = {
		-0.5f, -0.5f, 0.0f, // Left
		 0.5f, -0.5f, 0.0f, // Right
		 0.0f,  0.5f, 0.0f  // Top
	};

	Video::GL_base ctx;

	ctx.set_params(p);
	ctx.set_vertices(vs, 9);

	ctx.init();
	ctx.compile_shaders();
	ctx.load_vertices();

	ctx.render_loop();

	return 0;
}
