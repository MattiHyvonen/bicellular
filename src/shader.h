#pragma once
#include "GL.h"

#include <string>
#include <array>

#include "image.h"

class shader{
    GLuint id = 0;
        
public:
    GLuint getID();
    bool load(std::string vertFile, std::string fragFile);
    bool use();
    
    //send uniform data to the shader:
    bool setFilteringLevel(float filteredLevel, float dryLevel);
    bool setConvolutionMatrix(const std::array<float, 25>&);
    bool setMap(texture& mapTexture);
};
