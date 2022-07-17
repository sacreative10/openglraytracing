#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>


// loads a shader from a file, than returns it.
GLuint loadShader(const char* shaderPath, GLenum shaderType);

GLuint createShaderProgram(std::vector<GLuint> shaderList);


void DeleteGLItem(GLuint item);

void ForceTerminate();


