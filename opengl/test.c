#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <math.h>

#include "fll/fll.h"

#define VERTEX_SHADER "vshader.glsl"
#define FRAGMENT_SHADER "fshader.glsl"

void key_callback(GLFWwindow*, int, int, int, int);

int main(void) {
	GLFWwindow *window = NULL;
	GLint width = 0, height = 0, is_ok = 0;
	GLuint VBO = 0, VAO = 0, EBO = 0, vertex_shader = 0, fragment_shader = 0, shader_programm = 0;
	GLchar info[512] = "";
	char *shader_buffer = NULL;

	// square points
	GLfloat points[] = {
		// points      // colors	
		0.0, 0.5, 0.0, 1.0, 0.0, 0.0, // up right
		0.5, -0.5, 0.0, 0.0, 1.0, 0.0, // down right
		-0.5, -0.5, 0.0, 0.0, 0.0, 1.0 // down left
	};

	// points indexes
	GLuint index[] = {
		0, 1, 3,
		1, 2, 3
	};

	// glfw init
	glfwInit();

	// window init
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(800, 600, "test", NULL, NULL);
	if (!window) {
		fprintf(stderr, "glfwCreateWindow: Fail\n");
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);

	// glew init
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "glewInit: Fail\n");
		return 0;
	}

	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// key callback init
	glfwSetKeyCallback(window, key_callback);

	// load vertex shader
	shader_buffer = fll_read_from_file(VERTEX_SHADER);
	
	// init vertex shader
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const GLchar * const *)&shader_buffer, NULL);
	glCompileShader(vertex_shader);

	fll_free_s(shader_buffer);

	// check for shader compile errors
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_ok);
	if (!is_ok) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info);
		fprintf(stderr, "vShader - Fail!: %s\n", info);
	}

	// load fragment shader
	shader_buffer = fll_read_from_file(FRAGMENT_SHADER);

	// init fragment shader
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const GLchar * const *)&shader_buffer, NULL);
	glCompileShader(fragment_shader);

	fll_free_s(shader_buffer);

	// check for shader compile errors
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_ok);
	if (!is_ok) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info);
		fprintf(stderr, "fShader - Fail!: %s\n", info);
	}

	// make shader programm
	shader_programm = glCreateProgram();
	glAttachShader(shader_programm, vertex_shader);
	glAttachShader(shader_programm, fragment_shader);
	glLinkProgram(shader_programm);

	// check for shader programm link errors
	glGetProgramiv(shader_programm, GL_LINK_STATUS, &is_ok);
	if (!is_ok) {
		glGetProgramInfoLog(shader_programm, 512, NULL, info);
		fprintf(stderr, "Shader programm - Fail!: %s\n", info);
	}

	// vao, vbo, ebo init
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// fill vao, vbo, ebo
	glBindVertexArray(VAO);
	
	// bind and fill vbo
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	// bind and fill ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	// markup VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// unbind vao, vbo, ebo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	// main loop
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// actions
		glClearColor(0.2, 0.3, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// use shader program
		glUseProgram(shader_programm);

		// bind buffers
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// draw!
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// unbind buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glfwSwapBuffers(window);
	}
	
	// remove buffers
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// remove shaders
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// exit
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

