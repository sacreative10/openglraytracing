#include "include/glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>


void error_callback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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
        std::cerr << "Failed to open file: " << filename << std::endl;
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
        std::cerr << "Failed to compile shader: " << filename << std::endl;
        std::cerr << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    std::cout << "Loaded shader: " << filename << std::endl;
    return shader;
}

GLuint createShaderProgram(const GLuint& vertexShader, const GLuint& fragmentShader)
{
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Failed to link shader program" << std::endl;
        std::cerr << infoLog << std::endl;
        glDeleteProgram(shaderProgram);
        return 0;
    }
    std::cout << "Created shader program" << std::endl;
    return shaderProgram;
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GL_TRUE);

    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Raytracing In OpenGL", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    // load glad
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        error_callback(0, "glad load failed");
        return EXIT_FAILURE;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glViewport(0, 0, 800, 600);

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

    GLuint screenVertexShader = FileToShader("../src/shaders/ScreenVertexShader.vert.glsl", GL_VERTEX_SHADER);
    GLuint screenFragmentShader = FileToShader("../src/shaders/ScreenFragmentShader.frag.glsl", GL_FRAGMENT_SHADER);
    GLuint screenShaderProgram = createShaderProgram(screenVertexShader, screenFragmentShader);

    glDeleteShader(screenVertexShader);
    glDeleteShader(screenFragmentShader);


    GLuint ComputeShader = FileToShader("../src/shaders/ComputeShader.comp.glsl", GL_COMPUTE_SHADER);

    GLuint ComputeShaderProgram = createShaderProgram(ComputeShader, (GLuint)0);

    int workGroupCurrent[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCurrent[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCurrent[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCurrent[2]);
    std::cout << "Max work group count per compute shader: " << workGroupCurrent[0] << " " << workGroupCurrent[1] << " " << workGroupCurrent[2] << std::endl;

    int workGroupSize[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);
    std::cout << "Max work group size: " << workGroupSize[0] << " " << workGroupSize[1] << " " << workGroupSize[2] << std::endl;

    int workGroupInv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInv);
    std::cout << "Max work group invocations: " << workGroupInv << std::endl;

    while(!glfwWindowShouldClose(window))
    {
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
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &screenTex);
    glDeleteProgram(screenShaderProgram);
    glDeleteProgram(ComputeShaderProgram);
    glfwTerminate();
    return EXIT_SUCCESS;
}
