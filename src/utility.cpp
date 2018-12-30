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
