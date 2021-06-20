//---------------------------------------------------------------------------------------------------------------------------------------------
// Title: OpenGL 3D Scene
// Author: Sarah Spence & Copyright: Learn OpenGL (https://learnopengl.com/Introduction)
// Date: 2021-06-13
// Description: This project renders a 3D scene of my countertop with my laptop, a book, and a piece of paper sitting on it. 
//----------------------------------------------------------------------------------------------------------------------------------------------
#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h" // Camera class
#include "shader.h" // Shader class

using namespace std; // Standard namespace

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "SSpence: 3D Scene"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbo;         // Handle for the vertex buffer object
        GLuint nVertices;    // Number of indices of the mesh
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;

    // Initialize mesh objects
    GLMesh counterTopMesh;
    GLMesh laptopScreenMesh;
    GLMesh laptopBaseMesh;
    GLMesh bookMesh;
    GLMesh paperMesh;

    // Initialize textures
    GLuint textureIdGranite;
    GLuint textureIdLaptopScreen;
    GLuint textureIdLaptopKeyboard;
    GLuint textureIdBookCover;
    GLuint textureIdBookSide;
    GLuint textureIdBookPages;
    GLuint textureIdPaper;

    glm::vec2 gUVScale(5.0f, 5.0f);
    GLint gTexWrapMode = GL_REPEAT;

    // Shader programs
    GLuint programIdTexture;
    GLuint programIdLighting;
    GLuint programIdLamp;

    // camera
    Camera gCamera(glm::vec3(0.0f, 0.0f, 5.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;
    bool ortho = false;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;
}

// User-defined Functions
 //------------------------
bool Initialize(int, char* [], GLFWwindow** window);
void ResizeWindow(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);
void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CreateCountertop(GLMesh& mesh);
void CreateLaptopScreen(GLMesh& mesh);
void CreateLaptopBase(GLMesh& mesh);
void CreateBook(GLMesh& mesh);
void CreatePaper(GLMesh& mesh);
void DestroyMesh(GLMesh& mesh);
bool CreateTexture(const char* filename, GLuint& textureId);
void DestroyTexture(GLuint textureId);
void RenderCountertop();
void RenderLaptopScreen();
void RenderLaptopBase();
void RenderPaper();
void RenderBook();
void RenderScene();
void DestroyShaderProgram(GLuint programId);

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up. A function to flip
//-------------------------------------------------------------------------------------------
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    // Initialize window
    //-------------------
    if (!Initialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Initialize buffer data
    //-----------------------
    CreateCountertop(counterTopMesh);
    CreateLaptopScreen(laptopScreenMesh);
    CreateLaptopBase(laptopBaseMesh);
    CreateBook(bookMesh);
    CreatePaper(paperMesh);

    // Build and compile the shader programs
    // -------------------------------------
    Shader textureShader("shaderFiles/texture_shader.vs", "shaderfiles/texture_shader.fs");
    Shader lightingShader("shaderFiles/lighting_shader.vs", "shaderfiles/lighting_shader.fs");
    Shader lampShader("shaderFiles/lamp_shader.vs", "shaderfiles/lamp_shader.fs");

    programIdTexture = textureShader.ID;
    programIdLighting = lightingShader.ID;
    programIdLamp = lampShader.ID;

    // Load granite texture
    //----------------------
    const char* texFileName = "Textures/granite.jpg";
    if (!CreateTexture(texFileName, textureIdGranite))
    {
        cout << "Failed to load texture " << texFileName << endl;
        return EXIT_FAILURE;
    }

    // Load laptop screen texture
    //---------------------------
    texFileName = "Textures/laptop_screen.jpg";
    if (!CreateTexture(texFileName, textureIdLaptopScreen))
    {
        cout << "Failed to load texture " << texFileName << endl;
        return EXIT_FAILURE;
    }

    // Load laptop keyboard
    //----------------------
    texFileName = "Textures/laptop_keyboard.jpg";
    if (!CreateTexture(texFileName, textureIdLaptopKeyboard))
    {
        cout << "Failed to load texture " << texFileName << endl;
        return EXIT_FAILURE;
    }

    // Load book cover
    //----------------
    texFileName = "Textures/book_cover.jpg";
    if (!CreateTexture(texFileName, textureIdBookCover))
    {
        cout << "Failed to load texture " << texFileName << endl;
        return EXIT_FAILURE;
    }

    // Load book pages
    //----------------
    texFileName = "Textures/book_pages.jpg";
    if (!CreateTexture(texFileName, textureIdBookPages))
    {
        cout << "Failed to load texture " << texFileName << endl;
        return EXIT_FAILURE;
    }

    // Load book side
    //----------------
    texFileName = "Textures/book_side.jpg";
    if (!CreateTexture(texFileName, textureIdBookSide))
    {
        cout << "Failed to load texture " << texFileName << endl;
        return EXIT_FAILURE;
    }
    // Load book side
    //---------------
    texFileName = "Textures/paper.jpg";
    if (!CreateTexture(texFileName, textureIdPaper))
    {
        cout << "Failed to load texture " << texFileName << endl;
        return EXIT_FAILURE;
    }

    // Set textures for lighting shader
    //---------------------------------
    glUseProgram(programIdLighting);
    // Set the first texure as granite
    glUniform1i(glGetUniformLocation(programIdLighting, "textureGranite"), 0);
    // Set the second texture as paper
    glUniform1i(glGetUniformLocation(programIdLighting, "texturePaper"), 1);

    // Set textures for texture shader
    //--------------------------------
    glUseProgram(programIdTexture);
    // Set the first texture as screen
    glUniform1i(glGetUniformLocation(programIdTexture, "textureLaptopScreen"), 0);
    // Set the second texture as keyboard
    glUniform1i(glGetUniformLocation(programIdTexture, "textureLaptopKeyboard"), 1);
    // Set the third texture to book cover
    glUniform1i(glGetUniformLocation(programIdTexture, "textureBookCover"), 2);
    // Set the fourth texture to book pages
    glUniform1i(glGetUniformLocation(programIdTexture, "textureBookPages"), 3);
    // Set the fifth texture to book side
    glUniform1i(glGetUniformLocation(programIdTexture, "textureBookSide"), 4);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        ProcessInput(gWindow);

        // Render this frame
        //-------------------
        RenderScene();

        // Poll events
        //------------
        glfwPollEvents();
    }

    // Release mesh data
    //------------------
    DestroyMesh(counterTopMesh);
    DestroyMesh(laptopScreenMesh);
    DestroyMesh(laptopBaseMesh);
    DestroyMesh(bookMesh);

    // Release texture data
    //----------------------
    DestroyTexture(textureIdGranite);
    DestroyTexture(textureIdLaptopScreen);
    DestroyTexture(textureIdLaptopKeyboard);
    DestroyTexture(textureIdBookCover);
    DestroyTexture(textureIdBookPages);
    DestroyTexture(textureIdBookSide);
    DestroyTexture(textureIdPaper);

    // Release shader programs
    //-------------------------
    DestroyShaderProgram(programIdTexture);
    DestroyShaderProgram(programIdLighting);
    DestroyShaderProgram(programIdLamp);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
//--------------------------------------------
bool Initialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, ResizeWindow);
    glfwSetCursorPosCallback(*window, MousePositionCallback);
    glfwSetScrollCallback(*window, MouseScrollCallback);
    glfwSetMouseButtonCallback(*window, MouseButtonCallback);

    // tell GLFW to capture our mouse
    //--------------------------------
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Displays GPU OpenGL version
    //------------------------------
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//----------------------------------------------------------------------------------------------------------
void ProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        ortho = !ortho;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ----------------------------------------------------------------------------------------------
void ResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void MousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}

// Create the countertop
//-----------------------
void CreateCountertop(GLMesh& mesh)
{
    GLfloat verts[] = {
        // Vertex Positions // Normals  // Texture
        0.5f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

        0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    // Generate vao
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    // Create normal pointers
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
    // Create texture pointers
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Render countertop
//------------------
void RenderCountertop()
{
    // 1. Scales the object 
    glm::mat4 scale = glm::scale(glm::vec3(9.0f, 0.2f, 10.0f));
    // 2. Rotates shape
    glm::mat4 rotation = glm::rotate(45.0f, glm::vec3(0.0f, -1.0f, 0.0f));
    // 3. Position object
    glm::mat4 translation = glm::translate(glm::vec3(-4.0f, -0.5f, 0.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection or orthographic projection based on input given
    glm::mat4 projection;
    if (ortho) {
        float ortho_scale = 100;
        projection = glm::ortho(-((float)WINDOW_WIDTH / ortho_scale), ((float)WINDOW_WIDTH / ortho_scale), -((float)WINDOW_HEIGHT / ortho_scale), ((float)WINDOW_HEIGHT / ortho_scale), 4.5f, 6.5f);
    }
    else {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Set the shader to be used
    glUseProgram(programIdLighting);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(programIdLighting, "model");
    GLint viewLoc = glGetUniformLocation(programIdLighting, "view");
    GLint projLoc = glGetUniformLocation(programIdLighting, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Shader program for the cube color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(programIdLighting, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(programIdLighting, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(programIdLighting, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(programIdLighting, "viewPosition");

    // Pass color, light, and camera data to the Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightPositionLoc, 0.0f, 0.0f, 0.0f);
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(programIdLighting, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(counterTopMesh.vao);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIdGranite);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, counterTopMesh.nVertices);

    // Draw lamps
    glUseProgram(programIdLamp);

    // Position light 1
    // -----------------
    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(glm::vec3(-2.0f, 2.5f, -2.0f)) * glm::scale(glm::vec3(0.3f, 0.3f, 0.3f));

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(programIdLamp, "model");
    viewLoc = glGetUniformLocation(programIdLamp, "view");
    projLoc = glGetUniformLocation(programIdLamp, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, counterTopMesh.nVertices);

    //position light 2
    //------------------
    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(glm::vec3(2.0f, 2.5f, -2.0f)) * glm::scale(glm::vec3(0.3f, 0.3f, 0.3f));

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(programIdLamp, "model");
    viewLoc = glGetUniformLocation(programIdLamp, "view");
    projLoc = glGetUniformLocation(programIdLamp, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, counterTopMesh.nVertices);

    // Position light 3
    // -----------------
    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(glm::vec3(0.0f, 2.5f, -2.0f)) * glm::scale(glm::vec3(0.3f, 0.3f, 0.3f));

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(programIdLamp, "model");
    viewLoc = glGetUniformLocation(programIdLamp, "view");
    projLoc = glGetUniformLocation(programIdLamp, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, counterTopMesh.nVertices);
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

// Create the laptop screen
//--------------------------
void CreateLaptopScreen(GLMesh& mesh)
{
    GLfloat verts[] = {
        // Vertex Positions    // texture
        0.5f, 0.5f, 0.0f,  0.2f, 0.2f,
        0.5f, -0.5f, 0.0f,  0.2f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.2f,
        0.5f, -0.5f, 0.0f,  0.2f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.2f,

        0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -1.0f,  0.0f, 0.0f,
        0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, 0.5f, -1.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  0.0f, 0.0f,
        0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  0.0f, 0.0f,

        0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -1.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,

        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, 0.0f
    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerUV));

    // Create VAO
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Create VBO
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    // Create texture pointers
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(2);
}

// Render Laptop Screen
//----------------------
void RenderLaptopScreen()
{
    // 1. Scales the object
    glm::mat4 scale = glm::scale(glm::vec3(2.0f, 1.5f, 0.05f));
    // 2. Rotates shape by 120 degrees in the y axis
    glm::mat4 rotation = glm::rotate(120.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    // 3. Position the object
    glm::mat4 translation = glm::translate(glm::vec3(-2.1f, 0.4f, -2.6f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection or orthographic projection based on input given
    glm::mat4 projection;
    if (ortho) {
        float ortho_scale = 100;
        projection = glm::ortho(-((float)WINDOW_WIDTH / ortho_scale), ((float)WINDOW_WIDTH / ortho_scale), -((float)WINDOW_HEIGHT / ortho_scale), ((float)WINDOW_HEIGHT / ortho_scale), 4.5f, 6.5f);
    }
    else {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Set the shader to be used
    glUseProgram(programIdTexture);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(programIdTexture, "model");
    GLint viewLoc = glGetUniformLocation(programIdTexture, "view");
    GLint projLoc = glGetUniformLocation(programIdTexture, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint UVScaleLoc = glGetUniformLocation(programIdTexture, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(laptopScreenMesh.vao);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIdLaptopScreen);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, laptopScreenMesh.nVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

// Create laptop keyboard
//------------------------
void CreateLaptopBase(GLMesh& mesh) {
    GLfloat verts[] = {
        // Vertex Positions    // texture
        0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f,

        0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -1.0f,  0.0f, 0.0f,
        0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.0f,  0.2f, 0.0f,
        0.5f, 0.5f, -1.0f,  0.2f, 0.2f,
        -0.5f, 0.5f, -1.0f,  0.0f, 0.2f,
        0.5f, 0.5f, 0.0f,  0.2f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  0.0f, 0.2f,

        0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -1.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,

        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, 0.0f
    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerUV));

    // Create VAO
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Create VBO
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    // Create texture pointers
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(2);
}

// Render laptop keyboard
//------------------------
void RenderLaptopBase() {

    // 1. Scales the object
    glm::mat4 scale = glm::scale(glm::vec3(2.0f, 0.05f, 1.8f));
    // 2. Rotates shape by 120 degrees in the y axis
    glm::mat4 rotation = glm::rotate(120.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    // 3. Position the object
    glm::mat4 translation = glm::translate(glm::vec3(-1.1f, -0.4f, -1.2f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection or orthographic projection based on input given
    glm::mat4 projection;
    if (ortho) {
        float ortho_scale = 100;
        projection = glm::ortho(-((float)WINDOW_WIDTH / ortho_scale), ((float)WINDOW_WIDTH / ortho_scale), -((float)WINDOW_HEIGHT / ortho_scale), ((float)WINDOW_HEIGHT / ortho_scale), 4.5f, 6.5f);
    }
    else {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Set the shader to be used
    glUseProgram(programIdTexture);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(programIdTexture, "model");
    GLint viewLoc = glGetUniformLocation(programIdTexture, "view");
    GLint projLoc = glGetUniformLocation(programIdTexture, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint UVScaleLoc = glGetUniformLocation(programIdTexture, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(laptopBaseMesh.vao);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIdLaptopKeyboard);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, laptopBaseMesh.nVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

// Create the book
//-----------------
void CreateBook(GLMesh& mesh)
{
    GLfloat verts[] = {
        // Vertex Positions    // texture
        0.5f, 0.5f, 0.0f,  0.5f, 0.5f,   // Pages front
        0.5f, -0.5f, 0.0f,  0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.5f,
        0.5f, -0.5f, 0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.5f,

        0.5f, 0.5f, 0.0f,  0.0f, 0.5f,  // Pages right
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -1.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,  0.0f, 0.5f,
        0.5f, -0.5f, -1.0f,  0.5f, 0.0f,
        0.5f, 0.5f, -1.0f, 0.5f, 0.5f,

        -0.5f, -0.5f, 0.0f,  0.0f, 0.5f,  // Pages back
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.5f,
        -0.5f, 0.5f, -1.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.5f, 0.5f,

        0.5f, 0.5f, 0.0f,  0.2f, 0.0f,  // Cover
        0.5f, 0.5f, -1.0f,  0.2f, 0.2f,
        -0.5f, 0.5f, -1.0f,  0.0f, 0.2f,
        0.5f, 0.5f, 0.0f,  0.2f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  0.0f, 0.2f,

        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // Bottom
        0.5f, -0.5f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,


        0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  // Book Side Binding
        0.5f, 0.5f, -1.0f,  0.0f, 0.2f,
        -0.5f, 0.5f, -1.0f, 0.2f, 0.2f,
        0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f, 0.2f, 0.2f,
        -0.5f, -0.5f, -1.0f,  0.2f, 0.0f,
    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerUV));

    // Create VAO
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Create VBO
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    // Create texture pointers
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(2);
}

// Render Book
//-------------
void RenderBook() {

    // 1. Scales the object
    glm::mat4 scale = glm::scale(glm::vec3(2.0f, 0.5f, 1.0f));
    // 2. Rotates shape by 180 degrees about the y-axis
    glm::mat4 rotation = glm::rotate(180.0f, glm::vec3(0.0f, -1.5f, 0.0f));
    // 3. Position the object
    glm::mat4 translation = glm::translate(glm::vec3(2.0f, -0.2f, -3.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection or orthographic projection based on input given
    glm::mat4 projection;
    if (ortho) {
        float ortho_scale = 100;
        projection = glm::ortho(-((float)WINDOW_WIDTH / ortho_scale), ((float)WINDOW_WIDTH / ortho_scale), -((float)WINDOW_HEIGHT / ortho_scale), ((float)WINDOW_HEIGHT / ortho_scale), 4.5f, 6.5f);
    }
    else {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Set the shader to be used
    glUseProgram(programIdTexture);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(programIdTexture, "model");
    GLint viewLoc = glGetUniformLocation(programIdTexture, "view");
    GLint projLoc = glGetUniformLocation(programIdTexture, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint UVScaleLoc = glGetUniformLocation(programIdTexture, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(bookMesh.vao);

    // Bind texture for book pages
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIdBookPages);
    // Draws the book pages
    glDrawArrays(GL_TRIANGLES, 0, 18);

    // bind texture for book side
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIdBookSide);
    glDrawArrays(GL_TRIANGLES, 30, 36);

    // Bind texture for book cover
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIdBookCover);
    glDrawArrays(GL_TRIANGLES, 18, 30);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

}
// Create paper mesh
//--------------------
void CreatePaper(GLMesh& mesh) {
    GLfloat verts[] = {
        // Vertex Positions // Normals  // Texture
        0.5f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.2f, 0.0f,
        0.5f, 0.5f, -1.0f,  0.0f, 1.0f, 0.0f, 0.2f, 0.2f,
        -0.5f, 0.5f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.2f,
        0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.2f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.2f,

        0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    // Generate vao
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    // Create normal pointers
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
    // Create texture pointers
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Render and position the paper. Also add a yellow light to change the color of the paper.
//-----------------------------------------------------------------------------------------
void RenderPaper() {

    // 1. Scales the object 
    glm::mat4 scale = glm::scale(glm::vec3(1.0f, 0.0f, 1.5f));
    // 2. Rotates shape
    glm::mat4 rotation = glm::rotate(120.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    // 3. Position object
    glm::mat4 translation = glm::translate(glm::vec3(0.5f, -0.35f, 0.5f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;
    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection or orthographic projection based on input given
    glm::mat4 projection;
    if (ortho) {
        float ortho_scale = 100;
        projection = glm::ortho(-((float)WINDOW_WIDTH / ortho_scale), ((float)WINDOW_WIDTH / ortho_scale), -((float)WINDOW_HEIGHT / ortho_scale), ((float)WINDOW_HEIGHT / ortho_scale), 4.5f, 6.5f);
    }
    else {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Set the shader to be used
    glUseProgram(programIdLighting);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(programIdLighting, "model");
    GLint viewLoc = glGetUniformLocation(programIdLighting, "view");
    GLint projLoc = glGetUniformLocation(programIdLighting, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Shader program for the cube color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(programIdLighting, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(programIdLighting, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(programIdLighting, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(programIdLighting, "viewPosition");

    // Pass color, light, and camera data to the Shader program's corresponding uniforms
    // Add a yellow tint to this light to meet project requirements
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 0.0f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 0.6f);
    glUniform3f(lightPositionLoc, 0.0f, 0.0f, 0.0f);
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(programIdLighting, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(paperMesh.vao);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIdPaper);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, paperMesh.nVertices);

    // Draw lamps
    glUseProgram(programIdLamp);

    // Position light 1
    // ------------------
    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(glm::vec3(-4.0f, 2.5f, -2.0f)) * glm::scale(glm::vec3(0.3f, 0.3f, 0.3f));

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(programIdLamp, "model");
    viewLoc = glGetUniformLocation(programIdLamp, "view");
    projLoc = glGetUniformLocation(programIdLamp, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, paperMesh.nVertices);

    // Position light 2
    // ----------------------
    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(glm::vec3(4.0f, 2.5f, -2.0f)) * glm::scale(glm::vec3(0.3f, 0.3f, 0.3f));

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(programIdLamp, "model");
    viewLoc = glGetUniformLocation(programIdLamp, "view");
    projLoc = glGetUniformLocation(programIdLamp, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, paperMesh.nVertices);
    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, paperMesh.nVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}
// Render the scene
//------------------
void RenderScene() {

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Set the background color and Clear the frame and z buffers
    glClearColor(0.01f, 0.18f, 0.31f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Call functions to render objects
    RenderCountertop();
    RenderLaptopScreen();
    RenderLaptopBase();
    RenderBook();
    RenderPaper();

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// Destoy mesh data
//------------------
void DestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.vbo);
}


// Generate and load the texture
//-------------------------------
bool CreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}

// Destroy texture data
//----------------------
void DestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}

// Destroy shader program
//------------------------
void DestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}