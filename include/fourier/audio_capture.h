#pragma once
 // Documentation: https://miniaud.io/docs  
#include "miniaudio.h"
#include <vector>
#include <mutex>

class audio{

    public:
        audio();   //constructor to initialise the buffer of size 2048

        bool start();
        void stop();
        bool isInitialised = false;
        bool isActive() const { return isInitialised; }
        std::vector<float> getAudio();

    private:
        ma_device device;
        static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);    
        std::vector<float> ringBuffer;
        std::mutex buffer; 
        int write =0;

};








// LOW LEVEL API ( FROM THE DOC ) 


// void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
// {
//     // In playback mode copy data to pOutput. In capture mode read data from pInput. In full-duplex mode, both
//     // pOutput and pInput will be valid and you can move data from pInput into pOutput. Never process more than
//     // frameCount frames.
// }

// int main()
// {
//     ma_device_config config = ma_device_config_init(ma_device_type_playback);
//     config.playback.format   = ma_format_f32;   // Set to ma_format_unknown to use the device's native format.
//     config.playback.channels = 2;               // Set to 0 to use the device's native channel count.
//     config.sampleRate        = 48000;           // Set to 0 to use the device's native sample rate.
//     config.dataCallback      = data_callback;   // This function will be called when miniaudio needs more data.
//     config.pUserData         = pMyCustomData;   // Can be accessed from the device object (device.pUserData).

//     ma_device device;
//     if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
//         return -1;  // Failed to initialize the device.
//     }

//     ma_device_start(&device);     // The device is sleeping by default so you'll need to start it manually.

//     // Do something here. Probably your program's main loop.

//     ma_device_uninit(&device);
//     return 0;
// }