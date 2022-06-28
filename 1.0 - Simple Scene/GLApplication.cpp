// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <cstring>
#include <fstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// Local / project headers
#include "../Common/Scene.h"
#include "shader.hpp"
#include "SimpleScene_Quad.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

//Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Function declarations
bool savePPMImageFile(std::string& filepath, std::vector<GLfloat>& pixels, int width, int height);

//////////////////////////////////////////////////////////////////////
GLFWwindow* window;
Scene* scene;
SimpleScene_Quad* sceneQuad;
int windowWidth = 1280;
int windowHeight = 960;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	// Setting up OpenGL properties
	glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfwSwapInterval(1);
	// Open a window and create its OpenGL context
	window = glfwCreateWindow(windowWidth, windowHeight, // window dimensions
		"Sample 1 - Simple scene (Quad) with Scene Class", // window title
		nullptr, // which monitor (if full-screen mode)
		nullptr); // if sharing context with another window
	if (window == nullptr)
	{
		fprintf(stderr,
			"Failed to open GLFW window. If you have an Intel GPU, they are not 4.0 compatible.\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// OpenGL resource model - "glfwCreateWindow" creates the necessary data storage for the OpenGL
	// context but does NOT make the created context "current". We MUST make it current with the following
	// call
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	// Initialize GLEW
	glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Initialize the scene
	scene = new SimpleScene_Quad(windowWidth, windowHeight);
	sceneQuad = dynamic_cast<SimpleScene_Quad*>(scene);
	// Scene::Init encapsulates setting up the geometry and the texture
	// information for the scene
	scene->Init();

	// -----------
	do
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);
		// Now render the scene
		// Scene::Display method encapsulates pre-, render, and post- rendering operations
		scene->Display();

		glfwPollEvents();
		// Swap buffers
		glfwSwapBuffers(window);
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup IMGUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	scene->CleanUp();
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	delete scene;
	return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

bool savePPMImageFile(std::string& filepath, std::vector<GLfloat>& pixels, int width, int height)
{
	std::ofstream  texFile(filepath);

	texFile << "P3" << std::endl;
	texFile << width << "  " << height << std::endl;
	texFile << "255" << std::endl;

	auto it = pixels.begin();

	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			texFile << *it++ << " ";
			texFile << *it++ << " ";
			texFile << *it++ << " ";
		}

		texFile << std::endl;
	}

	texFile.close();

	return true;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		sceneQuad->GetCamera()->ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		sceneQuad->GetCamera()->ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		sceneQuad->GetCamera()->ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		sceneQuad->GetCamera()->ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		sceneQuad->GetCamera()->ProcessKeyboard(Camera_Movement::UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		sceneQuad->GetCamera()->ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	sceneQuad->GetCamera()->width = (float)width;
	sceneQuad->GetCamera()->height = (float)height;
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;
	sceneQuad->GetCamera()->ProcessMouseScroll(static_cast<float>(yoffset));
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	auto io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (state == GLFW_PRESS)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		sceneQuad->GetCamera()->ProcessMouseMovement(xoffset, yoffset);
	}
	else
	{
		lastX = xpos;
		lastY = ypos;
	}
	
}

