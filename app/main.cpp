// @formatter:off
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <vector>
#include <cmath>
// @formatter:on

// constants
constexpr float PI = 3.14159265358979323846f;

// polygon struct
struct PolygonState{
    int n = 5;
    float radius = 0.5f;

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    GLsizei indexCount = 0;
};

// function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void update(GLFWwindow* window);
void createPolygon(int n, float radius, std::vector<float>& vertices, std::vector<unsigned int>& indices);
void updatePolygon(PolygonState& polygon);

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
    uniform vec4 ourColor;

    void main(){
        FragColor = ourColor;
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

    int colorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    if(colorLocation == -1){
        std::cout << "Failed to find uniform 'ourColor'!\n";
        return EXIT_FAILURE;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data and cofigure vertex attributes
    float vertices[] = {
        // bottom row
        -1.0f, -1.0f, 0.0f, // 0
        0.0f, -1.0f, 0.0f,  // 1
        1.0f, -1.0f, 0.0f,  // 2

        // middle row
        -1.0f, 0.0f, 0.0f,  // 3
        0.0f, 0.0f, 0.0f,   // 4
        1.0f, 0.0f, 0.0f,   // 5

        // top row
        -1.0f, 1.0f, 0.0f,  // 6
        0.0f, 1.0f, 0.0f,   // 7
        1.0f, 1.0f, 0.0f    // 8

    };

    unsigned int indices[] = {
        // I quadrant
        4, 5, 8,
        4, 8, 7, 

        // II quadrant
        3, 4, 7,
        3, 7, 6,

        // III quadrant
        0, 1, 4,
        0, 4, 3,

        // IV quadrant
        1, 2, 5,
        1, 5, 4

    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // polygon geometry
    int n = 5;
    float radius = 0.5f;

    std::vector<float> polygonVertices;
    std::vector<unsigned int> polygonIndices;

    createPolygon(n, radius, polygonVertices, polygonIndices);

    unsigned int polygonVAO, polygonVBO, polygonEBO;
    
    glGenVertexArrays(1, &polygonVAO);
    glGenBuffers(1, &polygonVBO);
    glGenBuffers(1, &polygonEBO);

    glBindVertexArray(polygonVAO);
    // vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, polygonVBO);
    glBufferData(GL_ARRAY_BUFFER, polygonVertices.size() * sizeof(float), polygonVertices.data(), GL_STATIC_DRAW);

    // element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygonEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, polygonIndices.size() * sizeof(unsigned int), polygonIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0);

    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    PolygonState polygon;
    polygon.n = n;
    polygon.radius = radius;

    polygon.VAO = polygonVAO;
    polygon.VBO = polygonVBO;
    polygon.EBO = polygonEBO;

    polygon.indexCount = static_cast<GLsizei>(polygonIndices.size());

    glfwSetWindowUserPointer(window, &polygon);

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
        // I quadrant
        glUniform4f(colorLocation, 0.99f, 0.6f, 0.71f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        // II quadrant
        glUniform4f(colorLocation, 0.75f, 0.76f, 0.89f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6*sizeof(unsigned int)));
        // III quadrant
        glUniform4f(colorLocation, 0.99f, 0.96f, 0.71f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12*sizeof(unsigned int)));
        // IV quadrant
        glUniform4f(colorLocation, 0.97f, 0.96f, 0.92f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18*sizeof(unsigned int)));
        // regular polygon
        glBindVertexArray(polygonVAO);
        glUniform4f(colorLocation, 0.65f, 0.58f, 0.85f, 1.0f);
        glDrawElements(GL_TRIANGLES, polygon.indexCount, GL_UNSIGNED_INT, (void*) 0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &polygonVAO);
    glDeleteBuffers(1, &polygonVBO);
    glDeleteBuffers(1, &polygonEBO);
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

    // CTRL _ digit - change polygon
    if(key >= GLFW_KEY_3 && key <= GLFW_KEY_9 && (mods & GLFW_MOD_CONTROL)){
        int newN = key - GLFW_KEY_0;

        PolygonState* polygon = static_cast<PolygonState*>(glfwGetWindowUserPointer(window));

        if(polygon != nullptr){
            polygon->n = newN;
            updatePolygon(*polygon);
        }
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

// ------------------------------------------
// create polygon 
// ------------------------------------------
void createPolygon(int n, float radius, std::vector<float>& vertices, std::vector<unsigned int>& indices){
    vertices.clear();
    indices.clear();

    if(n < 3)
        return;
    
    // center
    vertices.insert(vertices.end(), {0.0f, 0.0f, 0.0f});

    // polygon vertices
    for(int i = 0; i<n; i++){
        float angle = PI / 2.0f + 2.0f * PI * i / n;

        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }
    // triangle indices
    for(int i = 0; i<n; i++){
        indices.push_back(0);
        indices.push_back(i+1);
        indices.push_back((i+1)%n + 1);
    }
}

// ------------------------------------------
// update polygon 
// ------------------------------------------
void updatePolygon(PolygonState& polygon){
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    createPolygon(polygon.n, polygon.radius, vertices, indices);

    // update vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, polygon.VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    // update element buffer
    glBindVertexArray(polygon.VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygon.EBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

    polygon.indexCount = static_cast<GLsizei>(indices.size());

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}