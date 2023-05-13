#include "shader_s.h"
#include "model.h"

#include <filesystem>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.f, 0.f, -1.f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.f, 0.0f);

// key
bool lButtonClicked = false;
bool firstMouse = true;
float yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// lighting
glm::vec3 lightPos(0.f, 4.f, 10.0f);

// imgui parameter
float bunny_size = 1.f;
glm::vec3 bunny_color = glm::vec3(1.0f, 1.0f, 0.0f);
float bunny_y = 0.f;
bool isMouseHoverUi = false;
bool isColorChanging = false;

int main(int argc, char* argv[])
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "The Standford Bunny", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	auto imguiContext = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();


	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// #1 FLOOR
	// build and compile floor shader program
	// ------------------------------------
	const std::string floorVsPath = RESOURCE_DIRECTORY + std::string("/shader/floor_shader.vs");
	const std::string floorFsPath = RESOURCE_DIRECTORY + std::string("/shader/floor_shader.fs");
	Shader floorShader(floorVsPath.c_str(), floorFsPath.c_str());

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// point	// texture
		0.5f, 0.5f, 0.0f, 10.f, 10.f, // top right
		0.5f, -0.5f, 0.0f, 10.f, 0.f, // bottom right
		-0.5f, -0.5f, 0.0f, 0.f, 0.f, // bottom left
		-0.5f, 0.5f, 0.0f, 0.f, 10.f  // top left
	};
	unsigned int indices[] = {
		// note that we start from 0!
		0, 1, 3, // first Triangle
		1, 2, 3	 // second Triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// load and create a texture
	// -------------------------
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	const std::string texturePath = RESOURCE_DIRECTORY + std::string("/texture/wood.png");
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// #2 BUNNY
	// build and compile floor shader program
	// ------------------------------------
	const std::string bunnyVsPath = RESOURCE_DIRECTORY + std::string("/shader/model_loading.vs");
	const std::string bunnyFsPath = RESOURCE_DIRECTORY + std::string("/shader/model_loading.fs");
	Shader bunnyShader(bunnyVsPath.c_str(), bunnyFsPath.c_str());

	// load models
	// -----------
	string objectFileName = argv[1];
	const std::string modelPath = RESOURCE_DIRECTORY + std::string("/model/") + objectFileName;
	Model bunnyModel(modelPath);

	// #0 COMMON
	// pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
	// -----------------------------------------------------------------------------------------------------------
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	floorShader.use();
	floorShader.setMat4("projection", projection);

	bunnyShader.use();
	bunnyShader.setMat4("projection", projection);



	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// imgui
		// -----
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (ImGui::Begin("Bunny Interface"))
		{
			ImGui::Text("Use WASD and left mouse button to change the viewpoint.");
			ImGui::SliderFloat("Size", &bunny_size, 0.f, 3.f);

			if (ImGui::ColorEdit3("Color", glm::value_ptr(bunny_color)))
			{
				isColorChanging = true;
			}
			else
			{
				if (isColorChanging && lButtonClicked)
				{
					isColorChanging = true;
				}
				else
				{
					isColorChanging = false;
				}
			}

			if (ImGui::IsWindowHovered())
			{
				isMouseHoverUi = true;
			}
			else
			{
				isMouseHoverUi = false;
			}
		}
		ImGui::End();

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		// #0. COMMON
		glm::vec3 eye = cameraPos;
		glm::vec3 center = cameraPos + cameraFront;
		glm::vec3 up = cameraUp;
		glm::mat4 view = glm::lookAt(eye, center, up);

		floorShader.use();
		floorShader.setMat4("view", view);

		bunnyShader.use();
		bunnyShader.setMat4("view", view);

		// #1. FLOOR
		floorShader.use();

		// bind Texture
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);

		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::scale(model, glm::vec3(50.f, 50.f, 50.f));
		model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
		floorShader.setMat4("model", model);

		floorShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		floorShader.setVec3("lightPos", lightPos);
		floorShader.setVec3("viewPos", cameraPos);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// #2. BUNNY
		bunnyShader.use();

		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::scale(model, glm::vec3(bunny_size, bunny_size, bunny_size));
		model = glm::translate(model, glm::vec3(0.f, 1.6f, 0.f));
		bunnyShader.setMat4("model", model);

		bunnyShader.setVec3("ObjColor", bunny_color);
		bunnyShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		bunnyShader.setVec3("lightPos", lightPos);
		bunnyShader.setVec3("viewPos", cameraPos);

		bunnyModel.Draw(bunnyShader);

		// ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// de-allocate all resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	ImGui_ImplOpenGL3_DestroyFontsTexture();
	ImGui_ImplOpenGL3_DestroyDeviceObjects();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(imguiContext);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

using namespace std;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 0.01f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		cout << isMouseHoverUi << endl;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (!lButtonClicked)
		return;

	if (isMouseHoverUi)
		return;

	if (isColorChanging)
		return;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		firstMouse = true;
		lButtonClicked = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		lButtonClicked = false;
	}
}