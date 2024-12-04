#include <iostream>
#include <cstdlib>
#include <string.h>
#include <math.h>
#include <wiringPi.h>
#include <unistd.h>
#include "rtmidi/RtMidi.h"
#include "AudioDevice.h"
#include "EmotiveKnob.h"

// AudioDevice audio_device;

enum MidiEvents {
	NOTE_ON = 144,
	NOTE_OFF = 128,
};

void HandleMidiMessage(double deltatime, std::vector< unsigned char > *message, void *userData) {
	unsigned int nBytes = message->size();
	switch ((int)message->at(0)) {
		case MidiEvents::NOTE_ON:
			const double newPitch = 440.0 * pow(2.0, static_cast<double>((int)message->at(1) - 69) / 12.0);
			// audio_device.getSynth().playNote(newPitch);
			break;
		// case MidiEvents::NOTE_OFF:
		// 	break;
	}
}

int getMidiPort(RtMidiIn *midi) {
	unsigned int nPorts = midi->getPortCount();
	if ( nPorts == 0 ) {
		std::cerr << "No ports available!\n";
		return -1;
	}
	for (int port = 0; port < nPorts; port++) {
		std::string portName = midi->getPortName(port);
		if (portName.find("MPK") != std::string::npos) {
			std::cout << "Port " << port << ": " << portName << std::endl;
			return port;
		}
	}
	return -1;
}

RtMidiIn* connectToMidiDevice() {
	RtMidiIn *midi = new RtMidiIn();

	const int midiPort = getMidiPort(midi);
	if (midiPort < 0) {
		delete midi;
		return NULL;
	}
	midi->openPort( midiPort );

	// This should be done immediately after opening the port to avoid 
	// having incoming messages written to the queue.
	midi->setCallback( &HandleMidiMessage );

	// Do we ignore sysex, timing, or active sensing messages.
	// midi->ignoreTypes( false, true, false );

	return midi;
}
 
int main()
{
	// if (audio_device.initiallize() < 0) {
    //     return -1;
    // }

	// RtMidiIn* midi = connectToMidiDevice();
	// while (!midi) {
	// 	midi = connectToMidiDevice();
	// 	printf("Waiting for MIDI device...\n");
	// 	usleep(250000);
	// }

	// printf("Reading MIDI input.\n");

    // audio_device.play();

	EmotiveKnob::setup();
	EmotiveKnob testKnob0(0);
	EmotiveKnob testKnob1(1);
	EmotiveKnob testKnob4(4);

	while (1) {
		const short val0 = testKnob0.readValue();
		const short val1 = testKnob1.readValue();
		const short val4 = testKnob4.readValue();
		printf("KNOB 0: %d, KNOB 1: %d, KNOB 4: %d\n", val0, val1, val4);
		usleep(100000);
		// printf("%d\n", testKnob0.readValue());
		// for (int id = 0; id < 8; id++) {
		// 	digitalWrite(KNOB_SELECT_C, (int)((id & 0b001)));
		// 	digitalWrite(KNOB_SELECT_B, (int)((id & 0b010) > 0));
		// 	digitalWrite(KNOB_SELECT_A, (int)((id & 0b100) > 0));
		// }
	}

	char input;
	std::cin.get(input);

	EmotiveKnob::cleanup();



	// // Clean up
	// delete midi;

	return 0;
}