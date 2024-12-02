#pragma once

#include <alsa/asoundlib.h>
#include <cstdlib>
#include <pthread.h>
#include <stdio.h>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include "Synth.h"

class AudioDevice {
public:
    AudioDevice();
    ~AudioDevice();

    int initialize();
    void play();
    void stop();

    void processBuffer();

private:
    void startAudioThread();
    void startVisualizationThread();
    void detectHDMIAndRender();
    void renderWaveform();

private:
    snd_pcm_t *pcm_handle;
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
    snd_pcm_hw_params_t *hwparams;

    std::atomic<bool> isPlaying;        // Controls audio playback
    std::atomic<bool> isHDMIConnected; // HDMI connection state
    std::thread visualizationThread;   // Thread for visualization
    std::mutex dataMutex;              // Protects access to `dataPtr`

    const unsigned int target_sample_rate = 48000; // Sample rate
    unsigned int exact_sample_rate;               // Adjusted sample rate
    int dir;  // Rate direction: exact_rate == rate --> dir = 0
              //                exact_rate < rate  --> dir = -1
              //                exact_rate > rate  --> dir = 1
    const int periods = 2;               // Number of periods
    snd_pcm_uframes_t period_size = 1024; // Period size in frames
    const int buffer_size = period_size >> 2; // Buffer size in frames

    std::vector<short> data; // Audio data buffer
    short* dataPtr;          // Pointer to the audio data buffer

    Synth synth; // Synth object for generating audio data
};
