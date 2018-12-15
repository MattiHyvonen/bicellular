#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string.h>

#include "lodepng.h"
#include "loadShaders.h"

GLFWwindow* window;
GLuint shaders[3] = {0, 0, 0};
GLuint convolutionMatrixID = 0;
GLuint convolutionLevelID = 0;

float randomf() {
    float result = (float)rand() / RAND_MAX;
    return result;
}

const float v = 1.0/256.0;

static const float convolution_laplacian[] = {
    -1.0*v,  -4.0*v,  -6.0*v,  -4.0*v,  -1.0*v,
    -4.0*v, -16.0*v, -24.0*v, -16.0*v,  -4.0*v,
    -6.0*v, -24.0*v, 476.0*v, -24.0*v,  -6.0*v,
    -4.0*v, -16.0*v, -24.0*v, -16.0*v,  -4.0*v,
    -1.0*v,  -4.0*v,  -6.0*v,  -4.0*v,  -1.0*v
};


static const float convolution_blur[] = {
     1.0*v,   4.0*v,   6.0*v,   4.0*v,   1.0*v,
     4.0*v,  16.0*v,  24.0*v,  16.0*v,   4.0*v,
     6.0*v,  24.0*v,  36.0*v,  24.0*v,   6.0*v,
     4.0*v,  16.0*v,  24.0*v,  16.0*v,   4.0*v,
     1.0*v,   4.0*v,   6.0*v,   4.0*v,   1.0*v
};

static const float convolution_default[] = {
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  1,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0
};

static const GLfloat rectangleVertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
    
        -1.0f, 1.0f, 0.0f,
         1.0f,-1.0f, 0.0f,
         1.0f, 1.0f, 0.0f
};

static const GLfloat rectangleUVs[] = {
        0.0f,  0.0f,
        1.0f,  0.0f,
        0.0f,  1.0f,
    
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
};

int makeColor_RGB(unsigned char r, unsigned char g, unsigned char b) {
    int result = 0;
    result = result | (255 << 24);
    result = result | (b << 16);
    result = result | (g << 8);
    result = result | r;
    return result;
}


enum convolutionTypeEnum{
    BLUR,
    LAPLACIAN,
    DEFAULT
};
    

class image{
    GLuint vertexArrayID = 0;
    GLuint vertexBufferID = 0;
    GLuint uvBufferID = 0;
    GLuint textureID = 0;
    GLuint frameBufferID = 0;
    GLuint transformationMatrixID = 0;
    GLuint shaderID = 0;
    GLuint texelOffsetID = 0;
    
    glm::vec2 texel_offsets[25];

    int width = 0;
    int height = 0;
    
    glm::mat4 transformation = glm::mat4(1.0f);
    
    bool setTexelOffsets() {
        if(width <= 0 || height <= 0)
            return false;
        
        float w = 1 / (float)width;
        float h = 1 / (float)height;
        
        for(int y=0; y<5; y++) {
            for(int x=0; x<5; x++) {
                texel_offsets[y*5 + x] = glm::vec2(w*(x-2), h*(y-2) );
            }
        }
        
        return true;
    }
    
public:
    convolutionTypeEnum convolutionType;
    
    void transform(float x, float y, float scale = 1) {
        transformation = glm::mat4(1.0f);
        transformation = glm::translate(transformation, glm::vec3(x, y, 0) );
        transformation = glm::scale(transformation, glm::vec3(scale, scale, 1) );
    }

    
    bool create(int w, int h, GLuint shader) {
        width = w;
        height = h;
        setTexelOffsets();
        convolutionType = DEFAULT;
        
        shaderID = shader;
        glUseProgram(shaderID);
        
        //get uniform locations from shader
        transformationMatrixID = glGetUniformLocation(
            shaderID, "transformation"
        );
        convolutionMatrixID = glGetUniformLocation(
            shaderID, "convolutionMatrix"
        );
        texelOffsetID = glGetUniformLocation(
            shaderID, "texelOffsets"
        );
        
        //set texel offsets and initial convolution matrix
        glUniform2fv(texelOffsetID, 25, (GLfloat*)texel_offsets);
        glUniform1fv(convolutionMatrixID, 25, convolution_default);
        
        //array object
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);
        
        //vertices
        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(
            GL_ARRAY_BUFFER, 
            sizeof(rectangleVertices), 
            rectangleVertices, 
            GL_STATIC_DRAW
        );
        
        //UVs
        glGenBuffers(1, &uvBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
        glBufferData(
            GL_ARRAY_BUFFER, 
            sizeof(rectangleUVs),
            rectangleUVs,
            GL_STATIC_DRAW
        );
        
        //framebuffer 
        glGenFramebuffers(1, &frameBufferID);
        
        //texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(
            GL_TEXTURE_2D,      //target
            0,                  //level of detail (mipmap reduction)
            GL_R32F,            //internal format
            width,              //width
            height,             //height
            0,                  //border: must always be 0
            GL_RED,             //format of pixel data
            GL_FLOAT,           //type of the pixel data
            0                   //pointer to pixels: empty
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

        return true;
    }
    
    
    bool setFromPixels(GLfloat* pixels) {
        glUseProgram(shaderID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(
            GL_TEXTURE_2D,      //target
            0,                  //level of detail (mipmap reduction)
            GL_R32F,            //internal format
            width,              //width
            height,             //height
            0,                  //border: must always be 0
            GL_RED,             //format of pixel data
            GL_FLOAT,   //type of the pixel data
            pixels              //pointer to pixels
        );        
        return true;
    }
    
    
    bool setAsTestPattern() {
        if(width <= 0 || height <= 0)
            return false;

        //generate the pixel data to array
        GLfloat* pixels = new GLfloat[height * width];
        for(int y =0; y < height; y++) {
            for(int x=0; x < width; x++) {
                int i = y * width + x;
                float f_x = 2*(float)x / (float)width - 1;
                float f_y = 2*(float)y / (float)height - 1;
                float value = 1-(f_x*f_x + f_y * f_y);
                pixels[i] = 2*randomf()-1;
            }
        }
        setFromPixels(pixels);
        delete[] pixels;
        return true;
    }
    
    
    bool render(GLfloat level = 1) {
        glUseProgram(shaderID);
        
        convolutionMatrixID = glGetUniformLocation(
            shaderID, "convolutionMatrix"
        );
        convolutionLevelID = glGetUniformLocation(
            shaderID, "convolutionLevel"
        );
        
        switch(convolutionType) {
            case LAPLACIAN:
                glUniform1fv(convolutionMatrixID, 25, convolution_laplacian);
                break;
            case BLUR:
                glUniform1fv(convolutionMatrixID, 25, convolution_blur);
                break;
            default:
                glUniform1fv(convolutionMatrixID, 25, convolution_default);
        }
        
        glUniform1f(convolutionLevelID, level);
        
        glUniformMatrix4fv(
            transformationMatrixID,
            1, 
            GL_FALSE, 
            &transformation[0][0]
        );

        //set texel offsets 
        glUniform2fv(texelOffsetID, 25, (GLfloat*)texel_offsets);
                
        glBindVertexArray(vertexArrayID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        glEnableVertexAttribArray(0); //for vertices
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); //vertices: 3 floats
        
        glEnableVertexAttribArray(1); //for UVs
        glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); //UVs: 2 floats
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        return true;
    }
    
    bool setAsRenderTarget() {

        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureID, 0);
        GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, drawBuffers);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return false;
        glViewport(0, 0, width, height);
        
        return true;
    }
    
} images[4];


// create GLFW window, load shaders and do some setup
bool initialize() {
    if(!glfwInit() ) {
        std::cout << "couldn't initialize glfw!\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    window = glfwCreateWindow(640, 512, "OpenGl program", NULL, NULL);
    if(window == NULL) {
        std::cout << "couldn't create window!\n";
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        std::cout << "couldn't initialize GLEW\n";
        glfwTerminate();            
        return false;
    }
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    shaders[0] = loadShaders(
        "data/default.vertexShader", 
        "data/palette.fragmentShader"
    );
    shaders[1] = loadShaders(
        "data/default.vertexShader", 
        "data/convolution.fragmentShader"
    );
    shaders[2] = loadShaders(
        "data/default.vertexShader", 
        "data/reaction.fragmentShader"
    );    
    
    float n = 3;
    images[0].create(640*n, 512*n, shaders[0]); //output shader
    images[1].create(640*n, 512*n, shaders[1]); //convolution shader
    images[2].create(640*n, 512*n, shaders[1]); //convolution shader
    images[3].create(640*n, 512*n, shaders[2]); //reaction shader
    
    images[0].setAsTestPattern();
    images[1].setAsTestPattern();
    images[2].setAsTestPattern();
    
    images[1].convolutionType = BLUR;
    images[2].convolutionType = LAPLACIAN;
        
    return true;
}


// check if user wants to quit (ESC pressed or window closed)
bool keepRunning() {
    return (
        glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS 
        && glfwWindowShouldClose(window) == 0
    );  
}


// clear the canvas
void clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void render() {  

    //draw 2 on 1 using the laplacian
    images[1].setAsRenderTarget();
    images[2].render(1);
    
    //draw 1 on 2 using the blur
    images[2].setAsRenderTarget();
    images[1].render(0.85);
    
    //draw 1 on 0 to give colors to the values
    images[0].setAsRenderTarget();
    images[2].render();
    
    //draw 0 on screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 640, 512);
    images[0].render();

    glfwSwapBuffers(window);
    glfwPollEvents();
}
