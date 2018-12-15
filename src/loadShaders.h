#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>

std::string fileToString(const char* path) {
    std::string result;
    std::ifstream file(path, std::ios::in);
    if(file.is_open() ) {
        std::stringstream ss;
        ss << file.rdbuf();
        result = ss.str();
        file.close();
    }
    else{
        std::cout << "couldn't open file " << path << "\n";
        result = "";
    }
    return result;
}

GLuint loadShaders(const char* vertexShaderPath, const char* fragmentShaderPath) {
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    //read shader source from files
    std::string vertexShaderSource = fileToString(vertexShaderPath);
    std::string fragmentShaderSource = fileToString(fragmentShaderPath);
    
    //compile shaders
    GLint result = GL_FALSE;
    int infoLogLength;
    
    const char* v_cstr = vertexShaderSource.c_str();
    const char* f_cstr = fragmentShaderSource.c_str();
    glShaderSource(vertexShaderID, 1, &v_cstr, NULL);    
    glShaderSource(fragmentShaderID, 1, &f_cstr, NULL);
    
    glCompileShader(vertexShaderID);
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0) {
        std::vector<char> vertexShaderErrorMessage(infoLogLength+1);
        glGetShaderInfoLog(
            vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]
        );
        printf("%s\n", &vertexShaderErrorMessage[0]);
    }

    glCompileShader(fragmentShaderID);
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::vector<char> fragmentShaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(
            fragmentShaderID,
            infoLogLength,
            NULL,
            &fragmentShaderErrorMessage[0]
        );
		printf("%s\n", &fragmentShaderErrorMessage[0]);
	}
    
    //link shader program
    GLuint shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShaderID);
    glAttachShader(shaderProgramID, fragmentShaderID);
    glLinkProgram(shaderProgramID);
    
    //free resources
    glDetachShader(shaderProgramID, vertexShaderID);
    glDetachShader(shaderProgramID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    
    return shaderProgramID;
}
