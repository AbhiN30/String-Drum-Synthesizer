#pragma once

class Synth {
public:
    Synth(double _sample_rate);
    ~Synth();
    void processBuffer(short* buffer, const int buffer_size);
private:
    double sample_rate;
};