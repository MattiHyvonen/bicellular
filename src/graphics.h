#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
