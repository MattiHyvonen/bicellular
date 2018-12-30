#include "shader.h"
#include "loadShaders.h"

GLuint shader::getID() {
    return id;
}

bool shader::load(std::string vertFile, std::string fragFile) {
    id = loadShaders(vertFile.c_str(), fragFile.c_str());
}

bool shader::use() {
    glUseProgram(id);
}


bool shader::setFilteringLevel(float filteredLevel, float dryLevel) {
    GLuint convolutionLevelID = glGetUniformLocation(
        id, "convolutionLevel"
    );
    glUniform1f(convolutionLevelID, filteredLevel);
    
    GLuint dryLevelID = glGetUniformLocation(
        id, "dryLevel"
    );
    glUniform1f(dryLevelID, dryLevel);
    
    return true;
}


bool shader::setConvolutionMatrix(const std::array<float, 25>& convmat) {
    GLuint convolutionMatrixID = glGetUniformLocation(
        id, "convolutionMatrix"
    );
    glUniform1fv(convolutionMatrixID, 25, &(convmat[0]) );
    
    return true;
}
