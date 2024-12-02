#include "AudioDevice.h"
#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <chrono>
#include <mutex>

#define AUDIO_HARDWARE_NAME "hw:CARD=Headphones"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400

std::mutex dataMutex; // Protect access to `dataPtr`

inline int clamp(int value, int min, int max) {
    return (value < min) ? min : (value > max) ? max : value;
}

AudioDevice::AudioDevice()
    : isPlaying{false}
    , isHDMIConnected{false}
    , data(period_size << 1) 
    , synth(target_sample_rate) {
    dataPtr = &data.data()[0];
    }

AudioDevice::~AudioDevice() {
    stop();
}

int AudioDevice::initialize() {
    snd_pcm_hw_params_alloca(&hwparams);

    if (snd_pcm_open(&pcm_handle, AUDIO_HARDWARE_NAME, stream, 0) < 0) {
        fprintf(stderr, "Error opening PCM device %s\n", AUDIO_HARDWARE_NAME);
        return -1;
    }

    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
        fprintf(stderr, "Cannot configure this PCM device.\n");
        return -1;
    }

    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        fprintf(stderr, "Error setting access.\n");
        return -1;
    }

    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
        fprintf(stderr, "Error setting format.\n");
        return -1;
    }

    exact_sample_rate = target_sample_rate;
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_sample_rate, 0) < 0) {
        fprintf(stderr, "Error setting rate.\n");
        return -1;
    }

    if (target_sample_rate != exact_sample_rate) {
        fprintf(stderr, "The rate %d Hz is not supported by your hardware. Using %d Hz instead.\n", target_sample_rate, exact_sample_rate);
    }

    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
        fprintf(stderr, "Error setting channels.\n");
        return -1;
    }

    if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0) < 0) {
        fprintf(stderr, "Error setting periods.\n");
        return -1;
    }

    if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, (period_size * periods) >> 2) < 0) {
        fprintf(stderr, "Error setting buffer size.\n");
        return -1;
    }

    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
        fprintf(stderr, "Error setting HW params.\n");
        return -1;
    }

    return 0;
}

void AudioDevice::play() {
    isPlaying = true;
    isHDMIConnected = false;
    startVisualizationThread();
    startAudioThread();
}

void AudioDevice::stop() {
    isPlaying = false;
    snd_pcm_drain(pcm_handle);
}

void AudioDevice::processBuffer() {
    std::lock_guard<std::mutex> lock(dataMutex);
    // Simulate audio synthesis
    std::fill(data.begin(), data.end(), rand() % 32767);
}

void AudioDevice::startAudioThread() {
    int pcmreturn;
    bool half_cycle = false;
    dataPtr = &data.data()[0];

    while (isPlaying) {
        processBuffer();
        while ((pcmreturn = snd_pcm_writei(pcm_handle, dataPtr, buffer_size)) < 0) {
            snd_pcm_prepare(pcm_handle);
            fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
        }
        dataPtr = half_cycle ? &data.data()[0] : &data.data()[period_size];
        half_cycle = !half_cycle;
    }
}

void AudioDevice::startVisualizationThread() {
    visualizationThread = std::thread(&AudioDevice::detectHDMIAndRender, this);
    visualizationThread.detach();
}

void AudioDevice::detectHDMIAndRender() {
    while (!isPlaying) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    while (true) {
        FILE* pipe = popen("vcgencmd display_power | grep 'display_power=1'", "r");
        char buffer[128];
        if (pipe) {
            fgets(buffer, sizeof(buffer), pipe);
            pclose(pipe);
            isHDMIConnected = (strstr(buffer, "display_power=1") != nullptr);
        }

        if (isHDMIConnected) {
            renderWaveform();
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void AudioDevice::renderWaveform() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize: %s\n", SDL_GetError());
        return;
    }

    SDL_Window* window = SDL_CreateWindow("Waveform Visualization",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "Window could not be created: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Used to scroll the waveform horizontally
    int offset = 0;

    while (isHDMIConnected && isPlaying) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear screen to black
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Set waveform color (green)

        {
            std::lock_guard<std::mutex> lock(dataMutex);

            // Loop through audio data and draw the waveform
            for (size_t i = 0; i < period_size - 1; ++i) {
                // Calculate x and y coordinates, adjusted by offset for scrolling
                int x1 = offset + i * (WINDOW_WIDTH / period_size);
                int y1 = ((dataPtr[i] / 32768.0) * (WINDOW_HEIGHT / 2)) + (WINDOW_HEIGHT / 2);
                int x2 = offset + (i + 1) * (WINDOW_WIDTH / period_size);
                int y2 = ((dataPtr[i + 1] / 32768.0) * (WINDOW_HEIGHT / 2)) + (WINDOW_HEIGHT / 2);

                // Ensure coordinates are within bounds
                x1 = (x1 < 0) ? 0 : (x1 >= WINDOW_WIDTH) ? WINDOW_WIDTH - 1 : x1;
                y1 = (y1 < 0) ? 0 : (y1 >= WINDOW_HEIGHT) ? WINDOW_HEIGHT - 1 : y1;
                x2 = (x2 < 0) ? 0 : (x2 >= WINDOW_WIDTH) ? WINDOW_WIDTH - 1 : x2;
                y2 = (y2 < 0) ? 0 : (y2 >= WINDOW_HEIGHT) ? WINDOW_HEIGHT - 1 : y2;

                // Draw the line between the points
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            }
        }

        // Update the screen with the new drawing
        SDL_RenderPresent(renderer);

        // Scroll the waveform by shifting the offset
        offset -= 1; // Move the drawing to the left
        if (offset <= -WINDOW_WIDTH) {
            offset = 0; // Reset when fully scrolled
        }

        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


