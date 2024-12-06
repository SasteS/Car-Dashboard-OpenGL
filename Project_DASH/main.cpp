#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  // For glm::ortho
//#include <glm/gtc/type_ptr.hpp>
//#include <vector>
#include <chrono>
#include <thread>
#include <map>

#include "speedometer.h"
#include "tachometer.h"
#include "GasPaddle.h"
#include "FuelTank.h"
#include "AC.h"
#include "Blinkers.h"
#include "Radio.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// text rendering libraries
#include <ft2build.h>
#include FT_FREETYPE_H

// car libraries
#include "TestBed.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);

int main(void)
{
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ INICIJALIZACIJA ++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 1920;
    unsigned int wHeight = 1080;
    const char wTitle[] = "[Car Dashboard]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    unsigned int circleShader = createShader("speedometer.vert", "speedometer.frag");
    unsigned int ticksShader = createShader("ticks.vert", "ticks.frag");
    unsigned int needleShader = createShader("needle.vert", "needle.frag");
    unsigned int textShader = createShader("text.vert", "text.frag");

    unsigned int progressBarShader = createShader("progressBar.vert", "progressBar.frag");
    
    unsigned int acShader = createShader("acShader.vert", "acShader.frag");
    
    unsigned int blinkersShader = createShader("blinker.vert", "blinker.frag");

    unsigned int indexShader = createShader("index.vert", "index.frag");

    unsigned int radioShader = createShader("radio.vert", "radio.frag");

    unsigned int panelShader = createShader("panel.vert", "panel.frag");

    unsigned int textureShader = createShader("texture.vert", "texture.frag");

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ CLASSES INITIALIZATION +++++++++++++++++++++++++++++++++++++++++++++++   
    Car car = Car();
    startSimulation(&car);
    car.setSpeedometer(30); // Crashes if it's < 30

    Speedometer speedometer = Speedometer(circleShader, ticksShader, needleShader, textShader);
    Tachometer tachometer = Tachometer(circleShader, ticksShader, needleShader, textShader);
    GasPaddle gasPaddle = GasPaddle(progressBarShader, 0.01f, 0.05f);
    FuelTank fuelTank = FuelTank(progressBarShader, 50.0f);
    AC ac = AC(acShader, textShader);
    Blinkers blinkers = Blinkers(blinkersShader);
    Radio radio = Radio(radioShader);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ CHARACTER TEXTURE GENERATION ++++++++++++++++++++++++++++++++++++++++++++++++

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "Fonts/BAUHS93.TTF", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);


    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        return -1;
    }

    struct Character {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    std::map<char, Character> Characters;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int textVAO, textVBO;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ CIRCLES +++++++++++++++++++++++++++++++++++++++++++++++++
    // CREATING SPEEDOMETER CIRCLES
    speedometer.createCircles();

    unsigned int stride = (2) * sizeof(float); // Only position (x, y)

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // Create and bind the buffer for the vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, speedometer.getCircleVertices().size() /*vertices.size()*/ * sizeof(glm::vec2), &speedometer.getCircleVertices()[0] /*&vertices[0]*/, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // CREATING TACHOMETER CIRCLES
    tachometer.createCircles();
    
    GLuint tachometerVBO, tachometerVAO;
    glGenVertexArrays(1, &tachometerVAO);
    glGenBuffers(1, &tachometerVBO);

    glBindVertexArray(tachometerVAO);

    // Create and bind the buffer for the tachometer vertices
    glBindBuffer(GL_ARRAY_BUFFER, tachometerVBO);
    glBufferData(GL_ARRAY_BUFFER, tachometer.getCircleVertices().size() * sizeof(glm::vec2), &tachometer.getCircleVertices()[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // CREATING AC VERTICES
    ac.createCircleVertices();

    GLuint acVBO, acVAO;
    glGenVertexArrays(1, &acVAO);
    glGenBuffers(1, &acVBO);

    glBindVertexArray(acVAO);

    // Create and bind the buffer for the ac vertices
    glBindBuffer(GL_ARRAY_BUFFER, acVBO);
    glBufferData(GL_ARRAY_BUFFER, ac.getCircleVertices().size() * sizeof(glm::vec2), &ac.getCircleVertices()[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ TICK MARKS +++++++++++++++++++++++++++++++++++++++++++++++++
    // CREATING SPEEDOMETER TICKS
    speedometer.createTicks();

    // Create VBO and VAO for tick marks
    GLuint tickVBO, tickVAO;
    glGenVertexArrays(1, &tickVAO);
    glGenBuffers(1, &tickVBO);

    glBindVertexArray(tickVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tickVBO);
    glBufferData(GL_ARRAY_BUFFER, speedometer.getTickVertices().size() * sizeof(glm::vec2), &speedometer.getTickVertices()[0] /*&tickVertices[0]*/, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // CREATING TACHOMETER TICKS
    tachometer.createTicks();

    GLuint tachometerTickVBO, tachometerTickVAO;
    glGenVertexArrays(1, &tachometerTickVAO);
    glGenBuffers(1, &tachometerTickVBO);

    glBindVertexArray(tachometerTickVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tachometerTickVBO);
    glBufferData(GL_ARRAY_BUFFER, tachometer.getTickVertices().size() * sizeof(glm::vec2), &tachometer.getTickVertices()[0] /*&tickVertices[0]*/, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ NEEDLE +++++++++++++++++++++++++++++++++++++++++++++++++
    // CREATING SPEEDOMETER NEEDLE
    speedometer.createNeedle();

    // Create a VAO and VBO for the needle
    GLuint needleVAO, needleVBO;
    glGenVertexArrays(1, &needleVAO);
    glGenBuffers(1, &needleVBO);

    glBindVertexArray(needleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, needleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * speedometer.getNeedleVertices().size(), speedometer.getNeedleVertices().data(), GL_STATIC_DRAW);

    // Set up the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // CREATING TACHOMETER NEEDLE
    tachometer.createNeedle();

    GLuint tachometerNeedleVAO, tachometerNeedleVBO;
    glGenVertexArrays(1, &tachometerNeedleVAO);
    glGenBuffers(1, &tachometerNeedleVBO);

    glBindVertexArray(tachometerNeedleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tachometerNeedleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* speedometer.getNeedleVertices().size(), speedometer.getNeedleVertices().data(), GL_STATIC_DRAW);

    // Set up the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROGRESS BAR ++++++++++++++++++++++++++++++++++++++++++++++++
    gasPaddle.createProgressBar();
    
    float gasLevel = gasPaddle.getGasLevel();
    float gasIncrement = gasPaddle.getGasIncrement();
    float gasDecrement = gasPaddle.getGasDecrement();

    /*std::vector<float> progressBarVertices = */gasPaddle.getProgressBarVertices();

    unsigned int progressBarVAO, progressBarVBO;
    glGenVertexArrays(1, &progressBarVAO);
    glGenBuffers(1, &progressBarVBO);

    glBindVertexArray(progressBarVAO);

    glBindBuffer(GL_ARRAY_BUFFER, progressBarVBO);
    glBufferData(GL_ARRAY_BUFFER, gasPaddle.getProgressBarVertices().size() * sizeof(float), gasPaddle.getProgressBarVertices().data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    // CREATING FUEL TANK
    fuelTank.createFuelTankVertices();

    std::vector<float> fuelTankVertices = fuelTank.getFuelTankVertices();
    float currentFuel = car.getFuelAmount();
    float maxFuel = car.getMaxFuelAmount();
    float fillRatio = currentFuel / maxFuel;

    std::vector<GLfloat> combinedVertices;
    combinedVertices.insert(combinedVertices.end(), fuelTankVertices.begin(), fuelTankVertices.end()); // the background
    combinedVertices.insert(combinedVertices.end(), fuelTankVertices.begin(), fuelTankVertices.end()); // the fuel bar


    GLuint fuelBarVAO, fuelBarVBO;
    glGenVertexArrays(1, &fuelBarVAO);
    glGenBuffers(1, &fuelBarVBO);

    // Bind and set data for the background
    glBindVertexArray(fuelBarVAO);

    glBindBuffer(GL_ARRAY_BUFFER, fuelBarVBO);
    glBufferData(GL_ARRAY_BUFFER, combinedVertices.size() * sizeof(GLfloat), combinedVertices.data(), GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ BLINKERS ++++++++++++++++++++++++++++++++++++++++++++++++
    blinkers.createBlinkersVertices();
    
    GLuint indicatorVAO, indicatorVBO;
    
    glGenVertexArrays(1, &indicatorVAO);
    glGenBuffers(1, &indicatorVBO);

    glBindVertexArray(indicatorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, indicatorVBO);
    

    std::vector<float> blinkersVertices;

    // Get the left and right blinkers vertices
    auto leftVertices = blinkers.getLeftBlinkersVertices();  // This should return a std::vector<float>
    auto rightVertices = blinkers.getRightBlinkersVertices();  // This should return a std::vector<float>

    //// Ensure both containers are of type std::vector<float> (or compatible)
    //static_assert(std::is_same<decltype(leftVertices), std::vector<float>>::value, "Left vertices must be std::vector<float>");
    //static_assert(std::is_same<decltype(rightVertices), std::vector<float>>::value, "Right vertices must be std::vector<float>");

    // Insert both sets of vertices into the blinkersVertices vector
    blinkersVertices.insert(blinkersVertices.end(), leftVertices.begin(), leftVertices.end());
    blinkersVertices.insert(blinkersVertices.end(), leftVertices.begin(), leftVertices.end());
    blinkersVertices.insert(blinkersVertices.end(), rightVertices.begin(), rightVertices.end());
    blinkersVertices.insert(blinkersVertices.end(), rightVertices.begin(), rightVertices.end());

    //std::cout << blinkersVertices.size();

    // Create buffer data
    glBufferData(GL_ARRAY_BUFFER, blinkersVertices.size() * sizeof(float), blinkersVertices.data(), GL_STATIC_DRAW);

    //std::vector<float> blinkersVertices;
    //blinkersVertices.insert(blinkersVertices.end(), blinkers.getLeftBlinkersVertices().begin(), blinkers.getLeftBlinkersVertices().end()); // the background
    //blinkersVertices.insert(blinkersVertices.end(), blinkers.getRightBlinkersVertices().begin(), blinkers.getRightBlinkersVertices().end()); // the fuel bar
    //glBufferData(GL_ARRAY_BUFFER, blinkersVertices.size() * sizeof(float), blinkersVertices.data(), GL_STATIC_DRAW);

    // Vertex position attribute
     glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++ RADIO ++++++++++++++++++++++++++++++++++++++++++++++++
    radio.createRadioVertices();

    GLuint radioVAO, radioVBO;
    // Create VAO and VBO
    glGenVertexArrays(1, &radioVAO);
    glGenBuffers(1, &radioVBO);
    glBindVertexArray(radioVAO);
    glBindBuffer(GL_ARRAY_BUFFER, radioVBO);

    std::vector<float> radioVertices;

    // Get left, right, and background rectangle vertices
    auto leftRadioVertices = radio.getRadioLeftVertices();
    auto rightRadioVertices = radio.getRadioRightVertices();
    auto backgroundRadioVertices = radio.getRadioBackgroundVertices();
    auto buttonRadioVertices = radio.getRadioButtonVertices();

    // Combine all vertices
    radioVertices.insert(radioVertices.end(), leftRadioVertices.begin(), leftRadioVertices.end());
    radioVertices.insert(radioVertices.end(), rightRadioVertices.begin(), rightRadioVertices.end());
    radioVertices.insert(radioVertices.end(), backgroundRadioVertices.begin(), backgroundRadioVertices.end());
    radioVertices.insert(radioVertices.end(), buttonRadioVertices.begin(), buttonRadioVertices.end());

    // Send vertex data to GPU
    glBufferData(GL_ARRAY_BUFFER, radioVertices.size() * sizeof(float), radioVertices.data(), GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ FRONT PANEL +++++++++++++++++++++++++++++++++++++++++++++++++
    std::vector<float> frontPanel = {
        -1.0f,  1.0f, 0.0f, // Top-left
        1.0f,  1.0f, 0.0f, // Top-right
        1.0f, 0.8f, 0.0f, // Bottom-right
        -1.0f, 0.8f, 0.0f  // Bottom-left
    };

    std::vector<float> visorVertices = {
        -1.0f, 0.5f, 0.0f,  // Top-left
         1.0f, 0.5f, 0.0f,  // Top-right
         1.0f, 0.0f, 0.0f,  // Bottom-right
        -1.0f, 0.0f, 0.0f   // Bottom-left
    };

    std::vector<float> panelVertices;

    panelVertices.insert(panelVertices.end(), frontPanel.begin(), frontPanel.end());
    panelVertices.insert(panelVertices.end(), visorVertices.begin(), visorVertices.end());

    GLuint panelVAO, panelVBO;

    glGenVertexArrays(1, &panelVAO);
    glGenBuffers(1, &panelVBO);
    glBindVertexArray(panelVAO);
    glBindBuffer(GL_ARRAY_BUFFER, panelVBO);

    // Send vertex data to GPU
    glBufferData(GL_ARRAY_BUFFER, panelVertices.size() * sizeof(float), panelVertices.data(), GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO
    glBindVertexArray(0);

    //--------------DriverId
    //unsigned imageTexture = loadImageToTexture("Images/driverId.jpg");
    unsigned imageTexture = loadImageToTexture("Images/visor2.png");
    unsigned radioButtonTexture = loadImageToTexture("Images/radio-button.png");
  
    std::cout << imageTexture << std::endl;
    std::cout << radioButtonTexture << std::endl;

    glBindTexture(GL_TEXTURE_2D, imageTexture); // Set texture
    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Horizontal repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Vertical repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Set filtering
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, radioButtonTexture); // Set texture
    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Horizontal repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Vertical repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Set filtering
    glBindTexture(GL_TEXTURE_2D, 0);

    // Combined vertex data (main texture + radio button)
    float combinedTextureVertices[] = {
        // Main texture vertices
         0.9f,  0.4f, 1.0f, 1.0f,  // Top-right
         0.9f,  0.0f, 1.0f, 0.0f,  // Bottom-right
         0.0f,  0.0f, 0.0f, 0.0f,  // Bottom-left
         0.0f,  0.4f, 0.0f, 1.0f,  // Top-left

         // Radio button vertices
          0.05f,  0.65f, 1.0f, 1.0f, // Top-right
          0.05f,  0.35f, 1.0f, 0.0f, // Bottom-right
         -0.15f,   0.35f, 0.0f, 0.0f, // Bottom-left
         -0.15f,   0.65f, 0.0f, 1.0f  // Top-left
    };

    // Combined indices (main texture + radio button)
    unsigned int combinedIndices[] = {
        // Main texture
        0, 1, 3,  // First triangle
        1, 2, 3,  // Second triangle

        // Radio button
        4, 5, 7,  // First triangle
        5, 6, 7   // Second triangle
    };


    // Create VAO, VBO, and EBO
    unsigned int textureVAO, textureVBO, EBO;
    glGenVertexArrays(1, &textureVAO);
    glGenBuffers(1, &textureVBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(textureVAO);

    // Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(combinedTextureVertices), combinedTextureVertices, GL_STATIC_DRAW);

    // Element Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(combinedIndices), combinedIndices, GL_STATIC_DRAW);

    // Vertex Attribute Pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);



    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP +++++++++++++++++++++++++++++++++++++++++++++++++
    std::chrono::duration<double, std::milli> sleep_duration(1000.0 / 60.0);
    glClearColor(0.02, 0.0, 0.02, 1.0);

    float needleAngle = 120.0f;
    bool changeCourse = false;

    float speed = 0.0f;
    int gear = 0;
    float tachometerValue = 0.0f;

    bool leftBlinkerActive = false;
    bool rightBlinkerActive = false;
    float blinkerStartTime = 0.0f; // Time when blinking started
    int blinkerCount = 0;          // Number of completed blinks
    const int maxBlinks = 3;       // Total blinks
    const float blinkInterval = 0.5f; // Interval in seconds for on/off blinking

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of the last frame

    bool mouseInsideRadioButton = false;
    static double lastClickTime = 0.0;
    bool isVisorOpening = false;
    bool isVisorClosed = true;
    float visorHeight = 0.5f;  // Initial height (closed)
    float visorSpeed = 0.01f;  // Speed of opening/closing


    // Define initial parameters
    float initialY = 1.0f;
    float targetY = 0.1f;
    bool movingDown = false;  // Determines direction of movement
    float translationY = initialY;  // Starting position
    float startTime = -1.0f;  // Start time of movement (used for smooth animation)
    float duration = 2.0f;  // Duration for the complete move (in seconds)
    while (!glfwWindowShouldClose(window))
    {
        //// Enables correct tranparency
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        // UNIVERSAL VALUES
        // Get window size and calculate aspect ratio
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspectRatio = (float)width / (float)height;

        // Create projection matrix adjusted for aspect ratio
        glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
        unsigned int projectionLoc;

        // ++++++++++++++++++++++++++++++++ AC RENDERING +++++++++++++++++++++++++++++ 
        glUseProgram(ac.getAcShader());

        // Handle input to change temperature
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            ac.setCurrentTemperature(std::min(ac.getCurrentTemperature() + 0.1f, ac.getMaxTemperature()));
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            ac.setCurrentTemperature(std::max(ac.getCurrentTemperature() - 0.1f, ac.getMinTemperature()));
        }

        // Pass the projection matrix
        projectionLoc = glGetUniformLocation(ac.getAcShader(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

        glm::vec2 circleCenter = glm::vec2(-0.125f, -0.5f);
        unsigned int circleCenterLoc = glGetUniformLocation(ac.getAcShader(), "circleCenter");
        glUniform2fv(circleCenterLoc, 1, &circleCenter[0]);
       
        // Pass the radius of the circle (same for both)
        float radius = 0.2f;  // Or any dynamic value for the radius
        unsigned int radiusLoc = glGetUniformLocation(ac.getAcShader(), "radius");
        glUniform1f(radiusLoc, radius);

        // Pass the temperature ratio (mapped to [0, 1])
        ac.mapTemperatureToRatio(); // Normalize the temperature
        ac.mapTemperatureToRatio(); // Get normalized temperature
       /* std::cout << ac.getCurrentTemperature() << std::endl;
        std::cout << ac.getTemperatureRatio() << std::endl;
        std::cout << "-----------" << std::endl;*/
        GLuint temperatureLoc = glGetUniformLocation(ac.getAcShader(), "temperatureRatio");
        glUniform1f(temperatureLoc, ac.getTemperatureRatio());

        // Draw the circle
        glBindVertexArray(acVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, ac.getCircle().getNumSegments() + 2);
        glBindVertexArray(0);

        // ----------------------------------- Text display
        glUseProgram(ac.getTextShader());

        glUniform1i(glGetUniformLocation(ac.getTextShader(), "isGearText"), false);

        projection = glm::ortho(0.0f, 5000.0f, 0.0f, 3000.0f);
        glUniformMatrix4fv(glGetUniformLocation(ac.getTextShader(), "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniform3f(glGetUniformLocation(ac.getTextShader(), "textColorStart"), 0.7f, 0.7f, 0.7f);
        glUniform3f(glGetUniformLocation(ac.getTextShader(), "textColorEnd"), 0.2f, 0.2f, 0.2f);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(textVAO);

        std::string currTemperatureLabel = std::to_string(ac.getCurrentTemperature()).substr(0,3) + "C";//std::to_string(speed);

        float x = 2155.0f;
        float y = 790.0f;

        // Only translate the text based on its circular trajectory
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

        // No rotation applied here
        glUniformMatrix4fv(glGetUniformLocation(ac.getTextShader(), "model"), 1, GL_FALSE, &model[0][0]);

        float scale = 3.0f; // Adjust scale as needed
        float xOffset = 0.0f;

        for (char c : currTemperatureLabel) {
            Character ch = Characters.at(c);

            float xpos = xOffset + ch.Bearing.x * scale;
            float ypos = -ch.Bearing.y * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // Update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);
            xOffset += (ch.Advance >> 6) * scale; // Advance is in 1/64 pixels
        }

        glBindVertexArray(0);

        // ++++++++++++++++++++++++++++++++ GAS PROGRESS BAR RENDERING +++++++++++++++++++++++++++++ 
        glUseProgram(gasPaddle.getProgressBarShader());

        // Update speed based on key press
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            speedUp(&car);
            speed = car.getSpeed();
        }
        else {
            slowDown(&car);
            speed = car.getSpeed();
        }
        //std::cout << speed << std::endl;
        // Normalize the speed to a value between 0.0 and 1.0 (for gas level)
        float normalizedSpeed = std::min(1.0f, std::max(0.0f, speed / car.getMaxSpeed()));

        // Update the gas level based on normalized speed
        gasLevel = normalizedSpeed;

        gasPaddle.setGasLevel(gasLevel);
        //std::cout << gasPaddle.getGasLevel() << std::endl;

        gasPaddle.updateProgressBar();

        glBindBuffer(GL_ARRAY_BUFFER, progressBarVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, gasPaddle.getProgressBarVertices().size() * sizeof(float), gasPaddle.getProgressBarVertices().data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Set projection matrix
        projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
        projectionLoc = glGetUniformLocation(gasPaddle.getProgressBarShader(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

        glUniform3f(glGetUniformLocation(gasPaddle.getProgressBarShader(), "color"), 0.0f, 1.0f, 0.0f); // Black color for background

        // Render the progress bar
        glBindVertexArray(progressBarVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        // ++++++++++++++++++++++++++++++++ FUEL TANK PROGRESS BAR RENDERING +++++++++++++++++++++++++++++ 
        glUseProgram(fuelTank.getFuelTankShader());

        // Get current fuel and calculate fill ratio
        currentFuel = car.getFuelAmount();
        fillRatio = currentFuel / maxFuel;

        /*std::cout << "Current Fuel: " << currentFuel << std::endl;
        std::cout << "Max Fuel: " << maxFuel << std::endl;
        std::cout << "Fill Ratio: " << fillRatio << std::endl;
        std::cout << "---------------" << std::endl;*/

        // Update the fill vertices based on the new fill ratio
        fuelTank.updateFuelTankVertices(fillRatio);

        // Update the buffer with the new fill vertices        
        size_t fillOffset = fuelTank.getFuelTankVertices().size() * sizeof(float);
        glBindBuffer(GL_ARRAY_BUFFER, fuelBarVBO);
        glBufferSubData(GL_ARRAY_BUFFER, fillOffset, fuelTank.getFuelTankVertices().size() * sizeof(float), fuelTank.getFuelTankVertices().data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render the background
        glBindVertexArray(fuelBarVAO);
        glUniform3f(glGetUniformLocation(fuelTank.getFuelTankShader(), "color"), 1.0f, 0.0f, 0.0f); // Black color for background
        glDrawArrays(GL_LINE_LOOP, 0, 4); // Use first 4 vertices for the background
        glBindVertexArray(0);

        // Render the fill
        glBindVertexArray(fuelBarVAO);
        glUniform3f(glGetUniformLocation(fuelTank.getFuelTankShader(), "color"), 0.0f, 1.0f, 0.0f); // Green color for the fill
        glDrawArrays(GL_TRIANGLE_FAN, 4, 4); // Use next 4 vertices for the fill
        glBindVertexArray(0);

        // ++++++++++++++++++++++++++++++++ RADIO RENDERING +++++++++++++++++++++++++++++
        glUseProgram(textShader);

        std::string currRadioLabel = "Turn on radio";

        x = 300.0f;
        y = 300.0f;

        // Set up the projection matrix
        projection = glm::ortho(0.0f, 5000.0f, 0.0f, 3000.0f);
        glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, &projection[0][0]);


        // Only translate the text based on its circular trajectory
        model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

        // No rotation applied here
        glUniformMatrix4fv(glGetUniformLocation(textShader, "model"), 1, GL_FALSE, &model[0][0]);

        scale = 3.0f; // Adjust scale as needed
        xOffset = 0.0f;

        for (char c : currRadioLabel) {
            Character ch = Characters.at(c);

            float xpos = xOffset + ch.Bearing.x * scale;
            float ypos = -ch.Bearing.y * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // Update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);
            xOffset += (ch.Advance >> 6) * scale; // Advance is in 1/64 pixels
        }

        glBindVertexArray(0);
        glUseProgram(0);
        
        // ------------------ background rendering
        glUseProgram(radio.getRadioShader());

        projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(radio.getRadioShader(), "projection"), 1, GL_FALSE, &projection[0][0]);
        //glUniform3f(glGetUniformLocation(radio.getRadioShader(), "color"), 0.2f, 0.0f, 0.2f); // Blue rectangles

        glBindVertexArray(radioVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

        // Cleanup
        glBindVertexArray(0);

        // Get mouse position in window coordinates
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Get window dimensions
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        // Convert mouse coordinates to OpenGL normalized device coordinates (NDC)
        float xNDC = 2.0f * (mouseX / windowWidth) - 1.0f;
        float yNDC = 1.0f - 2.0f * (mouseY / windowHeight);

        // Rectangle bounds
        float left = -0.11f;
        float right = 0.0f;
        float bottom = 0.4f;
        float top = 0.57f;

        // Check if the click is within the rectangle
        if (xNDC >= left && xNDC <= right && yNDC >= bottom && yNDC <= top) {
            //std::cout << "Inside radio button" << std::endl;
            mouseInsideRadioButton = true;
        }
        else {
            //std::cout << "Outside radio button" << std::endl;
            mouseInsideRadioButton = false;
        }

        float currentTime = glfwGetTime(); // Get current time in seconds
        if (mouseInsideRadioButton == true) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                if (currentTime - lastClickTime >= 0.5) { // 0.5 seconds debounce time
                    if (radio.getIsRadioOn() == false)
                        radio.setIsRadioOn(true);
                    else
                        radio.setIsRadioOn(false);

                    lastClickTime = currentTime; // Update the last click time
                }
            }
        }

        // ----------------- text rendering
        if (radio.getIsRadioOn() == true) {
            glUseProgram(textShader);


            float currentFrame = glfwGetTime(); // Get the current time in seconds
            deltaTime = currentFrame - lastFrame; // Calculate the time difference
            lastFrame = currentFrame; // Update the last frame time

            if (radio.getIsRadioOn()) {
                // Gradually increase alpha for fade-in effect
                float textAlpha = radio.getTextAlpha() + deltaTime; // deltaTime must be precomputed
                radio.setTextAlpha(std::min(textAlpha, 1.0f)); // Cap alpha at 1.0f

                // Scroll the text using the `model` matrix
                float textPosX = radio.getTextPositionX() - 300.0f * deltaTime; // Adjust scrolling speed
                if (textPosX < -radio.getRadioScreenWidth()) {
                    textPosX = 3000.0f; // Reset to the right side
                }
                radio.setPositionX(textPosX);

                /*std::cout << textPosX << std::endl;
                std::cout << deltaTime << std::endl;*/

                // Compute the model matrix for translation
                glm::mat4 model = glm::mat4(1.0f); // Identity matrix
                model = glm::translate(model, glm::vec3(textPosX, 1550.0f, 0.0f)); // Translate X and Y
                glUniformMatrix4fv(glGetUniformLocation(textShader, "model"), 1, GL_FALSE, &model[0][0]);

                // Activate texture and bind VAO
                glActiveTexture(GL_TEXTURE0);
                glBindVertexArray(textVAO);

                // Render the radio station name
                std::string radioStationName = "Cool FM 107.5";
                scale = 2.0f; // Adjust as needed
                xOffset = 0.0f;

                for (char c : radioStationName) {
                    Character ch = Characters.at(c);

                    float xpos = xOffset + ch.Bearing.x * scale;
                    float ypos = -(ch.Bearing.y * scale); // Adjust for alignment

                    float w = ch.Size.x * scale;
                    float h = ch.Size.y * scale;

                    // Update VBO for the quad vertices of this character
                    float vertices[6][4] = {
                        { xpos,     ypos + h,   0.0f, 0.0f },
                        { xpos,     ypos,       0.0f, 1.0f },
                        { xpos + w, ypos,       1.0f, 1.0f },

                        { xpos,     ypos + h,   0.0f, 0.0f },
                        { xpos + w, ypos,       1.0f, 1.0f },
                        { xpos + w, ypos + h,   1.0f, 0.0f }
                    };

                    glBindTexture(GL_TEXTURE_2D, ch.TextureID);

                    // Update buffer and render the character
                    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);

                    glDrawArrays(GL_TRIANGLES, 0, 6);

                    // Advance to the next character
                    xOffset += (ch.Advance >> 6) * scale; // Advance is in 1/64 pixels
                }

                // Cleanup
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            glUseProgram(0);
        }
        else 
            glUseProgram(0);

        // rendering boundaries
        glUseProgram(radio.getRadioShader());
        
        projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(radio.getRadioShader(), "projection"), 1, GL_FALSE, &projection[0][0]);
        //glUniform3f(glGetUniformLocation(radio.getRadioShader(), "color"), 0.02f, 0.0f, 0.02f); // Blue rectangles

              
        // Render the left rectangle
        glBindVertexArray(radioVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        // Render the right rectangle
        glDrawArrays(GL_TRIANGLE_FAN, 4, 4);


        // Cleanup
        glBindVertexArray(0);
        glUseProgram(0);

        // Unbind shader
        glUseProgram(0);

        // ++++++++++++++++++++++++++++++++ SPEEDOMETER RENDERING +++++++++++++++++++++++++++++
        // ----------------------------------- Circle display
        glUseProgram(speedometer.getCircleShader());

        // Create projection matrix adjusted for aspect ratio
        projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
        projectionLoc = glGetUniformLocation(speedometer.getCircleShader(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

        // Pass resolution uniform to the shader for gradient calculation
        glm::vec2 resolution = glm::vec2(width, height); // Pass the resolution for gradient scaling
        unsigned int resolutionLoc = glGetUniformLocation(speedometer.getCircleShader(), "resolution");
        glUniform2fv(resolutionLoc, 1, &resolution[0]);

        // Pass the radius of the circle (same for both)
        radius = 0.5f;  // Or any dynamic value for the radius
        radiusLoc = glGetUniformLocation(speedometer.getCircleShader(), "radius");
        glUniform1f(radiusLoc, radius);

        circleCenter = glm::vec2(-1.0f, 0.0f);
        circleCenterLoc = glGetUniformLocation(speedometer.getCircleShader(), "circleCenter");
        glUniform2fv(circleCenterLoc, 1, &circleCenter[0]);

        // Set `useGradient` for the outer circle (background)
        bool useGradient = false;  // For the outer circle (no gradient or static color)
        unsigned int useGradientLoc = glGetUniformLocation(speedometer.getCircleShader(), "useGradient");
        glUniform1i(useGradientLoc, useGradient);

        // Draw the outer circle (background)
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, speedometer.getCircleOuter().getNumSegments() /*numSegments*/ + 2); // Draw the outer circle (large circle)

        // Set `useGradient` for the inner circle (gradient effect)
        useGradient = true;  // For the small circle (inner circle)
        glUniform1i(useGradientLoc, useGradient);

        // Draw the inner circle (small circle)    
        glDrawArrays(GL_TRIANGLE_FAN, speedometer.getCircleInner().getNumSegments() + 2, speedometer.getCircleInner().getNumSegments() + 2); // Draw the inner circle
        glBindVertexArray(0);

        // ----------------------------------- Tick display
        glUseProgram(speedometer.getTicksShader());

        // Set projection matrix for ticks
        projectionLoc = glGetUniformLocation(speedometer.getTicksShader(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

        glBindVertexArray(tickVAO);

        // Loop through the ticks and alternate opacity and line width
        for (int i = 0; i < speedometer.getTickVertices().size() / 2; ++i) {  // Each tick has 2 points (outer + inner)       
            float opacity = 0.5f;  // Default opacity for even ticks

            if (i % 2 == 1) {
                opacity *= 0.5f;  // Halve opacity for every second tick

                // Set line thickness for every second tick (example: thinner line)
                glLineWidth(1.5f);  // Make lines thinner for odd ticks
            }
            else {
                glLineWidth(3.0f);  // Set thicker line for even ticks
            }

            // Pass tick color
            unsigned int tickColorLoc = glGetUniformLocation(speedometer.getTicksShader(), "tickColor");
            if (i % 2 == 1) {
                glUniform3f(tickColorLoc, 1.0f, 1.0f, 1.0f); // Set tick color to white for odd ticks
            }
            else {
                glUniform3f(tickColorLoc, 1.0f, 0.012f, 0.012f); // Set tick color to red for even ticks
            }

            // Pass opacity
            unsigned int opacityLoc = glGetUniformLocation(speedometer.getTicksShader(), "opacity");
            glUniform1f(opacityLoc, opacity);

            // Draw one tick (two points for each tick)
            glDrawArrays(GL_LINES, i * 2, 2); // Each tick consists of two vertices (outer + inner)
        }

        glBindVertexArray(0);

        // ----------------------------------- Text display
        glUseProgram(speedometer.getTextShader());

        glUniform1i(glGetUniformLocation(speedometer.getTextShader(), "isGearText"), false);

        projection = glm::ortho(0.0f, 5000.0f, 0.0f, 3000.0f);
        glUniformMatrix4fv(glGetUniformLocation(speedometer.getTextShader(), "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniform3f(glGetUniformLocation(speedometer.getTextShader(), "textColorStart"), 0.4f, 0.2f, 0.4f);
        glUniform3f(glGetUniformLocation(speedometer.getTextShader(), "textColorEnd"), 0.2f, 0.2f, 0.2f);
        
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(textVAO);

        std::vector<std::string> labels = { "0", "30", "60", "90", "120", "150", "180", "210", "240", "270" };
        int numLabels = labels.size();

        // Angles in degrees
        float startAngle = 225.0f; // Adjust as needed for where text starts
        float endAngle = -45.0f;   // Adjust as needed for where text ends
        radius = 360.0f;
        for (int i = 0; i < numLabels; ++i) {
            float angle = glm::radians(startAngle + i * (endAngle - startAngle) / (numLabels - 1));
            float x = 1025.0f /*1050*/ + radius * cos(angle);
            float y = 1590.0f + radius * sin(angle);

            // Only translate the text based on its circular trajectory
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

            // No rotation applied here
            glUniformMatrix4fv(glGetUniformLocation(speedometer.getTextShader(), "model"), 1, GL_FALSE, &model[0][0]);

            // Render the text
            std::string label = labels[i];
            float scale = 1.8f; // Adjust scale as needed
            float xOffset = 0.0f;

            for (char c : label) {
                Character ch = Characters.at(c);

                float xpos = xOffset + ch.Bearing.x * scale;
                float ypos = -ch.Bearing.y * scale;

                float w = ch.Size.x * scale;
                float h = ch.Size.y * scale;

                // Update VBO for each character
                float vertices[6][4] = {
                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos,     ypos,       0.0f, 1.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },

                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },
                    { xpos + w, ypos + h,   1.0f, 0.0f }
                };

                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                glBindBuffer(GL_ARRAY_BUFFER, textVBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDrawArrays(GL_TRIANGLES, 0, 6);
                xOffset += (ch.Advance >> 6) * scale; // Advance is in 1/64 pixels
            }
        }


        // TEXT THAT SHOWS CURRENT SPEED
        std::string currSpeedLabel = "000";//std::to_string(speed);
        if (speed < 10) {
            currSpeedLabel = "00" + std::to_string(speed).substr(0, 1);
        }
        else if (speed < 100) {
            currSpeedLabel = "0" + std::to_string(speed).substr(0, 2);
        }
        else {
            currSpeedLabel = std::to_string(speed).substr(0, 3);
        }

       /* float x = 970.0f;
        float y = 1190.0f;*/
        x = 970.0f;
        y = 1190.0f;

        // Only translate the text based on its circular trajectory
        //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
        model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

        // No rotation applied here
        glUniformMatrix4fv(glGetUniformLocation(speedometer.getTextShader(), "model"), 1, GL_FALSE, &model[0][0]);

        //float scale = 3.0f; // Adjust scale as needed
        //float xOffset = 0.0f;
        scale = 3.0f; // Adjust scale as needed
        xOffset = 0.0f;
        
        for (char c : currSpeedLabel) {
            Character ch = Characters.at(c);

            float xpos = xOffset + ch.Bearing.x * scale;
            float ypos = -ch.Bearing.y * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // Update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);
            xOffset += (ch.Advance >> 6) * scale; // Advance is in 1/64 pixels
        }

        glBindVertexArray(0);

        // ----------------------------------- Needle display
        glUseProgram(speedometer.getNeedleShader());

        needleAngle = glm::mix(120.0f, -120.0f, (speed - 0.0f) / (270.0f - 0.0f));

        // Dynamically change the needle angle (this could be based on speed or some other input)
        if (needleAngle == -120.0f)
            changeCourse = true;
        else if (needleAngle == 120)
            changeCourse = false;

        if (!changeCourse)
            needleAngle -= 1.0f;  // Example: this could be your speed value or other input
        else
            needleAngle += 1.0f;  // Example: this could be your speed value or other input
        float angleRadians = glm::radians(needleAngle);  // Convert degrees to radians

        // Create the model matrix: apply translation and rotation
        glm::mat4 needleModel = glm::mat4(1.0f);
        needleModel = glm::translate(needleModel, glm::vec3(-1.0f, 0.0f, 0.0f)); // Shift to the tachometer center
        needleModel = glm::rotate(needleModel, angleRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around the Z-axis

        // Pass the projection matrix to the shader
        projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
        projectionLoc = glGetUniformLocation(speedometer.getNeedleShader(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

        // Pass the model matrix to the shader for rotation and translation
        unsigned int modelLoc = glGetUniformLocation(speedometer.getNeedleShader(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &needleModel[0][0]);

        // Set the needle color (optional)
        unsigned int needleColorLoc = glGetUniformLocation(speedometer.getNeedleShader(), "needleColor");
        if (speed < 30.0f)
            glUniform3f(needleColorLoc, 0.0f, 0.0f, 1.0f); // Red color for the needle

        // Draw the needle
        glBindVertexArray(needleVAO);
        glDrawArrays(GL_LINES, 0, 2);  // Draw the needle (line from center to tip)
        glBindVertexArray(0);


        // ++++++++++++++++++++++++++++++++ TACHOMETER RENDERING +++++++++++++++++++++++++++++
        // ----------------------------------- Circle display
        glUseProgram(tachometer.getCircleShader());

        // Set the projection matrix
        projectionLoc = glGetUniformLocation(tachometer.getCircleShader(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

        // Set the resolution
        resolutionLoc = glGetUniformLocation(tachometer.getCircleShader(), "resolution");
        glUniform2fv(resolutionLoc, 1, &resolution[0]);

        // Set the circle center
        circleCenter = glm::vec2(0.75f, 0.0f);
        circleCenterLoc = glGetUniformLocation(tachometer.getCircleShader(), "circleCenter");
        glUniform2fv(circleCenterLoc, 1, &circleCenter[0]);

        // Bind the VAO once for both circles
        glBindVertexArray(tachometerVAO);

        // Draw the outer circle
        useGradient = false; // Static color for the outer circle
        useGradientLoc = glGetUniformLocation(tachometer.getCircleShader(), "useGradient");
        glUniform1i(useGradientLoc, useGradient);

        radiusLoc = glGetUniformLocation(tachometer.getCircleShader(), "radius");
        glUniform1f(radiusLoc, tachometer.getCircleOuter().getRadius());

        glDrawArrays(GL_TRIANGLE_FAN, 0, tachometer.getCircleOuter().getNumSegments() + 2); // +2 for the center and closing vertex

        // Draw the inner circle
        useGradient = true; // Enable gradient for the inner circle
        glUniform1i(useGradientLoc, useGradient);

        radiusLoc = glGetUniformLocation(tachometer.getCircleShader(), "radius");
        glUniform1f(radiusLoc, tachometer.getCircleInner().getRadius());

        int innerCircleStartIndex = tachometer.getCircleOuter().getNumSegments() + 2; // Start index for inner circle
        glDrawArrays(GL_TRIANGLE_FAN, innerCircleStartIndex, tachometer.getCircleInner().getNumSegments() + 2);

        glBindVertexArray(0);

        // ----------------------------------- Tick display
        glUseProgram(tachometer.getTicksShader());

        // Set projection matrix for ticks
        projectionLoc = glGetUniformLocation(tachometer.getTicksShader(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

        glBindVertexArray(tachometerTickVAO);

        // Loop through the ticks and alternate opacity and line width
        for (int i = 0; i < tachometer.getTickVertices().size() / 2; ++i) {  // Each tick has 2 points (outer + inner)       
            float opacity = 0.5f;  // Default opacity for even ticks

            if (i % 2 == 1) {
                opacity *= 0.5f;  // Halve opacity for every second tick

                // Set line thickness for every second tick (example: thinner line)
                glLineWidth(1.5f);  // Make lines thinner for odd ticks
            }
            else {
                glLineWidth(3.0f);  // Set thicker line for even ticks
            }

            // Pass tick color
            unsigned int tickColorLoc = glGetUniformLocation(tachometer.getTicksShader(), "tickColor");
            if (i % 2 == 1) {
                glUniform3f(tickColorLoc, 1.0f, 1.0f, 1.0f); // Set tick color to white for odd ticks
            }
            else {
                glUniform3f(tickColorLoc, 1.0f, 0.012f, 0.012f); // Set tick color to red for even ticks
            }

            // Pass opacity
            unsigned int opacityLoc = glGetUniformLocation(tachometer.getTicksShader(), "opacity");
            glUniform1f(opacityLoc, opacity);

            // Draw one tick (two points for each tick)
            glDrawArrays(GL_LINES, i * 2, 2); // Each tick consists of two vertices (outer + inner)
        }

        glBindVertexArray(0);

        // ----------------------------------- Text display
        glUseProgram(tachometer.getTextShader());

        glUniform1i(glGetUniformLocation(tachometer.getTextShader(), "isGearText"), false);

        projection = glm::ortho(0.0f, 5000.0f, 0.0f, 3000.0f);
        glUniformMatrix4fv(glGetUniformLocation(tachometer.getTextShader(), "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorStart"), 0.4f, 0.2f, 0.4f);
        glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorEnd"), 0.2f, 0.2f, 0.2f);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(textVAO);

        labels = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
        numLabels = labels.size();

        // Angles in degrees
        startAngle = 225.0f; // Adjust as needed for where text starts
        endAngle = -45.0f;   // Adjust as needed for where text ends
        radius = 360.0f;
        for (int i = 0; i < numLabels; ++i) {
            float angle = glm::radians(startAngle + i * (endAngle - startAngle) / (numLabels - 1));
            float x = 3530.0f /*3510.0f*/ + radius * cos(angle);
            float y = 1590.0f + radius * sin(angle);

            // Only translate the text based on its circular trajectory
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

            // No rotation applied here
            glUniformMatrix4fv(glGetUniformLocation(textShader, "model"), 1, GL_FALSE, &model[0][0]);

            // Render the text
            std::string label = labels[i];
            float scale = 1.8f; // Adjust scale as needed
            float xOffset = 0.0f;

            for (char c : label) {
                Character ch = Characters.at(c);

                float xpos = xOffset + ch.Bearing.x * scale;
                float ypos = -ch.Bearing.y * scale;

                float w = ch.Size.x * scale;
                float h = ch.Size.y * scale;

                // Update VBO for each character
                float vertices[6][4] = {
                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos,     ypos,       0.0f, 1.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },

                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },
                    { xpos + w, ypos + h,   1.0f, 0.0f }
                };

                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                glBindBuffer(GL_ARRAY_BUFFER, textVBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDrawArrays(GL_TRIANGLES, 0, 6);
                xOffset += (ch.Advance >> 6) * scale; // Advance is in 1/64 pixels
            }
        }

        
        gear = car.getGear();
        // std::cout << gear << std::endl;

        // TEXT THAT SHOWS CURRENT GEAR
        std::string currGearLabel = std::to_string(gear);

        x = 3530.0f;
        y = 1190.0f;

        glUniform1i(glGetUniformLocation(tachometer.getTextShader(), "isGearText"), true);
        glUniform1f(glGetUniformLocation(tachometer.getTextShader(), "time"), glfwGetTime() * 3); // Pass current time

        if (gear == 1) {
            glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorStart"), 1.0f, 0.0f, 0.0f);
            glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorEnd"), 0.02f, 0.0f, 0.02f);
        }
        else if (gear == 2) {
            glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorStart"), 1.0f, 0.5f, 0.0f);
            glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorEnd"), 0.02f, 0.0f, 0.02f);
        }
        else if (gear == 3) {
            glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorStart"), 1.0f, 0.0f, 0.7f);
            glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorEnd"), 0.02f, 0.0f, 0.02f);
        }
        else if (gear == 4) {
            glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorStart"), 0.2f, 0.2f, 1.0f);
            glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorEnd"), 0.02f, 0.0f, 0.02f);
        }
        else if (gear == 5) {
            glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorStart"), 0.4f, 1.0f, 0.4f);
            glUniform3f(glGetUniformLocation(tachometer.getTextShader(), "textColorEnd"), 0.02f, 0.0f, 0.02f);
        }


        // Only translate the text based on its circular trajectory
        model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

        // No rotation applied here
        glUniformMatrix4fv(glGetUniformLocation(tachometer.getTextShader(), "model"), 1, GL_FALSE, &model[0][0]);

        scale = 3.0f; // Adjust scale as needed
        xOffset = 0.0f;

        for (char c : currGearLabel) {
            Character ch = Characters.at(c);

            float xpos = xOffset + ch.Bearing.x * scale;
            float ypos = -ch.Bearing.y * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // Update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);
            xOffset += (ch.Advance >> 6) * scale; // Advance is in 1/64 pixels
        }

        glBindVertexArray(0);

        // ----------------------------------- Needle display
        glUseProgram(tachometer.getNeedleShader());

        tachometerValue = car.getTachometer();
        // std::cout << tachometerValue << std::endl;
        if (speed <= 1) {
            needleAngle = glm::mix(120.0f, -120.0f, (1.0f - 0.0f) / (8849.47f - 0.0f));
        }
        else {
            needleAngle = glm::mix(120.0f, -120.0f, (tachometerValue - 0.0f) / (8849.47f - 0.0f));
        }

        // Dynamically change the needle angle (this could be based on speed or some other input)
        if (needleAngle == -120.0f)
            changeCourse = true;
        else if (needleAngle == 120)
            changeCourse = false;

        if (!changeCourse)
            needleAngle -= 1.0f;  // Example: this could be your speed value or other input
        else
            needleAngle += 1.0f;  // Example: this could be your speed value or other input
        angleRadians = glm::radians(needleAngle);  // Convert degrees to radians

        // Create the model matrix: apply translation and rotation
        needleModel = glm::mat4(1.0f);
        needleModel = glm::translate(needleModel, glm::vec3(0.75f, 0.0f, 0.0f)); // Shift to the tachometer center
        needleModel = glm::rotate(needleModel, angleRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around the Z-axis

        // Pass the projection matrix to the shader
        projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
        projectionLoc = glGetUniformLocation(tachometer.getNeedleShader(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

        // Pass the model matrix to the shader for rotation and translation
        modelLoc = glGetUniformLocation(tachometer.getNeedleShader(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &needleModel[0][0]);

        // Set the needle color (optional)
        needleColorLoc = glGetUniformLocation(tachometer.getNeedleShader(), "needleColor");
        glUniform3f(needleColorLoc, 1.0f, 0.0f, 0.0f); // Red color for the needle

        // Draw the needle
        glBindVertexArray(tachometerNeedleVAO);
        glDrawArrays(GL_LINES, 0, 2);  // Draw the needle (line from center to tip)
        glBindVertexArray(0);

        // ++++++++++++++++++++++++++++++++ BLINKERS ++++++++++++++++++++++++
        glUseProgram(blinkers.getBlinkersShader());
        
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && !leftBlinkerActive && !rightBlinkerActive) {
            leftBlinkerActive = true;
            blinkerStartTime = glfwGetTime(); // Start blinking
            blinkerCount = 0;                 // Reset blink count
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !rightBlinkerActive && !leftBlinkerActive) {
            rightBlinkerActive = true;
            blinkerStartTime = glfwGetTime(); // Start blinking
            blinkerCount = 0;                 // Reset blink count
        }

        currentTime = glfwGetTime();

        // Set the projection matrix
        glUniformMatrix4fv(glGetUniformLocation(blinkers.getBlinkersShader(), "projection"), 1, GL_FALSE, &projection[0][0]);

        glBindVertexArray(indicatorVAO);

        // Left blinker translation (move left)
        glm::mat4 leftModel = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, 0.0f, 0.0f)); // Move left
        glUniformMatrix4fv(glGetUniformLocation(blinkers.getBlinkersShader(), "model"), 1, GL_FALSE, &leftModel[0][0]);
        glUniform3f(glGetUniformLocation(blinkers.getBlinkersShader(), "inputColor"), 0.0f, 1.0f, 0.0f); // Yellow color
        glUniform1f(glGetUniformLocation(blinkers.getBlinkersShader(), "alpha"), 0.4f); // Full opacity
        glDrawArrays(GL_TRIANGLES, 0, 3);  // Draw the left triangle (first 6 vertices)

        // Right blinker translation (move right)
        glm::mat4 rightModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.0f, 0.0f)); // Move right
        glUniformMatrix4fv(glGetUniformLocation(blinkers.getBlinkersShader(), "model"), 1, GL_FALSE, &rightModel[0][0]);
        glUniform3f(glGetUniformLocation(blinkers.getBlinkersShader(), "inputColor"), 0.0f, 1.0f, 0.0f); // Red color
        glUniform1f(glGetUniformLocation(blinkers.getBlinkersShader(), "alpha"), 0.4f); // Full opacity
        glDrawArrays(GL_TRIANGLES, 6, 3);  // Draw the right triangle (next 6 vertices)

        // Render the left blinker
        if (leftBlinkerActive && !rightBlinkerActive) {
            float elapsed = currentTime - blinkerStartTime;

            // Blink on/off based on the interval
            bool isBlinkOn = static_cast<int>(elapsed / blinkInterval) % 2 == 0;

            // Render the left blinker if it's "on"
            if (isBlinkOn) {
                glm::mat4 leftModel = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, 0.0f, 0.0f));
                glUniformMatrix4fv(glGetUniformLocation(blinkers.getBlinkersShader(), "model"), 1, GL_FALSE, &leftModel[0][0]);
                glUniform3f(glGetUniformLocation(blinkers.getBlinkersShader(), "inputColor"), 0.0f, 1.0f, 0.0f); // Yellow
                glUniform1f(glGetUniformLocation(blinkers.getBlinkersShader(), "alpha"), 1.0f); // Full opacity
                glDrawArrays(GL_TRIANGLES, 3, 3);
            }

            // Check if the blinker should stop
            if (elapsed >= maxBlinks * 2 * blinkInterval) {
                leftBlinkerActive = false; // Stop blinking
            }
        }

        // Render the right blinker
        if (rightBlinkerActive && !leftBlinkerActive) {
            float elapsed = currentTime - blinkerStartTime;

            // Blink on/off based on the interval
            bool isBlinkOn = static_cast<int>(elapsed / blinkInterval) % 2 == 0;

            // Render the right blinker if it's "on"
            if (isBlinkOn) {
                glm::mat4 rightModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.0f, 0.0f));
                glUniformMatrix4fv(glGetUniformLocation(blinkers.getBlinkersShader(), "model"), 1, GL_FALSE, &rightModel[0][0]);
                glUniform3f(glGetUniformLocation(blinkers.getBlinkersShader(), "inputColor"), 0.0f, 1.0f, 0.0f); // Yellow
                glUniform1f(glGetUniformLocation(blinkers.getBlinkersShader(), "alpha"), 1.0f); // Full opacity
                glDrawArrays(GL_TRIANGLES, 9, 3);
            }

            // Check if the blinker should stop
            if (elapsed >= maxBlinks * 2 * blinkInterval) {
                rightBlinkerActive = false; // Stop blinking
            }
        }

        glBindVertexArray(0);

        // ++++++++++++++++++++++++++++++++++++ FRONT PANEL +++++++++++++++++++++++++++++++
        glUseProgram(panelShader);

        projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(panelShader, "projection"), 1, GL_FALSE, &projection[0][0]);
        resolution = glm::vec2(width, height); // Pass the resolution for gradient scaling
        resolutionLoc = glGetUniformLocation(panelShader, "resolution");
        glUniform2fv(resolutionLoc, 1, &resolution[0]);

        GLint isVisorLoc = glGetUniformLocation(panelShader, "isVisor");
        glUniform1i(isVisorLoc, false);

        // Render the rectangle with gradient
        glBindVertexArray(panelVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);  // Render the rectangle
        glBindVertexArray(0);
   
        // Cleanup
        glBindVertexArray(0);

        glUseProgram(0);

        //--------------driver licence/visor
        // Handle key press for 'V'
        //currentTime = glfwGetTime();
        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
            if (currentTime - lastClickTime >= 1.1f) { // 0.5 seconds debounce time
                if (startTime == -1.0f) {  // If it's the first time V is pressed
                    movingDown = !movingDown;  // Toggle movement direction
                    startTime = static_cast<float>(glfwGetTime());  // Record start time
                }

                lastClickTime = currentTime; // Update the last click time
            }
        }

        std::cout << translationY << std::endl;
        
        // Update the texture position based on time and movement state
        if (startTime != -1.0f) {
            float elapsedTime = static_cast<float>(glfwGetTime()) - startTime;

            if (movingDown) {
                translationY = initialY - (elapsedTime / duration) * (initialY - targetY);
                if (translationY <= targetY) {
                    translationY = targetY;  // Stop at target position
                }
            }
            else {
                translationY = targetY + (elapsedTime / duration) * (initialY - targetY);
                if (translationY >= initialY) {
                    translationY = initialY;  // Stop at initial position
                }
            }

            // If movement is complete, reset startTime so the movement doesn't repeat automatically
            if (translationY == initialY || translationY == targetY) {
                startTime = -1.0f;  // Reset the start time after the move completes
            }
        }

        // Update the vertices based on translationY (only Y changes)
        float updatedVertices[] = {
            0.9f,  0.9f + translationY, 1.0f, 1.0f,  // Top-right
            0.9f,  0.0f + translationY, 1.0f, 0.0f,  // Bottom-right
            0.0f,  0.0f + translationY, 0.0f, 0.0f,  // Bottom-left
            0.0f,  0.9f + translationY, 0.0f, 1.0f   // Top-left
        };


        glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(updatedVertices), updatedVertices);

        // Render loop
        glUseProgram(textureShader);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, imageTexture);
        glUniform1i(glGetUniformLocation(textureShader, "uTex"), 0);

        glBindVertexArray(textureVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Render radio button
        glBindTexture(GL_TEXTURE_2D, radioButtonTexture);
        glUniform1i(glGetUniformLocation(textureShader, "uTex"), 0);
        glBindVertexArray(textureVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int))); // Draw next 6 indices (radio button)

        glBindVertexArray(0);


        // ++++++++++++++++++++++++++++++++++++ INDEX +++++++++++++++++++++++++++++++
        // ----------------------------------- Text display
        glUseProgram(indexShader);

        // Set up the projection matrix for rendering text
        projection = glm::ortho(0.0f, 5000.0f, 0.0f, 3000.0f);
        glUniformMatrix4fv(glGetUniformLocation(indexShader, "projection"), 1, GL_FALSE, &projection[0][0]);

        // Set the color of the text
        glm::vec3 textColor(0.7f, 0.7f, 0.7f); // Grey color
        glUniform3f(glGetUniformLocation(indexShader, "textColor"), textColor.x, textColor.y, textColor.z);

        // Activate texture unit and bind the VAO for rendering text
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(textVAO);

        // Prepare the string to render
        std::string indexLabel = "Stevan Sapun SV7/2021";

        x = 100.0f;
        y = 100.0f;
        scale = 2.0f; // Adjust scale as needed
        xOffset = 0.0f;

        // Render each character
        for (char c : indexLabel) {
            Character ch = Characters.at(c);

            float xpos = x + xOffset + ch.Bearing.x * scale;
            float ypos = y - (ch.Bearing.y * scale);

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // Update VBO for the quad vertices for this character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);

            // Update buffer and render glyph
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Advance to the next glyph
            xOffset += (ch.Advance >> 6) * scale; // Advance is in 1/64 pixels
        }

        // Clean up state
        glBindVertexArray(0);
        // ++++++++++++++++++++++++++++++++++++ RADIO +++++++++++++++++++++++++++++++
    

        // --------------------------

        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        std::this_thread::sleep_for(sleep_duration);
    }


    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(circleShader);
    endSimulation(&car);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" shader compilation failed! Error: \n");
        printf(infoLog);
    }
    return shader;
}

unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Shader program linking failed! Error: \n" << infoLog << std::endl;
    }
    return program;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}