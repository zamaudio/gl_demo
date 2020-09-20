/* GL Demo */
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct parameters {
	const GLchar* vertex_shader_source;
	const GLchar* fragment_shader_source;
	GLuint shader_program;
	GLuint res_width;
	GLuint res_height;
	GLFWwindow* window;
};

namespace Video {

	class GL_base {
	public:
		GL_base() {};

		void set_params(struct parameters p);
		void init(void);
		void compile_shaders(void);
		void set_vertices(GLfloat vertex[], GLuint count);
		void load_vertices(GLuint count);
		void render_loop(void);
		void cleanup(void);
	private:
		struct parameters params;
		GLuint vertex_count;
		GLuint VBO, VAO;
		static void key_callback (GLFWwindow* window, int key, int scancode, int action, int mode);
		GLfloat vertices[];
	};

}
