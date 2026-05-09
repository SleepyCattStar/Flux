// TABS WAY TO DISPLAY MULTIPLE GRAPHS

void ui::graph_tabs() {
    ImGui::SetNextWindowPos(ImVec2(350, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    
    // I renamed the window to be more general since it holds everything now
    ImGui::Begin("SpectraDash Analysis Engine", NULL, ImGuiWindowFlags_MenuBar);

    static float gain = 1.0f;
    static float noiseGate = 0.002f;
    static bool isPaused= false;
    ImGui::Spacing();
    ImGui::SliderFloat("Global Gain", &gain, 0.0f, 10.0f, "x%.1f");
    ImGui::SliderFloat("Global Noise Gate", &noiseGate, 0.0f, 0.15f, "%.4f");
    ImGui::Separator();
    ImGui::Spacing();

    // Pull the hardware data once per frame for all tabs to use
    std::vector<float> audioData = mic.getAudio();
    static std::vector<float> displayData(2048, 0.0f);
    static std::vector<float> peakHistory(300, 0.0f);

    // Apply the global gate and gain (Only if not paused!)
    if (!isPaused && audioData.size() > 0) {
        if (displayData.size() != audioData.size()) displayData.resize(audioData.size());
        
        float currentPeak = 0.0f;

        for(size_t i = 0; i < audioData.size(); i++) {
            float sample = audioData[i];
            
            // 1. Check Noise Gate
            if (std::abs(sample) < noiseGate) sample = 0.0f; 
            
            // 2. Apply Gain
            displayData[i] = sample * gain; 

            // 3. Track the highest peak for the second tab
            if (std::abs(displayData[i]) > currentPeak) {
                currentPeak = std::abs(displayData[i]);
            }
        }

        // Shift the peak history array for the EKG effect
        peakHistory.erase(peakHistory.begin());
        peakHistory.push_back(currentPeak);
    }

    // ==========================================
    // THE TAB BAR SYSTEM
    // ==========================================
    if (ImGui::BeginTabBar("AnalysisTabs")) {
        
        // --- TAB 1: RAW TIME DOMAIN ---
        if (ImGui::BeginTabItem("Time Domain")) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Raw Microsecond Audio Buffer");
            
            ImGui::PlotLines("##Waveform", displayData.data(), displayData.size(), 0, 
                isPaused ? "FROZEN" : "Live Feed", -1.0f, 1.0f, ImVec2(-1, 200));
                
            ImGui::EndTabItem(); // ALWAYS close the tab!
        }

        // --- TAB 2: PEAK AMPLITUDE ---
        if (ImGui::BeginTabItem("Peak Amplitude")) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Rolling Volume Envelope");

            ImGui::PlotLines("##PeakWaveform", peakHistory.data(), peakHistory.size(), 0, 
                isPaused ? "FROZEN" : "Live Envelope", 0.0f, 1.0f, ImVec2(-1, 200));

            ImGui::EndTabItem();
        }

        // --- TAB 3: FAST FOURIER TRANSFORM (Placeholder) ---
        if (ImGui::BeginTabItem("Frequency Domain (FFT)")) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), ">>> FFT Engine Offline <<<");
            ImGui::Text("Implement Cooley-Tukey algorithm here next...");
            
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar(); // ALWAYS close the tab bar!
    }

    ImGui::End();
}
