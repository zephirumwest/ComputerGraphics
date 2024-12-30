
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <shader.h>

using namespace std;

// Function Prototypes
GLFWwindow* glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void render();

// Global variables
GLFWwindow* window = NULL;
Shader* globalShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
unsigned int VBO, VAO;
float vertices[] = {
    0.025f,  0.025f, // top right
    0.025f, -0.025f, // bottom right
    -0.025f, -0.025f, // bottom left
    -0.025f,  0.025f  // top left
};

float speed1 = glm::radians(360.0f);  // 360 degrees/sec for the frectangle
float speed2 = glm::radians(90.0f);   // 90 degrees/sec for the rectangle
float speed3 = glm::radians(270.0f);  // 270 degrees/sec for the rectangle
float speed4 = glm::radians(180.0f); // 180 degrees/sec for the  rectangle

int main()
{
    window = glAllInit();

    globalShader = new Shader("4.3.transform.vs", "4.3.transform.fs");

    // render loop
    while (!glfwWindowShouldClose(window)) {
        render();
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Homework05", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Allow modern extension features
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cout << "GLEW initialisation failed!" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(-1);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return window;
}

void render()
{
    float currentTime = glfwGetTime();
    glm::mat4 transform;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    globalShader->use();

    // green rectangle
    transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, speed1 * currentTime, glm::vec3(0.0f, 0.0f, 1.0f));

    globalShader->setMat4("transform", transform);
    globalShader->setVec4("inColor", 0.0f, 1.0f, 0.0f, 1.0f);

    // draw rectangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //yellow rectangle
    float spiral = sin(currentTime/3);
    transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, speed2 * currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, glm::vec3(0.5f * spiral, 0.0f, 0.0f));

    globalShader->setMat4("transform", transform);
    globalShader->setVec4("inColor", 1.0f, 1.0f, 0.0f, 1.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //red rectangle
    transform = glm::rotate(transform, speed4 * currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, glm::vec3(0.0f, 0.1f, 0.0f));
    transform = glm::scale(transform, glm::vec3(1.0f, 4.0f, 1.0f));

    globalShader->setMat4("transform", transform);
    globalShader->setVec4("inColor", 1.0f, 0.0f, 0.0f, 1.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


    glfwSwapBuffers(window);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
