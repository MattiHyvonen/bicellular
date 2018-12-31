#pragma once

#include "GL.h"
#include <iostream>
#include <string.h>

#include "shader.h"
#include "image.h"

GLuint getCurrentShader();

// create GLFW window, load shaders and do some setup
bool initialize();

// check if user wants to quit (ESC pressed or window closed)
bool running();

// clear the canvas
void clear();

// set the whole screen as the render target
bool resetRenderTarget();

void render();

void handleEvents();

bool saveFrame(std::string path, 
               int startFrame = 0, 
               int frames = 1500, 
               int frameSkip = 0
              );
