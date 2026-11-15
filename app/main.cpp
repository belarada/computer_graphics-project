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

// shader
const char *vertexShaderSource = R"S(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main(){
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)S";

// fragment shader
const char *fragmentShaderSource = R"S(
    #version 330 core
    out vec4 FragColor;
    void main(){
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
)S";

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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "computer graphics", nullptr, nullptr);
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

    // build and compile shader program
    // --------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data and cofigure vertex attributes
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,

        -0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f

    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // set initial viewport
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0,0, fbWidth, fbHeight);

    //render loop
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        update(window);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
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