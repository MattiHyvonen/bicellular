#include "graphics.h"
#include "convolutionMatrix.h"

float randomf() {
    float result = (float)rand() / RAND_MAX;
    return result;
}

GLFWwindow* window;
shader shaders[3];
image images[4];
convolutionMatrix blurMatrix(BLUR);
convolutionMatrix laplacianMatrix(LAPLACIAN);

bool run = true;
bool changes = true;

bool running() {
    return run;
}

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
    
    glfwSetTime(1);
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    
    //0: palette shader
    shaders[0].load(
        "data/default.vertexShader", 
        "data/palette.fragmentShader"
    );

    //1: convolution shader
    shaders[1].load(
        "data/default.vertexShader", 
        "data/convolution.fragmentShader"
    );
    shaders[1].use();
    shaders[1].setFilteringLevel(1, 0);
    shaders[1].setConvolutionMatrix(blurMatrix.get() );
    
    //2: default (copy) shader
    shaders[2].load(
        "data/default.vertexShader",
        "data/default.fragmentShader"
    );
    
    float n = 0.5;
    
    images[0].create(640*n, 512*n);
    images[1].create(640*n, 512*n);
    
    images[1].setAsTestPattern();
    
    return true;
}


void handleEvents() {
    glfwPollEvents();
    
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS 
            || glfwWindowShouldClose(window) != 0)
        run = false;
}


void clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


bool resetRenderTarget() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 640*2, 512*2);
    return true;
}


void render() {
    /*
    if(glfwGetTime() < 1.0 || run == false)
        return;
    
    glfwSetTime(0);
    */
    
    //diffusion:
    //render 1 on 0 using the laplacian filter
    shaders[1].use();
    shaders[1].setConvolutionMatrix(laplacianMatrix.get() );
    shaders[1].setFilteringLevel(0.0065, 1);
    images[0].setAsRenderTarget();
    images[1].render();
    
    //render 0 on 1 using the blur filter
    shaders[1].setConvolutionMatrix(blurMatrix.get() );
    shaders[1].setFilteringLevel(0, 1);
    images[1].setAsRenderTarget();
    images[0].render();
    
    //render 1 on screen using the palette shader
    shaders[0].use();
    resetRenderTarget();
    images[0].render();

    glfwSwapBuffers(window);
}
