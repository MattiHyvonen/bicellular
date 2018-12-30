#pragma once
#include <vector>
#include <stdlib.h>

#include "utility.h"

class wave{
    float frequency;
    float phase;
    float amplitude;
public:
    wave(float f = 1, float ph = 0, float A=1) 
        : frequency(f), phase(ph), amplitude(A) 
        {}    
    //x is {-1...1}
    float getValueAt(float x) {
        return amplitude * sin(x*PI*frequency + phase);
    }
};


class waveset{
    const int MAX_WAVES = 20;
    const int MAX_F = 15;
    std::vector<wave> waves;
public:
    waveset() {
        waves.resize(rand()%(MAX_WAVES-2) + 2);
        float A = 1.0 / waves.size();
        for(int i=0; i<waves.size(); i++) {
            waves[i] = wave( 
                (int)(pow(randomf(),20)*(MAX_F-1))+ 1, 
                0, 
                A
            );
        }
    }
    
    //x is {-1...1}
    float getValueAt(float x) {
        float result = 0;
        for(int i=0; i<waves.size(); i++)
            result += waves[i].getValueAt(x);
        return result;
    }
};
