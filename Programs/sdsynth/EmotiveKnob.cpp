#include "EmotiveKnob.h"

EmotiveKnob::EmotiveKnob(const int _id) 
    : id{_id} {
}

EmotiveKnob::~EmotiveKnob() {}

unsigned char EmotiveKnob::readValue() {
    // gpio select using id
    // read bits
    return 0;
}
