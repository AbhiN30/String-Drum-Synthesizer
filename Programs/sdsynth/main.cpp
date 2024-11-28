// cmidi.cpp
 
#include <iostream>
#include <cstdlib>
#include <string.h>
#include "rtmidi/RtMidi.h"
 
void HandleMidiMessage( double deltatime, std::vector< unsigned char > *message, void *userData )
{
	unsigned int nBytes = message->size();
	for ( unsigned int i=0; i<nBytes; i++ )
		std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
	if ( nBytes > 0 )
		std::cout << "stamp = " << deltatime << std::endl;
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
	RtMidiIn* midi = connectToMidiDevice();
	if (!midi) {
		return -1;
	}

	std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
	char input;
	std::cin.get(input);

	// Clean up
	delete midi;

	return 0;
}