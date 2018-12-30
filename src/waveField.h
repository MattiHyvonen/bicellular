#pragma once
#include "GL.h"
#include "utility.h"
#include <vector>

class wave{
public:
    glm::vec2 waveVector;
    float phase;
    float amplitude;
    wave(glm::vec2 v = glm::vec2(0,0), float A = 1, float phase_ = 0) 
        : waveVector(v), amplitude(A), phase(phase_) {}
        
    float getAt(glm::vec2 P); //get value of wave at position P
};


class waveSeries{
public:
    std::vector<wave> harmonics;
    
    float getAt(glm::vec2 P);//get sum of waves at position P
    
    //arguments: wave number x & y, and phase
    void addHarmonic(int n_x, int n_y, float phase); 
    
    float getAmplitudeSum();
    void normalize();
    void randomize(int n_harmonics = 5, int max_wave_number = 4);
    void randomChange(float amount = 0.01);
};
