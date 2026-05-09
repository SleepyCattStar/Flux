#define MINIAUDIO_IMPLEMENTATION
#include<iostream>
#include "fourier/audio_capture.h"

// constructor to initialise the buffer of size 2048
audio::audio() {
    ringBuffer.resize(2048, 0.0f);  
}

void audio::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 framecount){
    // this is called 44k times a second cuz microphone is usually at a very high frequency
    audio* capture = (audio*)pDevice->pUserData; 

    if(!capture || pInput == nullptr) {
        return;             // NOTHING captured
    }

    const float* input = (const float*)pInput; 

    std::lock_guard<std::mutex> lock(capture->buffer); // Lock the buffer , if UI tries to read it , it has to wait

    for (ma_uint32 i = 0; i < framecount; i++) {
        capture->ringBuffer[capture->write] = input[i];
        
        capture->write = (capture->write + 1) % capture->ringBuffer.size();
    }
}

bool audio::start() {
    if(isInitialised) {
        return true;
    }

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format   = ma_format_f32; // We want floating point math for Fourier transforms
    deviceConfig.capture.channels = 1;             // 1 channel (Mono)
    deviceConfig.sampleRate       = 44100;         // CD Quality
    deviceConfig.dataCallback     = data_callback;  // Tell Linux to fire our function
    deviceConfig.pUserData        = this;          // Pass our class instance

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        std::cerr << "[FATAL] Failed to initialize audio capture device.\n";
        return false;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cerr << "[FATAL] Failed to start microphone \n";
        ma_device_uninit(&device);
        return false;
    }


    isInitialised = true;

    std::cout << "[SYSTEM] Audio capture started \n";
    return true;
}

void audio::stop() {
    if(!isInitialised) {
        return;
    }

    // ma_device_stop(&device); 
    ma_device_uninit(&device); 
    isInitialised = false;

    std::cout << "[SYSTEM] Audio capture stopped \n";
    }


// void audio::stop() {
//     std::cout << "\n--- STOP FUNCTION TRIGGERED ---\n";
//     std::cout << "State before stopping: " << isInitialised << "\n";

//     if(!isInitialised) {
//         std::cout << "ABORT: System thinks it's already off!\n";
//         return;
//     }

//     ma_device_uninit(&device); 
//     isInitialised = false;

//     std::cout << "[SYSTEM] Audio capture stopped.\n";
//     std::cout << "State after stopping: " << isInitialised << "\n-------------------------------\n";
// }


//         std::vector<float> getAudio();


std::vector<float> audio::getAudio() {
    std::lock_guard<std::mutex> lock(buffer); // Lock the buffer while we copy it

    return ringBuffer;
}