#include "waveField.h"

float wave::getAt(glm::vec2 P) {
    return amplitude * sin( (glm::dot(P, waveVector) + phase) * TWO_PI );
}

void waveSeries::addHarmonic(int n_x, int n_y, float phase) {
    harmonics.push_back(wave(glm::vec2(n_x, n_y), phase) );
}

float waveSeries::getAmplitudeSum() {
    float sum = 0;
    for(int i=0; i<harmonics.size(); i++) {
        sum += harmonics[i].amplitude;
    }
    return sum;
}


float waveSeries::getAt(glm::vec2 P) {
    if(harmonics.empty() )
        return 0;
    
    float sum = 0;
    for(int i=0; i<harmonics.size(); i++) {
        sum += harmonics[i].getAt(P);
    }
    return sum;
}

void waveSeries::randomize(int n_harmonics, int max_wave_number) {
    harmonics.clear();
    for(int i=0; i<n_harmonics; i++)
        addHarmonic( rand()%max_wave_number,
                     rand()%max_wave_number,
                     randomf()
                   );
}

void waveSeries::normalize() {
    float sum = getAmplitudeSum();
    if(sum == 0) return;
    
    float factor = 1/sum;
    for(int i=0; i<harmonics.size(); i++) {
        harmonics[i].amplitude *= factor;
    }
}


void waveSeries::randomChange(float amount) {
    for(int i=0; i<harmonics.size(); i++) {
        harmonics[i].phase += randomf(-amount, amount);
        harmonics[i].amplitude += randomf(-amount, amount);
    }
    
    this->normalize();
}
