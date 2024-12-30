// 28_GouraudShading: ambient + diffuse + specular lighting in Phone lighting model
//                  : using Gouraud Shading algorithm
//          : Mouse left button: arcball control for the object
//          : Keyboard 'a': to switch between view and camera rotation modes
//          : Keyboard 'r': to reset the arcball

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>
#define M_PI       3.14159265358979323846   // pi


#include <shader.h>
#include <cube.h>
#include <arcball.h>


using namespace std;

// Function Prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double x, double y);
void loadTexture();
void render();
void drawHexagonalPrism();
void dynamic_vertice_mapping();

// Global variables
GLFWwindow *mainWindow = NULL;
Shader *globalShader = NULL;
Shader *lampShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
Cube *cube;
Cube *lamp;
glm::mat4 projection, view, model;

// for arcball
float arcballSpeed = 0.2f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true );
static Arcball modelArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
bool arcballCamRot = true;

// for camera
glm::vec3 cameraPos(0.0f, 3.0f, 7.0f);
glm::vec3 camTarget(0.0f, 0.0f, 0.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);

// for lighting
glm::vec3 lightSize(0.2f, 0.2f, 0.2f);
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
float ambientStrenth = 0.1f;
float specularStrength = 0.5f;
float specularPower = 64.0f;

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
    globalShader = new Shader("basic_lighting.vs", "basic_lighting.fs");
    lampShader = new Shader("lamp.vs", "lamp.fs");
    
    // projection matrix
    projection = glm::perspective(glm::radians(45.0f),
                                  (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    globalShader->use();
    globalShader->setMat4("projection", projection);
    globalShader->setVec3("objectColor", objectColor);
    globalShader->setVec3("lightColor", lightColor);
    globalShader->setVec3("lightPos", lightPos);
    view = glm::lookAt(cameraPos, camTarget, camUp);
    globalShader->setMat4("view", view);
    globalShader->setFloat("ambientStrenth", ambientStrenth);
    globalShader->setFloat("specularStrength", specularStrength);
    globalShader->setFloat("specularPower", specularPower);
    
    lampShader->use();
    lampShader->setMat4("projection", projection);
    
    // cube initialization
    cube = new Cube();
    lamp = new Cube();
    
    cout << "ARCBALL: camera rotation mode" << endl;

    // load texture
    loadTexture();

    // generate hexagonal prism vertices, normals, colors, and texture coordinates
    dynamic_vertice_mapping();
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(mainWindow)) {
        render();
        glfwPollEvents();
    }
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

GLFWwindow *glAllInit()
{
    GLFWwindow *window;
    
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gouraud Shading", NULL, NULL);
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

    // Set orange color
    unsigned char orange[3] = { 1.0f, 0.5f, 0.31f   };

    // Assign the color to the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &orange);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    view = view * camArcBall.createRotationMatrix();
    
    // cube object
    globalShader->use();
    globalShader->setMat4("view", view);
    model = glm::mat4(1.0f);
    model = model * modelArcBall.createRotationMatrix();
    globalShader->setMat4("model", model);
    glBindTexture(GL_TEXTURE_2D, texture);
    drawHexagonalPrism();

    
    // lamp
    lampShader->use();
    lampShader->setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, lightSize);
    lampShader->setMat4("model", model);
    cube->draw(lampShader);
    
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
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        camArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
        modelArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        arcballCamRot = !arcballCamRot;
        if (arcballCamRot) {
            cout << "ARCBALL: Camera rotation mode" << endl;
        }
        else {
            cout << "ARCBALL: Model  rotation mode" << endl;
        }
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (arcballCamRot)
        camArcBall.mouseButtonCallback( window, button, action, mods );
    else
        modelArcBall.mouseButtonCallback( window, button, action, mods );
}

void cursor_position_callback(GLFWwindow *window, double x, double y) {
    if (arcballCamRot)
        camArcBall.cursorCallback( window, x, y );
    else
        modelArcBall.cursorCallback( window, x, y );
}
