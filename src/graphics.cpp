#include "graphics.h"
#include "convolutionMatrix.h"
#include "utility.h"


GLFWwindow* window;
shader shaders[3];
image images[3];
colorMap palette;
convolutionMatrix blurMatrix(BLUR);
convolutionMatrix laplacianMatrix(LAPLACIAN);
convolutionMatrix neighbourhoodMatrix(NEIGHBOURHOOD);

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
    
    window = glfwCreateWindow(1920, 1080, "OpenGL program", NULL, NULL);
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

    //1: reaction shader
    shaders[1].load(
        "data/default.vertexShader", 
        "data/reaction.fragmentShader"
    );
    shaders[1].use();
    shaders[1].setFilteringLevel(1, 0);
    shaders[1].setConvolutionMatrix(blurMatrix.get() );
    palette.create(512, 512);
    shaders[1].setMap(palette);    
    
    //2: convolution shader (diffusion)
    shaders[2].load(
        "data/default.vertexShader",
        "data/convolution.fragmentShader"
    );
    
    float n = 2;
    
    images[0].create(1920*n, 1080*n);
    images[1].create(1920*n, 1080*n);
    
    images[2].create(1920, 1080, 3); //to render out

    images[0].setAsTestPattern();

    shaders[2].use();
    shaders[2].setFilteringLevel(0, 1);
    shaders[2].setConvolutionMatrix(laplacianMatrix.get() );
    images[1].setAsRenderTarget();
    images[0].transform(0, 0, 0.1);
    images[0].render();
    
    resetRenderTarget();
    
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
    glViewport(0, 0, 1920, 1080);
    return true;
}


void render() {
    
    //render 1 on 0, reaction
    shaders[1].use();
    shaders[1].setFilteringLevel(0.5, 1);
    shaders[1].setConvolutionMatrix(neighbourhoodMatrix.get() );
    images[0].setAsRenderTarget();
    images[1].transform(0, 0, 1);
    images[1].render();
    
    //render 0 on 1, diffusion
    shaders[2].use();
    shaders[2].setFilteringLevel(0.007, 1);
    shaders[2].setConvolutionMatrix(laplacianMatrix.get() );
    images[1].setAsRenderTarget();
    images[0].transform(0, 0, 1.0011);
    images[0].render();
    
    //render 0 on screen using the palette shader
    shaders[0].use();
    resetRenderTarget();
    images[1].transform(0, 0, 1);
    images[1].render();
    
    glfwSwapBuffers(window);
    clear();
}


//NOTE: accesses images[2] with no checking!
//return false if reached end or there was error, true otherwise
bool saveFrame(std::string path, int startFrame, int frames, int frameSkip) {
    static int frameCount = -1;
    frameCount++;
    
    //not started yet
    if(frameCount < startFrame)
        return true;
    
    //already finished
    if(frameCount > frames + startFrame)
        return false;
    
    //check frame skipping, render and save frame
    if( frameSkip == 0 || (frameCount-startFrame)%frameSkip == 0) {
        
        //render 0 on 2 using the palette shader
        shaders[0].use();
        images[2].setAsRenderTarget();
        images[1].transform(0, 0, 1);
        images[1].render();
        
        std::string filename = generateFrameFilename(path + "frame", ".png", frames);
        if(filename != "")
            images[2].saveAsPNG(filename);
        else
            return false;
    }

    return true;
}

