// 01_ImmediateMode
// Immediate Mode Example using glfw

#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void draw_donut();

// window size
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

bool isTurn = false;
bool isReset = false;
bool isRestart = false;
bool isReTurn = false;

bool saveStopTime = false;
double stopTime = 0;


bool isSpeedUp = false;
bool isSpeedDown = false;
double speed = 1;
double a = 0;
double b = 0;

int main()
{   
    // glfw: initialize and configure
    // ------------------------------
    if (!glfwInit()) {
        std::cout << "Failed to initiallize GLFW" << std::endl;
        return -1;
    }; 

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "01_1_Immediate Mode", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // we don't have to use GLEW library in this program
    // cause immediate mode doesn't need any modern OpenGL features (core profile)
    
    // main loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        // setting the camera
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        float ratio = SCR_WIDTH / (float)SCR_HEIGHT;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // orthographic projection of NDC
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        draw_donut();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void draw_donut() {
    float x[] = { -0.23f, -0.5f, 0.23f, 0.5f, 0.23f, 0.5f, -0.23f, -0.5f, -0.23f, -0.5f };
    float y[] = { -0.23f, -0.5f, -0.23f, -0.5f, 0.23f, 0.5f, 0.23f, 0.5f, -0.5f, -0.5f };

    double timeValue = glfwGetTime();
    double time = stopTime + timeValue;
    double sptime = time * speed;
    
    glBegin(GL_TRIANGLE_STRIP);
    
    glColor3f(1.f, 0.64f, 0.f);
    
    if (saveStopTime) {
		b = b + sptime;
		saveStopTime = false;
	}

    if (isRestart) {
        glfwSetTime(0);
        isRestart = false;
    }

    if (isSpeedUp && isTurn) {
        a = a + sptime;
        stopTime = 0;
        speed += 0.4;
        isSpeedUp = false;
    }

    if (isSpeedDown && isTurn) {
        a = a + sptime;
        stopTime = 0;
        speed -= 0.4;
        isSpeedDown = false;
    }


    if (isReset) {
        a = 0;
        b = 0;
        stopTime = 0;
        glfwSetTime(0);

        for (int i = 0; i < 10; i++) {
            double cx = x[i] * sin(stopTime) + y[i] * cos(stopTime);
            double cy = y[i] * sin(stopTime) - x[i] * cos(stopTime);

            glVertex3f(cx, cy, .0f);
        }
        isTurn = false;
        isReset = false;
    }

    if (isTurn && !isReset) {
        isReset = false;
        
        for (int i = 0; i < 10; i++) {
            double cx = x[i] * sin(a + b + sptime) + y[i] * cos(a + b + sptime);
            double cy = y[i] * sin(a + b + sptime) - x[i] * cos(a + b + sptime);
            
            glVertex3f(cx, cy, .0f);
        }
    }

    else if(!isTurn){

        for (int i = 0; i < 10; i++) {
            double cx = x[i] * sin(a + b) + y[i] * cos(a + b);
            double cy = y[i] * sin(a + b) - x[i] * cos(a + b);

            glVertex3f(cx, cy, .0f);
        }
    }


    glEnd();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (isTurn){    
            saveStopTime = true;
            isTurn = false;           
        }
        else {
            isRestart = true;
            isTurn = true;
        }
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        isReset = true;
    }

    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS && speed < 1.8) {
       isRestart = true;
       isSpeedUp = true;    
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS && speed > 0.2) {
        isRestart = true;
        isSpeedDown = true; 
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
