#pragma once
#include "graphics.h"

#include <array>
#include <iostream>

/* Convolution matrices are 5 x 5
 * represented as std::array<float, 25>
 * Matrices are normalized so that the sum of all elements is either 1 or -1 or 0
 * and all values are scaled between -1...1
 */


enum convolutionTypeEnum{
    BLUR,
    LAPLACIAN,
    NEIGHBOURHOOD,
    DEFAULT
};


/* internal matrices. Note: unnormalized! */


static const std::array<float, 25> convolution_laplacian = {
    1,  4,    6,   4,  1,
    4, 16,   24,  16,  4,
    6, 24, -220,  24,  6,
    4, 16,   24,  16,  4,
    1,  4,    6,   4,  1
};

/*
static const std::array<float, 25> convolution_laplacian = {
     1,  2,   2,  2,  1,
     2,  3,   4,  3,  2,
     2,  4, -56,  4,  2,
     2,  3,   4,  3,  2,
     1,  2,   2,  2,  1
};
*/

/*
static const std::array<float, 25> convolution_laplacian = {
     0,  0,  0,  0,  0,
     0,  1,  1,  1,  0,
     0,  1, -8,  1,  0,
     0,  1,  1,  1,  0,
     0,  0,  0,  0,  0
};
*/


static const std::array<float, 25> convolution_blur = {
     1,   4,   6,   4,   1,
     4,  16,  24,  16,   4,
     6,  24,  36,  24,   6,
     4,  16,  24,  16,   4,
     1,   4,   6,   4,   1
};


/*
static const std::array<float, 25> convolution_blur = {
     1, 1, 1, 1, 1,
     1, 1, 1, 1, 1,
     1, 1, 1, 1, 1,
     1, 1, 1, 1, 1,
     1, 1, 1, 1, 1
};
*/
static const std::array<float, 25> convolution_default = {
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  1,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0
};

/*
static const std::array<float, 25> neighbourhood = {
     1,  3,  3,  3,  1,
     3,  2,  1,  2,  3,
     3,  1,  0,  1,  3,
     3,  2,  1,  2,  3,
     1,  3,  3,  3,  1
};
*/
static const std::array<float, 25> neighbourhood = {
     0,  1,  1,  1,  0,
     1,  2,  3,  2,  1,
     1,  3, -5,  3,  1,
     1,  2,  3,  2,  1,
     0,  1,  1,  1,  0
};

class convolutionMatrix{
    std::array<float, 25> matrix;
public:
    
    void normalize() {
        
        float sum = 0;
        float min = 0;
        float max = 0;
        for(int i=0; i<25; i++) {
            sum += matrix[i];
            if(matrix[i] > max) max = matrix[i];
            if(matrix[i] < min) min = matrix[i];            
        }

        //if sum is 0, scale everything between -1 and 1
        if(sum == 0) {
            return;
            min *= -1;
            if(min > max) max = min;
            if(max == 0) return;
            for(int i = 0; i < 25; i++)
                matrix[i] /= max;      
            
        }
        
        //if sum is nonzero, scale everything so that it is 1
        else {
            for(int i=0; i<25; i++)
                matrix[i] /= sum;
        }

    }

    
    bool set(const std::array<float, 25>& src) { //set as array & normalize
        matrix = src;
        normalize();
    }
    
    
    bool set(convolutionTypeEnum type) { //set as one of the internal matrices
        switch(type) {
            case BLUR:
                set(convolution_blur);
                break;
            case LAPLACIAN:
                set(convolution_laplacian);
                break;
            case NEIGHBOURHOOD:
                set(neighbourhood);
                break;
            default:
                set(convolution_default);
        }
        return true;
    }
    

    convolutionMatrix(convolutionTypeEnum type = DEFAULT) {
        set(type);
    }
    
    
    const std::array<float, 25>& get() {
        return matrix;
    }
    
};
