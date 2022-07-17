#include "GLItems.h"
#include <iostream>
#include <string.h>
#include <fstream>

#include "logger.h"

GLuint loadShader(const char *shaderPath, GLenum shaderType)
{

    std::ifstream file((shaderPath));
    std::string shaderSource;
    std::string line;

    if(file.is_open())
    {
        while(std::getline(file, line))
            shaderSource += line + "\n";
        file.close();
    }
    else
    {
	logger::Log(logger::LogLevel::FATAL, "Failed to open shader file: " + std::string(shaderPath));
	ForceTerminate();
    }

    GLuint shader = glCreateShader(shaderType);
    const char* source = shaderSource.c_str();
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        // TODO: Only add infoLog if in DEBUG mode
	logger::Log(logger::LogLevel::FATAL, "Failed to compile shader: " + std::string(infoLog));
        glDeleteShader(shader);
	ForceTerminate();
        return 0;
    }
    logger::Log(logger::LogLevel::DEBUG, "Compiled shader: " + std::string(shaderPath));
    return shader;
}


GLuint createShaderProgram(std::vector<GLuint> shaderList)
{
	    GLuint program = glCreateProgram();
    for(GLuint shader : shaderList)
	glAttachShader(program, shader);
    glLinkProgram(program);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
	glGetProgramInfoLog(program, 512, NULL, infoLog);
	logger::Log(logger::LogLevel::FATAL, "Failed to link shader program: " + std::string(infoLog));
	ForceTerminate();
    }
    logger::Log(logger::LogLevel::DEBUG, "Linked shader program");
    return program;
}






void DeleteGLItem(GLuint item)
{
	logger::Log(logger::LogLevel::DEBUG, "Deleted GL item: " + std::to_string(item));
	glDeleteProgram(item);
}


void ForceTerminate()
{
	logger::Log(logger::LogLevel::FATAL, "Force terminating");
    	glfwTerminate();
	exit(EXIT_FAILURE);
}
