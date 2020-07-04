#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

void key_callback(GLFWwindow*, int, int, int, int);

int main()
{	
	GLFWwindow *window;
	GLint width, height;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n" );
	    return 0;
	}

	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	
	glfwSetKeyCallback(window, key_callback);


	while(!glfwWindowShouldClose(window))
	{
    	glfwPollEvents();

    	// actions
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);	
		
    	glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
