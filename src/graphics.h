#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string.h>

#include "image.h"

// create GLFW window, load shaders and do some setup
bool initialize();

// check if user wants to quit (ESC pressed or window closed)
bool keepRunning();

// clear the canvas
void clear();

void render();
