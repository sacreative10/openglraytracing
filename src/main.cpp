#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <chrono>
#include "logger.h"
#include "GLItems.h"

#include <imgui.h>



const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 400;

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;


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

glm::vec3 cameraPos = glm::vec3(13.0f, 2.0f, 3.0f);
glm::vec3 lookingAt = glm::vec3(0.0f, 0.0f, 0.0f);
bool rotate = false;

bool vSync = true;

void error_callback(int error, const char* description)
{
	logger::Log(logger::LogLevel::ERROR, std::string("GLFW error: ") + description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	logger::Log(logger::LogLevel::INFO, std::string("Framebuffer size changed to ") + std::to_string(width) + "x" + std::to_string(height));
	glViewport(0, 0, width, height);
}

void input_callback(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


// This function generates and binds all the objects arrays. Edit this to your liking.
std::vector<GLuint> alltheobjects(GLfloat (&Vertices)[], GLuint Indicies[])
{
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
	return std::vector<GLuint>{VAO, VBO, EBO};
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GL_TRUE);
	glfwSetErrorCallback(error_callback);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Raytracing In OpenGL", NULL, NULL);
	logger::Log(logger::LogLevel::INFO, std::string("GL window created with OpenGL version ") + std::to_string(OPENGL_MAJOR_VERSION) + "." + std::to_string(OPENGL_MINOR_VERSION));

	if(!window)
	{
		logger::Log(logger::LogLevel::FATAL, std::string("Failed to create GL window"));
		ForceTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);

	// vsync
	if(vSync){
		glfwSwapInterval(1);
		logger::Log(logger::LogLevel::INFO, std::string("V-Sync enabled"));
	}
	else {
		glfwSwapInterval(0);
		logger::Log(logger::LogLevel::INFO, std::string("V-Sync disabled"));
	}

	// loading glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		logger::Log(logger::LogLevel::FATAL, std::string("Failed to initialize GLAD"));
		ForceTerminate();
		return EXIT_FAILURE;
	}

	logger::Log(logger::LogLevel::INFO, std::string("GLAD initialized"));
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	GLuint VAO, VBO, EBO;

	std::vector<GLuint> objects = alltheobjects(ScreenTriVert, ScreenTriIndices);
	VAO = objects[0];
	VBO = objects[1];
	EBO = objects[2];

	GLuint screenTex;
	glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
	glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screenTex, 1, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT);
	glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	GLuint screenVertexShader = loadShader("../src/shaders/ScreenVertexShader.vert", GL_VERTEX_SHADER);
	GLuint screenFragmentShader = loadShader("../src/shaders/ScreenFragmentShader.frag", GL_FRAGMENT_SHADER);
	GLuint screenShaderProgram = createShaderProgram(std::vector<GLuint>{screenVertexShader, screenFragmentShader});

	DeleteGLItem(screenVertexShader);
	DeleteGLItem(screenFragmentShader);

	GLuint ComputeShader = loadShader("../src/shaders/ComputeShader.comp", GL_COMPUTE_SHADER);
	GLuint ComputeShaderProgram = createShaderProgram({ComputeShader});

	DeleteGLItem(ComputeShader);

	int workGroupCurrent[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCurrent[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCurrent[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCurrent[2]);
	logger::Log(logger::LogLevel::DEBUG, std::string("Max work group count: ") + std::to_string(workGroupCurrent[0]) + " " + std::to_string(workGroupCurrent[1]) + " " + std::to_string(workGroupCurrent[2]));

	int workGroupSize[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);
	logger::Log(logger::LogLevel::DEBUG, std::string("Max work group size: ") + std::to_string(workGroupSize[0]) + " " + std::to_string(workGroupSize[1]) + " " + std::to_string(workGroupSize[2]));

	int workGroupInv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInv);
	logger::Log(logger::LogLevel::DEBUG, std::string("Max work group invocations: ") + std::to_string(workGroupInv));

	std::chrono::duration<double> meanFPS;
	auto startTime = std::chrono::high_resolution_clock::now();
	int frameCount = 0;

	bool show_demo_window = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO(); 
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	int MAXDEPTH = 2;
	int NUM_SAMPLES = 2;

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		frameCount++;
		input_callback(window);
		if(rotate){
			float angle = 0.05f;
			glm::mat4 rotationMatrix = glm::mat4(cos(angle), 0.0, sin(angle), 0.0,
					0.0, 1.0,        0.0, 0.0,
					-sin(angle),  0.0, cos(angle), 0.0,
					0.0,  0.0,        0.0, 1.0);
			cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos, 1.0));
		}
		glUseProgram(ComputeShaderProgram);
		// time elapsed since the beginning of the program
		glUniform1f(glGetUniformLocation(ComputeShaderProgram, "time"), glfwGetTime());
		glUniform3f(glGetUniformLocation(ComputeShaderProgram, "lookFrom"), cameraPos.x, cameraPos.y, cameraPos.z);
		glUniform3f(glGetUniformLocation(ComputeShaderProgram, "lookAt"), lookingAt.x, lookingAt.y, lookingAt.z);
		glUniform1iv(glGetUniformLocation(ComputeShaderProgram, "MAXDEPTHi"), 1, &MAXDEPTH);
		glUniform1iv(glGetUniformLocation(ComputeShaderProgram, "NUMSAMPLESi"), 1, &NUM_SAMPLES);
		glDispatchCompute(ceil(SCR_WIDTH / 8), ceil(SCR_HEIGHT / 4), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glUseProgram(screenShaderProgram);
		glBindTextureUnit(0, screenTex);
		glUniform1i(glGetUniformLocation(screenShaderProgram, "screenTex"), 0);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(ScreenTriIndices) / sizeof(ScreenTriIndices[0]), GL_UNSIGNED_INT, 0);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Checkbox("Rotate", &rotate);
		ImGui::Text("Camera Position: %.3f %.3f %.3f", cameraPos.x, cameraPos.y, cameraPos.z);
		ImGui::Text("Looking At: %.3f %.3f %.3f", lookingAt.x, lookingAt.y, lookingAt.z);
		ImGui::SliderFloat3("Camera Position", &cameraPos.x, -10.0f, 10.0f);
		ImGui::SliderFloat3("Looking At", &lookingAt.x, -10.0f, 10.0f);
		
		ImGui::Text("Max Depth: %d", MAXDEPTH);
		ImGui::SliderInt("Max Depth", &MAXDEPTH, 1, 10);

		ImGui::Text("Number of Samples: %d", NUM_SAMPLES);
		ImGui::SliderInt("Number of Samples", &NUM_SAMPLES, 1, 10);

		ImGui::End();
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &screenTex);
	glDeleteProgram(screenShaderProgram);
	glDeleteProgram(ComputeShaderProgram);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	logger::Log(logger::LogLevel::DEBUG, "Shutting down...");
	return EXIT_SUCCESS;
}
