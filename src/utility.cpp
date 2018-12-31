#include "utility.h"

float randomf() {
    float result = (float)rand() / RAND_MAX;
    return result;
}

float randomf(float min, float max) {
    float scale = max - min;
    float result = randomf() * scale + min;
    return result;
}

std::string generateFrameFilename(std::string path, 
                                  std::string extension, 
                                  int max_frames
                                 ) 
{
    static int frame_n = 0;
    if(frame_n > max_frames) 
        return "";
    std::string result = path + std::to_string(frame_n) + extension;
    frame_n++;
    return result;
}
