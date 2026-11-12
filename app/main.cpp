// @formatter:off
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
// @formatter:on

// function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void update(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    if(!glfwInit()){
        std::cout << "Failed to init GLFW!\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window 
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "first week", nullptr, nullptr);
    if(window == nullptr){
        std::cout << "Failed to create a window!\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwSetWindowSizeLimits(window, 200, 150, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window, cursor_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to init GLAD!\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // set initial viewport
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0,0, fbWidth, fbHeight);

    //render loop
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        update(window);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// ------------------------------------------
// keyboard callback
// ------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action != GLFW_PRESS){
        return;
    }

    // ESC - close window
    if(key == GLFW_KEY_ESCAPE){
        glfwSetWindowShouldClose(window, true);
    }

    // R - red
    if(key == GLFW_KEY_R){
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    }

    // G - green
    if(key == GLFW_KEY_G){
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    }

    // B - blue
    if(key == GLFW_KEY_B){
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    }

    // CTRL + UP - decrease window height
    if(key == GLFW_KEY_UP && (mods & GLFW_MOD_CONTROL)){
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        if(height - 25 < 150)
            height = 150;
        else
            height -= 25;
        glfwSetWindowSize(window, width, height);
    }

    // CTRL + DOWN - increase window height
    if(key == GLFW_KEY_DOWN && (mods & GLFW_MOD_CONTROL)){
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetWindowSize(window, width, height + 25);
    }

    // CTRL + LEFT - decrease window width
    if(key == GLFW_KEY_LEFT && (mods & GLFW_MOD_CONTROL)){
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        if(width - 25 < 200)
            width = 200;
        else
            width -= 25;
        glfwSetWindowSize(window, width, height);
    }

    // CTRL + RIGHT - increase window width
    if(key == GLFW_KEY_RIGHT && (mods & GLFW_MOD_CONTROL)){
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetWindowSize(window, width + 25, height);
    }
}

// ------------------------------------------
// mouse callback
// ------------------------------------------
void mouse_callback(GLFWwindow* window, int button, int action, int mods){
    // MOUSE LEFT - random window color
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        float r = static_cast<float>(std::rand()) / RAND_MAX;
        float g = static_cast<float>(std::rand()) / RAND_MAX;
        float b = static_cast<float>(std::rand()) / RAND_MAX;

        glClearColor(r, g, b, 1.0f);
    }
}

// ------------------------------------------
// cursor callback
// ------------------------------------------
void cursor_callback(GLFWwindow* window, double xpos, double ypos){
    bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
    if(!shiftPressed)
        return;
    
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    if(width <= 0 || height <= 0)
        return;
    
    double x = xpos / width;
    double y = 1.0 - ypos / height;

    glClearColor(static_cast<float>(x), static_cast<float>(y), 0.0f, 1.0f);
}
// ------------------------------------------
// framebuffer callback
// ------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width, height);
}

// ------------------------------------------
// update
// ------------------------------------------
void update(GLFWwindow* window){
    glClear(GL_COLOR_BUFFER_BIT);
}