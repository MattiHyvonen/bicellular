#pragma once

#define PI 3.14159265
#define TWO_PI 6.2831853
#include <stdlib.h>
#include <time.h>
#include <string>

float randomf();
float randomf(float min, float max);

std::string generateFrameFilename(std::string path, 
                                  std::string extension,
                                  int max_frames = 10000
                                 );
