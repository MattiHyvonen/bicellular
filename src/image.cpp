#include "image.h"
#include "waveset.h"
#include <iostream>

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


GLuint getCurrentShader() {
    GLint result;
    glGetIntegerv(GL_CURRENT_PROGRAM, &result);
    return result;
}


bool texture::bindTexture(GLuint textureUnit) {
    if(textureUnit > 80)
        return false;
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
    return true;
}


bool texture::setFromPixels(GLfloat* pixels) {
    //bind to texture unit 0
    bindTexture(0);
    
    //set pixels and settings
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


bool texture::create(int w, int h) {
    width = w;
    height = h;
    
    //generate
    glGenTextures(1, &textureID);
    
    //set as empty image
    setFromPixels(0);

    //minify & magnify filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
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
    //transformation = glm::rotate(transformation, 0.005f, glm::vec3(0.0f, 0.0f, 1.0f));
}


bool image::create(int w, int h) {
    width = w;
    height = h;
    texture::create(w,h);
    
    setTexelOffsets();
    
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
    
    return true;
}


bool image::setAsTestPattern() {
    if(width <= 0 || height <= 0)
        return false;

    waveset w;
    
    //generate the pixel data to array
    GLfloat* pixels = new GLfloat[height * width];
    for(int y =0; y < height; y++) {
        for(int x=0; x < width; x++) {
            int i = y * width + x;
            float f_x = 2*(float)x / (float)width - 1;
            float f_y = 2*(float)y / (float)height - 1;
            float f = f_x * f_x + f_y * f_y;
            pixels[i] = w.getValueAt(f);            
        }
    }
    setFromPixels(pixels);
    delete[] pixels;
    return true;
}


bool image::render() {
    
    //set transformation
    GLuint transformationMatrixID = glGetUniformLocation(
        getCurrentShader(), "transformation"
    );
    glUniformMatrix4fv(
        transformationMatrixID,
        1, 
        GL_FALSE, 
        &transformation[0][0]
    );

    //set texel offsets
    GLuint texelOffsetID = glGetUniformLocation(
        getCurrentShader(), "texelOffsets"
    );
    glUniform2fv(texelOffsetID, 25, (GLfloat*)texel_offsets);


    glBindVertexArray(vertexArrayID);
    bindTexture(0);
    
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
    bindTexture(0);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureID, 0);
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;
    glViewport(0, 0, width, height);
    
    return true;
}   


bool colorMap::create(int w) {
    waveset ws;
    
    image::create(w, 1);
    GLfloat* pixels = new GLfloat[width];
    
    //set pixels from wave function
    float min = 0;
    float max = 0;
    for(int x=0; x < width; x++) {
        float f_x = 2*(float)x / (float)width - 1;
        pixels[x] = ws.getValueAt(f_x);
        if(pixels[x] < min) 
            min = pixels[x];
        if(pixels[x] > max)
            max = pixels[x];
    }
    
    if(min >= max)
        return false;
    
    float sum = 0;
    float min2 = 0;
    float max2 = 0;
    //normalize to {-1...1}
    for(int x=0; x < width; x++) {
        float scale = max - min;
        pixels[x] *= (1.0/scale);
        sum += pixels[x];
        if(pixels[x] < min2) 
            min2 = pixels[x];
        if(pixels[x] > max2)
            max2 = pixels[x];        
    }
    std::cout << "normalized palette\n sum = " << sum << "\n"
    <<"max = " << max2 << "\n"
    <<"min = " << min2 << "\n";
    setFromPixels(pixels);
    delete[] pixels;
    
    return true;
}

