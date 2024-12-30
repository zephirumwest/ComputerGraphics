#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>

#include <shader.h>
#include <arcball.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


using namespace std;

// Function Prototypes
GLFWwindow* glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void loadTexture();
void render();
void drawHexagonalPrism();
void dynamic_vertice_mapping();

// Global variables
GLFWwindow* mainWindow = NULL;
Shader* globalShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
glm::mat4 projection, view, model;
glm::vec3 camPosition(0.0f, 3.0f, 7.0f);
glm::vec3 camTarget(0.0f, 0.0f, 0.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);

// for arcball
float arcballSpeed = 0.3f;
static Arcball modelArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);

// for texture
static unsigned int texture; // Array of texture ids.

// Hexagonal Prism vertices, normals, colors, and texture coordinates
GLfloat hexVertices[72];
GLfloat hexNormals[72];
GLfloat hexColors[96];
GLfloat hexTexCoords[48];

// index array for glDrawElements()
unsigned int hexIndices[36] = {
    0, 1, 2, 0, 2, 3,
    4, 5, 6, 4, 6, 7,
    8, 9, 10, 8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23
};

int main()
{
    mainWindow = glAllInit();

    // shader loading and compile (by calling the constructor)
    globalShader = new Shader("global.vs", "global.fs");

    // projection and view matrix
    globalShader->use();
    projection = glm::perspective(glm::radians(45.0f),
        (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    globalShader->setMat4("projection", projection);
    view = glm::lookAt(camPosition, camTarget, camUp);
    globalShader->setMat4("view", view);

    // load texture
    loadTexture();

    // generate hexagonal prism vertices, normals, colors, and texture coordinates
    dynamic_vertice_mapping();

    while (!glfwWindowShouldClose(mainWindow)) {
        render();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow* glAllInit()
{
    GLFWwindow* window;

    // glfw: initialize and configure
    if (!glfwInit()) {
        printf("GLFW initialisation failed!");
        glfwTerminate();
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // glfw window creation
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Texture 1", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // OpenGL states
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Allow modern extension features
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cout << "GLEW initialisation failed!" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(-1);
    }

    return window;
}

void loadTexture() {

    // Create texture ids.
    glGenTextures(1, &texture);

    // All upcomming GL_TEXTURE_2D operations now on "texture" object
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* image = stbi_load("world_map.jpg", &width, &height, &nrChannels, 0);
    if (!image) {
        printf("texture %s loading error ... \n", "world_map.jpg");
    }
    else printf("texture %s loaded\n", "world_map.jpg");

    GLenum format;
    if (nrChannels == 1) format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free image memory
    stbi_image_free(image);
}

void render() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model = modelArcBall.createRotationMatrix();

    globalShader->use();
    globalShader->setMat4("model", model);

    glBindTexture(GL_TEXTURE_2D, texture);

    drawHexagonalPrism();

    glfwSwapBuffers(mainWindow);
}

void drawHexagonalPrism() {
    // Bind VAO, VBO, EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hexVertices) + sizeof(hexNormals) + sizeof(hexColors) + sizeof(hexTexCoords), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hexVertices), hexVertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(hexVertices), sizeof(hexNormals), hexNormals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(hexVertices) + sizeof(hexNormals), sizeof(hexColors), hexColors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(hexVertices) + sizeof(hexNormals) + sizeof(hexColors), sizeof(hexTexCoords), hexTexCoords);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hexIndices), hexIndices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(hexVertices)));
    glEnableVertexAttribArray(1);

    // Color attribute
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(hexVertices) + sizeof(hexNormals)));
    glEnableVertexAttribArray(2);

    // Texture attribute
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(hexVertices) + sizeof(hexNormals) + sizeof(hexColors)));
    glEnableVertexAttribArray(3);

    // Draw the hexagonal prism
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Clean up
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void dynamic_vertice_mapping() {
    for (int i = 0; i < 6; i++) {
        hexVertices[i * 12] = sin(i * M_PI / 3);
        hexVertices[i * 12 + 1] = -1; // y = -1
        hexVertices[i * 12 + 2] = cos(i * M_PI / 3);
        hexVertices[i * 12 + 3] = sin(i * M_PI / 3);
        hexVertices[i * 12 + 4] = 1; // y = 1
        hexVertices[i * 12 + 5] = cos(i * M_PI / 3);
        hexVertices[i * 12 + 6] = sin((i + 1) * M_PI / 3);
        hexVertices[i * 12 + 7] = 1;
        hexVertices[i * 12 + 8] = cos((i + 1) * M_PI / 3);
        hexVertices[i * 12 + 9] = sin((i + 1) * M_PI / 3);
        hexVertices[i * 12 + 10] = -1;
        hexVertices[i * 12 + 11] = cos((i + 1) * M_PI / 3);
    }

    for (int i = 0; i < 6; i++) {
        hexNormals[i * 12] = 0;
        hexNormals[i * 12 + 1] = 0;
        hexNormals[i * 12 + 2] = -1;
        hexNormals[i * 12 + 3] = 0;
        hexNormals[i * 12 + 4] = 0;
        hexNormals[i * 12 + 5] = 1;
        hexNormals[i * 12 + 6] = 0;
        hexNormals[i * 12 + 7] = 0;
        hexNormals[i * 12 + 8] = 1;
        hexNormals[i * 12 + 9] = 0;
        hexNormals[i * 12 + 10] = 0;
        hexNormals[i * 12 + 11] = -1;
    }

    for (int i = 0; i < 6; i++) {
        hexColors[i * 16] = 1;
        hexColors[i * 16 + 1] = 0;
        hexColors[i * 16 + 2] = 0;
        hexColors[i * 16 + 3] = 1;
        hexColors[i * 16 + 4] = 1;
        hexColors[i * 16 + 5] = 0;
        hexColors[i * 16 + 6] = 0;
        hexColors[i * 16 + 7] = 1;
        hexColors[i * 16 + 8] = 0;
        hexColors[i * 16 + 9] = 1;
        hexColors[i * 16 + 10] = 0;
        hexColors[i * 16 + 11] = 1;
        hexColors[i * 16 + 12] = 0;
        hexColors[i * 16 + 13] = 0;
        hexColors[i * 16 + 14] = 1;
        hexColors[i * 16 + 15] = 1;
    }

    for (int i = 0; i < 6; i++) {
        hexTexCoords[i * 8] = i / 6.;
        hexTexCoords[i * 8 + 1] = 1.;
        hexTexCoords[i * 8 + 2] = i / 6.;
        hexTexCoords[i * 8 + 3] = 0.;
        hexTexCoords[i * 8 + 4] = (i + 1) / 6.;
        hexTexCoords[i * 8 + 5] = 0.;
        hexTexCoords[i * 8 + 6] = (i + 1) / 6.;
        hexTexCoords[i * 8 + 7] = 1.;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        modelArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    modelArcBall.mouseButtonCallback(window, button, action, mods);
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
    modelArcBall.cursorCallback(window, x, y);
}
