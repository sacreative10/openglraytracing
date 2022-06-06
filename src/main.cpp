#include "include/glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <chrono>
#include <vector>
void NormalLogger(std::string message)
{
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string time_str = std::ctime(&time);
    time_str.pop_back();
    // print in yellow bold
    std::cout << "\033[1;33m" << "[" << time_str << "]" << ": " << message << "\033[0m" << std::endl;
}

// Error logger
// In red color outputs the message in bold
// takes the message as a parameter
// and the message is printed to the console, along with the time
void ErrorLogger(std::string message)
{
    // time in hours, minutes, seconds
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string time_string = std::ctime(&time);
    time_string.pop_back();
    std::cout << "\033[1;31m" << "[" << time_string << "]"<< ": " << message << "\033[0m" << std::endl;
}

void NormalLoggerFlush(std::string message)
{
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string time_str = std::ctime(&time);
    time_str.pop_back();
    // print in yellow bold
    std::cout << "\033[1;33m" << "[" << time_str << "]" << ": " << message << "\033[0m" << "\r";
}

void ForceTerminate()
{
    ErrorLogger("Forcing termination, this is a fatal error");
    glfwTerminate();
    exit(EXIT_FAILURE);
}


void error_callback(int error, const char* description)
{
    ErrorLogger(description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    NormalLogger("Framebuffer size changed to " + std::to_string(width) + "x" + std::to_string(height));
    glViewport(0, 0, width, height);
}

void input_callback(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

bool vSync = true;

GLfloat ScreenTriVert[] =
{
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

GLuint ScreenTriIndices[] =
{
    0, 2, 1,
    0, 3, 2
};

GLuint FileToShader(const char* filename, GLenum shaderType)
{
    std::ifstream file(filename);
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
        ErrorLogger("Failed to open file: " + std::string(filename));
        ForceTerminate();
        return 0;
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
        ErrorLogger("Failed to compile shader: " + std::string(infoLog));
        glDeleteShader(shader);
        ForceTerminate();
        return 0;
    }
    NormalLogger("Compiled shader: " + std::string(filename));
    return shader;
}

GLuint createShaderProgram(std::vector<GLuint> shaders)
{
    GLuint shaderProgram = glCreateProgram();
    for(GLuint shader : shaders)
        glAttachShader(shaderProgram, shader);
    glLinkProgram(shaderProgram);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        // TODO: Only add infoLog if in DEBUG mode
        ErrorLogger("Failed to link shader program: " + std::string(infoLog));
        glDeleteProgram(shaderProgram);
        ForceTerminate();
        return 0;
    }
    NormalLogger("Linked shader program");
    return shaderProgram;
}
void DeleteShader(GLuint shader)
{
    NormalLogger("Cleaning up shader: " + std::to_string(shader));
    glDeleteShader(shader);
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GL_TRUE);
    NormalLogger("Initialising with OpenGL version: " + std::to_string(OPENGL_MAJOR_VERSION) + "." + std::to_string(OPENGL_MINOR_VERSION));
    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Raytracing In OpenGL", NULL, NULL);
    if(!window)
    {
        ErrorLogger("Failed to create window");
        ForceTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    // vsync
    if(vSync){
        glfwSwapInterval(1);
        NormalLogger("Running with vsync");
    }
    else {
        glfwSwapInterval(0);
        NormalLogger("Running without vsync");
    }
    // load glad
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        ErrorLogger("Failed to initialize GLAD");
        ForceTerminate();
        return EXIT_FAILURE;
    }
    NormalLogger("Loaded glad");
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    GLuint VAO, VBO, EBO;
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);

    glNamedBufferData(VBO, sizeof(ScreenTriVert), ScreenTriVert, GL_STATIC_DRAW);
    glNamedBufferData(EBO, sizeof(ScreenTriIndices), ScreenTriIndices, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

    glEnableVertexArrayAttrib(VAO, 1);
    glVertexArrayAttribBinding(VAO, 1, 0);
    glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(GLfloat) * 5);
    glVertexArrayElementBuffer(VAO, EBO);

    GLuint screenTex;
    glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
    glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureStorage2D(screenTex, 1, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT);
    glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    GLuint screenVertexShader = FileToShader("../src/shaders/ScreenVertexShader.vert", GL_VERTEX_SHADER);
    GLuint screenFragmentShader = FileToShader("../src/shaders/ScreenFragmentShader.frag", GL_FRAGMENT_SHADER);
    GLuint screenShaderProgram = createShaderProgram({screenVertexShader, screenFragmentShader});

    DeleteShader(screenVertexShader);
    DeleteShader(screenFragmentShader);

    GLuint ComputeShader = FileToShader("../src/shaders/ComputeShader.comp", GL_COMPUTE_SHADER);
    GLuint ComputeShaderProgram = createShaderProgram({ComputeShader});

    DeleteShader(ComputeShader);

    int workGroupCurrent[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCurrent[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCurrent[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCurrent[2]);
    NormalLogger("Max work group count: " + std::to_string(workGroupCurrent[0]) + " " + std::to_string(workGroupCurrent[1]) + " " + std::to_string(workGroupCurrent[2]));

    int workGroupSize[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);
    NormalLogger("Max work group size: " + std::to_string(workGroupSize[0]) + " " + std::to_string(workGroupSize[1]) + " " + std::to_string(workGroupSize[2]));

    int workGroupInv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInv);
    NormalLogger("Max work group invocations: " + std::to_string(workGroupInv));

    std::chrono::duration<double> meanFPS;
    auto prevTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    while(!glfwWindowShouldClose(window))
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        frameCount++;
        input_callback(window);
        glUseProgram(ComputeShaderProgram);
        glDispatchCompute(ceil(SCR_WIDTH / 8), ceil(SCR_HEIGHT / 4), 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glUseProgram(screenShaderProgram);
        glBindTextureUnit(0, screenTex);
        glUniform1i(glGetUniformLocation(screenShaderProgram, "screenTex"), 0);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(ScreenTriIndices) / sizeof(ScreenTriIndices[0]), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        if(frameCount % 100 == 0)
        {
            auto currTime = std::chrono::high_resolution_clock::now();
            meanFPS = (currTime - prevTime) / 100;
            prevTime = currTime;
            NormalLogger("FPS: " + std::to_string(1 / meanFPS.count())); 
        }
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &screenTex);
    glDeleteProgram(screenShaderProgram);
    glDeleteProgram(ComputeShaderProgram);
    ErrorLogger("Application Finished, Terminating");
    glfwTerminate();
    return EXIT_SUCCESS;
}
