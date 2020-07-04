#include <GL/gl.h>

const GLchar *vshader = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"out vec3 vertex_color;\n"
"void main() {\n"
"	gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"	vertex_color = color;\n"
"}\0";

const GLchar *fshader = "#version 330 core\n"
"out vec4 color;\n"
"in vec3 vertex_color;\n"
"void main() {\n"
"	color = vec4(vertex_color, 1.0);\n"
"}\0";
