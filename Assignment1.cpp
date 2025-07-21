//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//

#include <iostream>
#include <list>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>


using namespace glm;
using namespace std;

class Projectile
{
public:
    Projectile(vec3 position, vec3 velocity, int shaderProgram) : mPosition(position), mVelocity(velocity)
    {
        mWorldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

    }
    
    void Update(float dt)
    {
        mPosition += mVelocity * dt;
    }
    
    void Draw() {
        // this is a bit of a shortcut, since we have a single vbo, it is already bound
        // let's just set the world matrix in the vertex shader
        
        mat4 worldMatrix = translate(mat4(1.0f), mPosition) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(mWorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
private:
    GLuint mWorldMatrixLocation;
    vec3 mPosition;
    vec3 mVelocity;
};


const char* getVertexShaderSource()
{
    // For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return
                "#version 330 core\n"
                "layout (location = 0) in vec3 aPos;"
                "layout (location = 1) in vec3 aColor;"
                ""
                "uniform mat4 worldMatrix;"
                "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
                "uniform mat4 projectionMatrix = mat4(1.0);"
                ""
                "out vec3 vertexColor;"
                "void main()"
                "{"
                "   vertexColor = aColor;"
                "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
                "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
                "}";
}


const char* getFragmentShaderSource()
{
    return
                "#version 330 core\n"
                "in vec3 vertexColor;"
                "out vec4 FragColor;"
                "void main()"
                "{"
                "   FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
                "}";
}


int compileAndLinkShaders()
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = getVertexShaderSource();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = getFragmentShaderSource();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

vec3 prism1VertexArray[] =  {  // position,                            color
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), //left - red
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
        
        vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), // far - blue
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        
        vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), // bottom - turquoise
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), // near - green
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), // right - purple
        vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        
        vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f), // top - gray
        vec3( 0.5f, 0.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f)
    };
vec3 prism2VertexArray[] =  {  // position,                            color
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), //left - red
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 1.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 1.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 1.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
        
        vec3( 0.5f, 1.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), // far - blue
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f, 1.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        
        vec3( 0.5f, 1.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), // bottom - turquoise
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        
        vec3(-0.5f, 1.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), // near - green
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        
        vec3( 0.5f, 1.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3(-0.5f, 1.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        
        vec3( 0.5f, 1.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), // right - purple
        vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f, 1.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        
        vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f, 1.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
        
        vec3( 0.5f, 1.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f), // top - gray
        vec3( 0.5f, 1.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f),
        vec3(-0.5f, 1.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f),
        
        vec3( 0.5f, 1.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f),
        vec3(-0.5f, 1.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f),
        vec3(-0.5f, 1.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f)
    };

    glm::vec3 pyramidVertexArray[] = {
    // left face
    { -0.5f,-0.5f,-0.5f }, {1,0,0},
    { -0.5f,-0.5f, 0.5f }, {1,0,0},
    {  0.0f, 1.5f, 0.0f }, {1,0,0},

    // back face
    {  0.5f,-0.5f,-0.5f }, {0,0,1},
    { -0.5f,-0.5f,-0.5f }, {0,0,1},
    {  0.0f, 1.5f, 0.0f }, {0,0,1},

    // right face
    {  0.5f,-0.5f, 0.5f }, {1,0,1},
    {  0.5f,-0.5f,-0.5f }, {1,0,1},
    {  0.0f, 1.5f, 0.0f }, {1,0,1},

    // front face
    { -0.5f,-0.5f, 0.5f }, {0,1,0},
    {  0.5f,-0.5f, 0.5f }, {0,1,0},
    {  0.0f, 1.5f, 0.0f }, {0,1,0},

    { -0.5f,-0.5f,-0.5f }, {0,1,1},
    {  0.5f,-0.5f,-0.5f }, {0,1,1},
    {  0.5f,-0.5f, 0.5f }, {0,1,1},

    { -0.5f,-0.5f,-0.5f }, {0,1,1},
    {  0.5f,-0.5f, 0.5f }, {0,1,1},
    { -0.5f,-0.5f, 0.5f }, {0,1,1},
};

glm::vec3 tetraVertexArray[] = {
    // face red
    {  1.f,  1.f,  1.f }, {1.f, 0.f, 0.f},
    {  1.f, -1.f, -1.f }, {1.f, 0.f, 0.f},
    { -1.f,  1.f, -1.f }, {1.f, 0.f, 0.f},

    // face green
    {  1.f,  1.f,  1.f }, {0.f, 1.f, 0.f},
    { -1.f, -1.f,  1.f }, {0.f, 1.f, 0.f},
    {  1.f, -1.f, -1.f }, {0.f, 1.f, 0.f},

    // face blue
    {  1.f,  1.f,  1.f }, {0.f, 0.f, 1.f},
    { -1.f,  1.f, -1.f }, {0.f, 0.f, 1.f},
    { -1.f, -1.f,  1.f }, {0.f, 0.f, 1.f},

    // face yellow
    {  1.f, -1.f, -1.f }, {1.f, 1.f, 0.f},
    { -1.f, -1.f,  1.f }, {1.f, 1.f, 0.f},
    { -1.f,  1.f, -1.f }, {1.f, 1.f, 0.f},
};


int createVertexArrayObject(const glm::vec3* vertexArray, int arraySize)
{
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, arraySize, vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          2*sizeof(glm::vec3), // stride - each vertex contain 2 vec3 (position, color)
                          (void*)0             // array buffer offset
                          );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          2*sizeof(glm::vec3),
                          (void*)sizeof(glm::vec3)      // color is offseted a vec3 (comes after position)
                          );
    glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return vertexArrayObject;
}

int main(int argc, char *argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(800, 600, "Comp371 - Lab 03", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // @TODO 3 - Disable mouse cursor
    // ...
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkShaders();
    
    // We can set the shader once, since we have only one
    glUseProgram(shaderProgram);

    
    // Camera parameters for view transform
    vec3 cameraPosition(0.6f,1.0f,10.0f);
    vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);
    
    // Other camera parameters
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 70.0f;
    float cameraVerticalAngle = 20.0f;
    bool  cameraFirstPerson = true; // press 1 or 2 to toggle this variable
    bool cameraMouseControl = true; // press m to switch to keyboard only camera controls

    // Spinning cube at camera position
    float spinningCubeAngle = 0.0f;
    
    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(90.0f,            // field of view in degrees
                                             800.0f / 600.0f,  // aspect ratio
                                             0.01f, 100.0f);   // near and far (near > 0)
    
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
                             cameraPosition + cameraLookAt,  // center
                             cameraUp ); // up
    
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    
    
    // Define and upload geometry to the GPU here ...
    int vaoCube = createVertexArrayObject(prism1VertexArray, sizeof(prism1VertexArray));
    int vaoPrism = createVertexArrayObject(prism2VertexArray, sizeof(prism2VertexArray));
    int vaoPyramid = createVertexArrayObject(pyramidVertexArray, sizeof(pyramidVertexArray));
    int vaoTetra = createVertexArrayObject(tetraVertexArray, sizeof(tetraVertexArray));

    
    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
    
    // Other OpenGL states to set once
    // Enable Backface culling
    glEnable(GL_CULL_FACE);
    
    // @TODO 1 - Enable Depth Test
    // ...
    
    
    // Container for projectiles to be implemented in tutorial
    list<Projectile> projectileList;
    
    
    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;
        glEnable(GL_DEPTH_TEST);
        // Each frame, reset color of each pixel to glClearColor

        // @TODO 1 - Clear Depth Buffer Bit as well
        // ...
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        // Draw geometry
        glBindVertexArray(vaoPrism);

        // Draw ground
        mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.01f, 0.0f)) * scale(mat4(1.0f), vec3(10.0f, 0.02f, 10.0f));
        GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        
        // Draw prism
        mat4 prismWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.5f, 0.8f)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &prismWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(vaoPyramid);
        
        // Draw ptramid
        mat4 pyramidWorldMatrix = translate(mat4(1.0f), vec3(-2.0f, 0.5f, -1.f)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pyramidWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 18);
        
        // Draw tetra

        glBindVertexArray(vaoTetra);
        mat4 tetraWorldMatrix = translate(mat4(1.0f), vec3(2.0f, 0.7f, -1.5f)) * scale(mat4(1.0f), vec3(0.7f, 0.7f, 0.7f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &tetraWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        
        // Spinning cube at camera position
        spinningCubeAngle += 180.0f * dt;
        
        glBindVertexArray(vaoCube);
        vec3 cubeOrbitCenter = vec3(0.f, 5.f, 0.f);
        mat4 spinningCubeWorldMatrix = 
                                        rotate(mat4(1.0f), radians(spinningCubeAngle), vec3(0.0f, 1.0f, 0.0f)) *
                                        translate(mat4(1.0f), cubeOrbitCenter);
                                        
        
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &spinningCubeWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        mat4 spinningCube2WorldMatrix =  spinningCubeWorldMatrix * rotate(mat4(1.0f), radians(spinningCubeAngle / 2), vec3(0.0f, 1.0f, 0.0f)) * translate(mat4(1.0f), vec3(3.f, 0.f, 0.f)) * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));

        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &spinningCube2WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        mat4 spinningCube3WorldMatrix =  spinningCube2WorldMatrix * rotate(mat4(1.0f), radians(spinningCubeAngle), vec3(0.0f, 1.0f, 0.0f)) * translate(mat4(1.0f), vec3(3.f, 0.f, 0.f)) * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
        
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &spinningCube3WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        
        
        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // move camera down
        {
            cameraFirstPerson = true;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // move camera down
        {
            cameraFirstPerson = false;
        }
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) // toggle mouse control
        {
            cameraMouseControl = false;
        }
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) // toggle mouse control
        {
            cameraMouseControl = true;
        }

        
        // This was solution for Lab02 - Moving camera exercise
        // We'll change this to be a first or third person camera
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;
        
        
        // @TODO 4 - Calculate mouse motion dx and dy
        //         - Update camera horizontal and vertical angle
        //...
        if (cameraMouseControl){
            double mousePosx, mousePosy;
            glfwGetCursorPos(window, &mousePosx, &mousePosy);
            double dx = lastMousePosX - mousePosx;
            double dy = lastMousePosY - mousePosy;
            lastMousePosX = mousePosx;
            lastMousePosY = mousePosy;
            const float cameraAngularSpeed = 50.f;
            cameraHorizontalAngle += dx * dt * cameraAngularSpeed;
            cameraVerticalAngle += dy * dt * cameraAngularSpeed * 0.75f;
            
        }
        else{
            double dx = 0.;
            double dy = 0.;
            if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
            {
                dy = 1.;
            }
            if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
            {
                dy = -1.;
            }
            if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
            {
                dx = 1.;
            }
            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            {
                dx = -1.;
            }

            
            const float cameraAngularSpeed = 50.f;
            cameraHorizontalAngle += dx * dt * cameraAngularSpeed;
            cameraVerticalAngle += dy * dt * cameraAngularSpeed * 0.75f;
        }
        
        cameraHorizontalAngle = std::fmod(cameraHorizontalAngle, 360.f);
        cameraVerticalAngle = std::max(-85.f, std::min(cameraVerticalAngle, 85.f));

        float theta = glm::radians(cameraHorizontalAngle);
        float phi = glm::radians(cameraVerticalAngle);
        cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.f, 1.f, 0.f));
        glm::normalize(cameraSideVector);

        
        // @TODO 5 = use camera lookat and side vectors to update positions with ASDW
        // adjust code below
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
        {
            cameraPosition -= currentCameraSpeed * dt * cameraSideVector;
        }
        
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
        {
            cameraPosition += currentCameraSpeed * dt * cameraSideVector;
        }
        
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera up
        {
            cameraPosition -= cameraLookAt * currentCameraSpeed * dt;
        }
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera down
        {
            cameraPosition += cameraLookAt * currentCameraSpeed * dt;
        }
      

        
        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp );

        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        


    }

    
    // Shutdown GLFW
    glfwTerminate();
    
	return 0;
}
