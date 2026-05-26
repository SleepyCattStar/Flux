#pragma once

#include <iostream>
#include "imgui.h"
#include "fourier/audio_capture.h"
#include<vector>
#include "fourier/fft_engine.h"

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
        void graph_fft();
        void performance_analysis();

    private:
        audio mic;
        std::vector<float> displayData;   // the data which is used to plot those graphs
        std::vector<float> peakHistory;      // to plot the maxi graph.

        fft engine;  // instance of class fft to use the computation functions
        std::vector<float> fftMagnitudes;   
        bool isPaused = false;
        double autoResumeTime = 0.0;
        float globalGain = 1.0f;
        float globalNoiseGate = 0.002f;
        float dominantFrequency = 0.0f;


};