#include "graphics.h"

#include "lodepng.h"
#include "loadShaders.h"

float randomf() {
    float result = (float)rand() / RAND_MAX;
    return result;
}

GLFWwindow* window;
GLuint shaders[3] = {0, 0, 0};
image images[3];


bool initialize() {
    if(!glfwInit() ) {
        std::cout << "couldn't initialize glfw!\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    window = glfwCreateWindow(640*2, 512*2, "OpenGL program", NULL, NULL);
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
    
    float n = 0.5;
    images[0].create(640*n, 512*n, shaders[0]); //output shader
    images[1].create(640*n, 512*n, shaders[1]); //convolution shader
    images[2].create(640*n, 512*n, shaders[1]); //convolution shader
    images[3].create(640*n, 512*n, shaders[2]); //reaction shader
    
    //images[1].convolutionType = LAPLACIAN;
    //images[2].convolutionType = DEFAULT;
        
    images[0].setAsTestPattern();
    images[1].setAsTestPattern();
    images[2].setAsTestPattern();
    
    return true;
}


bool keepRunning() {
    return (
        glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS 
        && glfwWindowShouldClose(window) == 0
    );  
}


void clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void render() {  
    //draw 1 on 2
    images[2].setAsRenderTarget();
    images[1].render();

    //draw 2 on 1
    images[1].setAsRenderTarget();
    images[2].render();
/*
    //draw 3 on 2 using the reaction shader
    images[1].setAsRenderTarget();
    images[3].render();
*/
    //copy 1 on 0
    images[0].setAsRenderTarget();
    images[1].render();
    
    //draw 0 on screen using the 'palette' shader
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 640*2, 512*2);
    images[0].render();

    glfwSwapBuffers(window);
    glfwPollEvents();
}
