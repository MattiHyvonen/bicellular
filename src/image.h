#pragma once

#include "graphics.h"

enum convolutionTypeEnum{
    BLUR,
    LAPLACIAN,
    DEFAULT
};


class image{
    int width = 0;
    int height = 0;

    //IDs for buffer objects, the image texture and a shader
    GLuint vertexArrayID = 0;
    GLuint vertexBufferID = 0;
    GLuint uvBufferID = 0;
    GLuint textureID = 0;
    GLuint frameBufferID = 0;
    GLuint shaderID = 0;
    
    //texel offsets are relative vectors to neighbouring texels
    glm::vec2 texel_offsets[25];

    //transformation matrix for transforming the image
    glm::mat4 transformation = glm::mat4(1.0f);
    
    //calculate texel offsets from the image size and the texture size
    bool setTexelOffsets();
    
public:
    //the type of convolution filter to use
    convolutionTypeEnum convolutionType = DEFAULT;
    
    //set image transformation
    void transform(float x, float y, float scale = 1);
    
    bool create(int w, int h, GLuint shader);
    bool setFromPixels(GLfloat* pixels);
    bool setAsTestPattern();
    bool render(GLfloat convolutionLevel = 1, GLfloat dryLevel = 0);
    bool setAsRenderTarget();
    
};

