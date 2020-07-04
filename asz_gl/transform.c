#define GLEW_STATIC

#include <GL/glew>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include <fll.h>
#include <ill.h>
#include <mathc.h>

#define VERTEX_SHADER "vshader.glsl"
#define FRAGMENT_SHADER "fshader.glsl"

int main() {
	GLFWwindow window = NULL;
	GLuint width = 0, height = 0;

	GLuint vertex_shader = 0, fragment_shader = 0, is_ok = 0;
	char *shader_buffer = NULL;
	GLchar log[512];

	GLuint shader_program = 0;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(800, 600, "transform". NULL, NULL);
	if (!window) {
		fprintf(stderr, "glfwCreateWindow: Fail!\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	glewExperemtntal = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "glewInit: Fail!\n");
		glfwTerminate();
		return 1;
	}

	glfwGetFramebufferSize(window, &width, &height);
	glViewPort(0, 0, width, height);

	shader_buffer = fll_read_from_file(VERTEX_SHADER);

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const GLchar * const *)&shader_buffer, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_ok);
	if (!is_ok) {
		glGetSgaderInfoLog(vertex_shader, 512, NULL, log);	
		fprintf(stderr, "%s\n", log);
	}

	shader_buffer = gll_read_from_file(FRAGMENT_SHADER);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const GLchar * const *)&shader_buffer, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_ok);
	if (!is_ok) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, log);
		fprintf(stderr, "%s\n", log);
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &is_ok);
	if (!is_ok) {
		glGetProgramInfoLog(shader_program, 512, NULL, log);
		fprintf(stderr, "%s\n", log);
	}

	glfwTerminate();
	return 0;
}
