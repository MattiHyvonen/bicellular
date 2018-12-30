#pragma once
#include "GL.h"

class texture{
protected:
    GLuint textureID = 0;
    int width = 0;
    int height = 0;
public:
    bool bindTexture(GLuint textureUnit = 0);
    bool setFromPixels(GLfloat* pixels);
    bool create(int w, int h);
};


class image : public texture{
protected:
    //IDs for buffer objects, the image texture and a shader
    GLuint vertexArrayID = 0;
    GLuint vertexBufferID = 0;
    GLuint uvBufferID = 0;
    GLuint frameBufferID = 0;
    
    //texel offsets are relative vectors to neighbouring texels
    glm::vec2 texel_offsets[25];

    //transformation matrix for transforming the image
    glm::mat4 transformation = glm::mat4(1.0f);
    
    //calculate texel offsets from the image size and the texture size
    bool setTexelOffsets();
    
public:
    
    //set image transformation
    void transform(float x, float y, float scale = 1);

    bool create(int w, int h);
    bool setAsTestPattern();
    bool render();
    bool setAsRenderTarget();
    
};


class colorMap : public image{
public:
    bool create(int w = 1024);
    bool normalize();
};
