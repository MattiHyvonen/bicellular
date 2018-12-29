#include "image.h"


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



bool image::setFromPixels(GLfloat* pixels) {
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
        GL_FLOAT,           //type of the pixel data
        pixels              //pointer to pixels
    );        
    return true;
}
    
    
    
bool image::setTexelOffsets() {
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


void image::transform(float x, float y, float scale) {
    transformation = glm::mat4(1.0f);
    transformation = glm::translate(transformation, glm::vec3(x, y, 0) );
    transformation = glm::scale(transformation, glm::vec3(scale, scale, 1) );
}


bool image::create(int w, int h, GLuint shader) {
    width = w;
    height = h;
    setTexelOffsets();
    
    shaderID = shader;
    glUseProgram(shaderID);
    
    //get uniform locations from shader

    GLuint convolutionMatrixID = glGetUniformLocation(
        shaderID, "convolutionMatrix"
    );
    GLuint texelOffsetID = glGetUniformLocation(
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


bool image::setAsTestPattern() {
    if(width <= 0 || height <= 0)
        return false;

    //generate the pixel data to array
    GLfloat* pixels = new GLfloat[height * width];
    for(int y =0; y < height; y++) {
        for(int x=0; x < width; x++) {
            int i = y * width + x;
            float f_x = 2*(float)x / (float)width - 1;
            float f_y = 2*(float)y / (float)height - 1;
            float value = (f_x*f_x + f_y * f_y);
            if(value > 0.01) pixels[i] = -1;
            else pixels[i] = 1;
        }
    }
    setFromPixels(pixels);
    delete[] pixels;
    return true;
}


bool image::render(GLfloat convolutionLevel, GLfloat dryLevel) {
    glUseProgram(shaderID);
    
    GLuint convolutionMatrixID = glGetUniformLocation(
        shaderID, "convolutionMatrix"
    );
    GLuint convolutionLevelID = glGetUniformLocation(
        shaderID, "convolutionLevel"
    );
    GLuint dryLevelID = glGetUniformLocation(
        shaderID, "dryLevel"
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
    
    glUniform1f(convolutionLevelID, convolutionLevel);
    glUniform1f(dryLevelID, dryLevel);
    
    //get uniform locations from shader
    GLuint transformationMatrixID = glGetUniformLocation(
        shaderID, "transformation"
    );
    
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


bool image::setAsRenderTarget() {
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
