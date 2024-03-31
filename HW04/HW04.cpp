#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <shader.h>

using namespace std;


// function prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void key_callback(GLFWwindow *window, int button, int action, int mods);
void normalize_cursor_position(double x, double y, float &nx, float &ny);
void update_vb_vertex(int vlindex, float x, float y);
void compute_contact();

// global variables
GLFWwindow *window = NULL;
Shader *ourShader;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
bool dragging = false;
bool complete = false;
int nInter = 0;
float interV[4];
unsigned int interVAO, VAO[2];
unsigned int interVBO, VBO[2];
float quadFunc[130];
float lineVer[4];


int main()
{
    // window creation
    window = glAllInit();


    // build and compile our shader program
    // you can name your shader files however you like
    // ------------------------------------
    ourShader = new Shader("3.2.shader.vs", "3.2.shader.fs");


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    for (int i = 0; i < 129; i += 2) {
        quadFunc[i] = (i - float(64)) / float(64);
        quadFunc[i + 1] = (2.0f * float(pow(quadFunc[i], 2))) - float(0.8f * quadFunc[i]) - 0.42f;
    }


    glGenVertexArrays(1, &interVAO);
    glGenBuffers(1, &interVBO);
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadFunc), quadFunc, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(VAO[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVer), lineVer, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ourShader->use();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) 
    {
        //render
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader->setVec4("inColor", 1.0f, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_LINE_STRIP, 0, 65);
        glBindVertexArray(0);

        ourShader->setVec4("inColor", 0.0f, 1.0f, 0.0f, 1.0f);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);

        if (nInter > 0) {
            glPointSize(10.0f);
            ourShader->setVec4("inColor", 1.0f, 1.0f, 0.0f, 1.0f);
            glBindVertexArray(interVAO);
            glDrawArrays(GL_POINTS, 0, nInter);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteVertexArrays(1, &interVAO);
    glDeleteBuffers(1, &interVBO);
    glfwTerminate();
    return 0;
}

GLFWwindow *glAllInit()
{
    GLFWwindow *window;
    
    // glfw: initialize and configure
    // ------------------------------
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
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Homework04", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, key_callback);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    
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

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (!complete) {
        float nx, ny;
        if (dragging) {
            normalize_cursor_position(xpos, ypos, nx, ny);
            update_vb_vertex(1, nx, ny);
        }
    }
}

// glfw: keyboard callback
// ---------------------------------------------------------------------------------------------
void key_callback(GLFWwindow *window, int button, int action, int mods)
{
    double xpos, ypos;
    float nx, ny;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS && !complete) {
            glfwGetCursorPos(window, &xpos, &ypos);
            normalize_cursor_position(xpos, ypos, nx, ny);
            lineVer[0] = nx;
            lineVer[1] = ny;
            update_vb_vertex(0, nx, ny); 
            update_vb_vertex(1, nx, ny);
            dragging = true;
        }
        else if (action == GLFW_RELEASE && !complete) {
            glfwGetCursorPos(window, &xpos, &ypos);
            normalize_cursor_position(xpos, ypos, nx, ny);
            lineVer[2] = nx;
            lineVer[3] = ny;
            update_vb_vertex(1, nx, ny);
            dragging = false;
            complete = true;
            compute_contact();
        }
    }
}

void normalize_cursor_position(double x, double y, float &nx, float &ny)
{
    nx = ((float)x / (float)SCR_WIDTH) * 2.0f - 1.0f;
    ny = -1.0f * (((float)y / (float)SCR_HEIGHT) * 2.0f - 1.0f);
}

void update_vb_vertex(int vindex, float x, float y)
{
    float n[2];
    n[0] = x;
    n[1] = y;
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferSubData(GL_ARRAY_BUFFER, vindex * 2 * sizeof(float), sizeof(float) * 2, n);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void compute_contact() {
    float a = lineVer[2] - lineVer[0],
          b = lineVer[0],
          c = lineVer[3] - lineVer[1],
          d = lineVer[1];
    float A = float(pow(a, 2)) * 2.0f,
          B = (4.0f * a * b) - (0.8f * a) - c,
          C = (float(pow(b, 2)) * 2.0f) - (0.8f * b) - d - 0.42f;
    float D = float(pow(B, 2)) - (4 * A * C);
    int vindex = 0;
    if (D < 0) return;
    if (D > 0) {
        float t1 = (float(sqrt(D)) - B) / (2 * A),
              t2 = (float(sqrt(D)) + B) / (2 * A);
        t2 = -t2;
        if (0 <= t1 && t1 <= 1) {
            interV[vindex++] = a * t1 + b;
            interV[vindex++] = c * t1 + d;
            nInter++;
        }
        if (0 <= t2 && t2 <= 1) {
            interV[vindex++] = a * t2 + b;
            interV[vindex++] = c * t2 + d;
            nInter++;
        }
    }
    else if (D == 0) {
        float t1 = B / (2 * A);
        B = -B;
        if (0 <= t1 && 1 <= t1) {
            interV[vindex++] = a * t1 + b;
            interV[vindex++] = c * t1 + d;
            nInter++;
        }
    }

    glBindVertexArray(interVAO);
    glBindBuffer(GL_ARRAY_BUFFER, interVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interV), interV, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}
