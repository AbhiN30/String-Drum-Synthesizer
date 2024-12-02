#pragma once

#include <wiringPi.h>

class EmotiveKnob {
public:
    EmotiveKnob(const int _id);
    ~EmotiveKnob();

    unsigned char readValue();

private:
    int id;
};