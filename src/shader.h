#pragma once
#include "graphics.h"

#include <string>
#include <array>

class shader{
    GLuint id = 0;
        
public:
    GLuint getID();
    bool load(std::string vertFile, std::string fragFile);
    bool use();
    
    //send uniform data to the shader:
    bool setFilteringLevel(float filteredLevel, float dryLevel);
    bool setConvolutionMatrix(const std::array<float, 25>&);
};
