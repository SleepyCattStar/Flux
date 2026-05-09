#pragma once

#include <iostream>
#include "imgui.h"
#include "fourier/audio_capture.h"
#include<vector>

class ui {
    public:
        void render(); 
        void changeTheme();
        void audiocapture();
        void graph_tabs();
        void graph();
        void graph_time_domain();
        void graph_peak();
        void process_audio_data();


    private:
        audio mic;
        std::vector<float> displayData;
        std::vector<float> peakHistory;
        
        bool isPaused = false;
        double autoResumeTime = 0.0;
        float globalGain = 1.0f;
        float globalNoiseGate = 0.002f;


};