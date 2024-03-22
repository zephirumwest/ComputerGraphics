#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
"}\n\0";
const char* fragmentShader2Source = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.5f, 0.0f, 0.5f, 1.0f);\n"
"}\n\0";

bool fillMode = true;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HelloShapes", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialisation failed!");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // build and compile our shader program
    // ------------------------------------
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    unsigned int shaderProgram = glCreateProgram();
    unsigned int shaderProgram2 = glCreateProgram();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glShaderSource(fragmentShader2, 1, &fragmentShader2Source, NULL);
    glCompileShader(fragmentShader2);
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);
    glUseProgram(shaderProgram2);

    // set up vertex data and configure vertex attributes
    // --------------------------------------------------
    const float PI = 3.14159265359f;
    const int numTriangles = 64;
    float radius = 0.5f;
    float circleVertices[numTriangles * 3 * 2 + 1]; // Each triangle has 3 vertices, and each vertex has 2 coordinates (x, y)

    // Center vertex
    const float circleCenterX = -0.5f;
    const float circleCenterY = 0.0f;

    circleVertices[0] = circleCenterX;
    circleVertices[1] = circleCenterY;
    circleVertices[2] = 0.0f;

    for (int i = 0; i < numTriangles + 1; i++) {
        float angle = 2 * PI * i / numTriangles;
        circleVertices[(i + 1) * 3] = circleCenterX + radius * cos(angle);
        circleVertices[(i + 1) * 3 + 1] = circleCenterY + radius * sin(angle);
        circleVertices[(i + 1) * 3 + 2] = 0.0f;
    }

    // Hexagon vertices with updated initial position
    const float hexagonCenterX = 0.5f;
    const float hexagonCenterY = 0.0f;

    double root = sqrt(3) / 10;
    float hexagonVertices[] = {
        0.1f + hexagonCenterX,  root + hexagonCenterY, 0.0f,
        -0.1f + hexagonCenterX, root + hexagonCenterY, 0.0f,
        0.2f + hexagonCenterX, 0.0f + hexagonCenterY, 0.0f,
        -0.2f + hexagonCenterX,  0.0f + hexagonCenterY, 0.0f,
        0.1f + hexagonCenterX, -root + hexagonCenterY, 0.0f,
        -0.1f + hexagonCenterX, -root + hexagonCenterY, 0.0f
    };

    unsigned int VBO_circle, VBO_hexagon, VAO_circle, VAO_hexagon;
    glGenVertexArrays(1, &VAO_circle);
    glGenBuffers(1, &VBO_circle);
    glGenVertexArrays(1, &VAO_hexagon);
    glGenBuffers(1, &VBO_hexagon);

    // Circle setup
    glBindVertexArray(VAO_circle);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_circle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Hexagon setup
    glBindVertexArray(VAO_hexagon);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_hexagon);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hexagonVertices), hexagonVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (fillMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // draw circle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO_circle);
        glDrawArrays(GL_TRIANGLE_FAN, 0, numTriangles + 2);

        //
                // draw hexagon
        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO_hexagon);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO_circle);
    glDeleteBuffers(1, &VBO_circle);
    glDeleteVertexArrays(1, &VAO_hexagon);
    glDeleteBuffers(1, &VBO_hexagon);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

// glfw: whenever a key is pressed, this callback is called
// -------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        fillMode = !fillMode;
    }
}
