#include "Synth.h"

Synth::Synth(double _sample_rate) 
    : sample_rate{_sample_rate} {}
Synth::~Synth() {}

void Synth::processBuffer(short* buffer, const int buffer_size) {
    for(int sampleIdx = 0; sampleIdx < buffer_size; sampleIdx += 2) {
        const short sample_L = (sampleIdx % 128) * 100 - 5000;  
        const short sample_R = (sampleIdx % 256) * 100 - 5000;

        buffer[sampleIdx+0] = sample_L;
        buffer[sampleIdx+1] = sample_L;
    }
}