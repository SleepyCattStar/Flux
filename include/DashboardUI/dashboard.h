#pragma once

#include <iostream>
#include "imgui.h"
#include "fourier/audio_capture.h"

class ui {
    public:
        void render(); 
        void changeTheme();
        void graph();
        void audiocapture();

    private:
        audio mic;

};