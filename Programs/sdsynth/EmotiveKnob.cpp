#include "EmotiveKnob.h"

EmotiveKnob::EmotiveKnob(const int _id) 
    : id{_id} {
}

EmotiveKnob::~EmotiveKnob() {}

short EmotiveKnob::readValue() {
    // gpio select using id
    // read bits
    return 0;
}
