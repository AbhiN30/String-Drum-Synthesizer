#include <iostream>
#include <vector>
#include <cstring>
#include "portmidi.h"

#define DEBUG(x) if (x != pmNoError) { std::cerr << Pm_GetErrorText(x) << std::endl; }

PmDeviceID findDevice() {
    const int num_devices = Pm_CountDevices();
    for (int i = 0; i < num_devices; ++i) {
        const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
        if (!strcmp(info->name, "MPK mini 3 MIDI 1"))
        {
            std::cout << "Name: " << info->name << std::endl;
            std::cout << "Input Available: " << info->input << std::endl;
            std::cout << "Output Available: " << info->output << std::endl;
            std::cout << "Is Opened: " << info->opened << std::endl;
            std::cout << std::endl;
            return i;
        }
    }
    return -1;
}

#define NO_MIDI_DEVICE -1

// enum ErrorTypes {
//     NO_MIDI_DEVICE = -1
// }

int main() {
    DEBUG(Pm_Initialize());

    PmDeviceID deviceID = findDevice();
    if (deviceID < 0) {
        std::cerr << "Could not find MIDI Device." << std::endl;
        return NO_MIDI_DEVICE;
    }

    const PmDeviceInfo* deviceInfo = Pm_GetDeviceInfo(deviceID);
    std::cout << deviceInfo->name << std::endl; // debug
    

    const int32_t latency = 0;
    PortMidiStream* midi_stream;
    DEBUG(Pm_OpenOutput(&midi_stream,
            deviceID,
            NULL,
            16, // how many messages in the queue
            NULL,
            NULL,
            latency));

    std::vector<PmEvent> midi_events(16);
    while (1) {
        const int num_events_read = Pm_Read(midi_stream, midi_events.data(), midi_events.size());
        if (num_events_read < 0) {
            DEBUG((PmError)num_events_read);
        }

        for (int i = 0; i < num_events_read; i++) {
            if (midi_events[i].message != 0) {
                std::cout << "Message: " << midi_events[i].message << " | Time: " << midi_events[i].timestamp << std::endl;
            }
        }
    }

    DEBUG(Pm_Close(midi_stream));

    DEBUG(Pm_Terminate());
    return 0;
}