// Docs->    ReadMe of https://github.com/ocornut/imgui

#include "DashboardUI/dashboard.h"         // audio mic, is declared here/
#include "fourier/audio_capture.h"     // for the class audio, class instance mic, and the function start(), stop()
#include <iostream>
#include<fstream>   // to make export data to csv file 
#include<string>    
#include <filesystem>  
#include<chrono>

void ui::render() {
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Signal Analyser Dashboard");


    if(ImGui::Button("Start")) {
        std::cout << "Start Button Pressed !\n";
    }

    ImGui::Separator();
    ImGui::Text("This is a simple dashboard for the Fourier Signal Analyser.");
    ImGui::Text("Custom Theme");
    ImGui::Text(" Live Audio Graph");
    ImGui::Text(" Live Fourier Analysis");
    ImGui::Text("Uses Threads for smoother performance");

    ImGui::End();
}



void ui::graph() {
    ImGui::SetNextWindowPos(ImVec2(350, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Live Audio Graph", NULL, ImGuiWindowFlags_MenuBar);

    // ==========================================
    // THE MENU BAR (Your exact File/CSV Logic!)
    // ==========================================
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            
            if (ImGui::MenuItem("Export Snapshot to CSV")) {
                std::string folderName = "snapshots";

                if (!std::filesystem::exists(folderName)) {
                    std::filesystem::create_directory(folderName);
                }

                int fileIndex = 1;
                std::string baseName = folderName + "/waveform_snapshot";
                std::string extension = ".csv";
                std::string finalFilename = baseName + extension;

                while (true) {
                    std::ifstream checkFile(finalFilename);
                    if (!checkFile.good()) {
                        break; 
                    }
                    finalFilename = baseName + "_" + std::to_string(fileIndex) + extension;
                    fileIndex++;
                }

                std::ofstream file(finalFilename);
                if (file.is_open()) {
                    file << "Index,Amplitude\n";
                    for (size_t i = 0; i < displayData.size(); i++) {
                        file << i << "," << displayData[i] << "\n";
                    }
                    file.close();
                    
                    std::cout << "[SYSTEM] Graph data exported to " << finalFilename << "\n";
                }
                else {
                    std::cout << "[ERROR] Failed to write to " << folderName << " folder.\n";
                }
            }

            if (ImGui::MenuItem("Clear Graph Data")) {
                // Overwrite the entire array with exact zeros to flatline the graph
                displayData.assign(displayData.size(), 0.0f);
                
                // If they clear it while paused, unpause it and set the 2-second timer
                isPaused = false; 
                autoResumeTime = ImGui::GetTime() + 2.0;
                
                std::cout << "[SYSTEM] Graph cleared. Waiting 2 seconds for the next data.\n";
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem(isPaused ? "Resume Live Feed" : "Freeze All Graphs")) {
                isPaused = !isPaused; 
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Separator();
    ImGui::Spacing();
    
    // ==========================================
    // GLOBAL SLIDERS
    // ==========================================
    ImGui::SliderFloat("Digital Gain", &globalGain, 0.0f, 10.0f, "x%.1f");
    ImGui::SliderFloat("Noise Gate", &globalNoiseGate, 0.0f, 0.15f, "%.4f");
    ImGui::Spacing();

    // ==========================================
    // DRAW GRAPH
    // (Math is gone! We just draw the pre-calculated array here)
    // ==========================================
    ImGui::PlotLines(
        "##Waveform",           
        displayData.data(),     
        displayData.size(),     
        0,                      
        isPaused ? "FROZEN: Raw Signals" : "Live Raw Signals", 
        -1.0f,                  
        1.0f,                   
        ImVec2(-1, 200)         
    );

    ImGui::End();
}

void ui::changeTheme() {
    ImGui::SetNextWindowPos(ImVec2(20, 250), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);
    ImGui::Begin("Theme Settings");

    static float my_color[4] = { 0.2f, 0.4f, 0.8f, 1.0f }; // Defaults to a nice blue

    if (ImGui::ColorEdit4("Accent Color", my_color)) {
        
        ImGuiStyle& style = ImGui::GetStyle();

        ImVec4 customColor = ImVec4(my_color[0], my_color[1], my_color[2], my_color[3]);
        style.Colors[ImGuiCol_Button]        = customColor;
        style.Colors[ImGuiCol_Header]        = customColor; // Used for menus and collapsibles
        style.Colors[ImGuiCol_TitleBgActive] = customColor; // The top bar of focused windows
        style.Colors[ImGuiCol_FrameBg]       = customColor; // The background of checkboxes/sliders
        
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(my_color[0] + 0.1f, my_color[1] + 0.1f, my_color[2] + 0.1f, my_color[3]);
        style.Colors[ImGuiCol_ButtonActive]  = ImVec4(my_color[0] - 0.1f, my_color[1] - 0.1f, my_color[2] - 0.1f, my_color[3]);
    }

    ImGui::Separator();
    ImGui::Spacing();

    if(ImGui::Button("Reset to Dark Theme")) {
        ImGui::StyleColorsDark();
    }
    ImGui::SameLine();
    if(ImGui::Button("Reset to Light Theme")) {
        ImGui::StyleColorsLight();
    }
    
    ImGui::End();
}

void ui::audiocapture() {
    ImGui::SetNextWindowPos(ImVec2(350, 450), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Signal Input");

    ImGui::Text("MICROPHONE SETTINGS");
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Turn on Microphone ")) {
        if(mic.isActive()) {
            std::cout << "[WARNING] Microphone is already on! \n";
            ImGui::OpenPopup("Warning");
        } else {
            mic.start();
        }
    }
    
    ImGui::SameLine(); 
    
    if (ImGui::Button("Turn off Microphone")) {
        if(!mic.isActive()) {
            std::cout << "[WARNING] Microphone is already off! \n";
            ImGui::OpenPopup("Warning");
        } else {
            mic.stop();
        }
    }

    // Notification Popup 
    if (ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {   
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[!] Invalid Action");
        ImGui::Separator();
        
        if (mic.isActive()) {
            ImGui::Text("The microphone is already on");
        } else {
            ImGui::Text("The microphone is already off");
        }
        
        ImGui::Spacing();
        
        if (ImGui::Button("Understood", ImVec2(120, 0))) { 
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::EndPopup();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Microphone Status: ");
    ImGui::SameLine();

    if (mic.isActive()) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "ON ");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "OFF");
    }

    // std::cout<< "[SYSTEM] Microphone is " << (mic.isActive() ? "ON\n" : "OFF\n");   

    ImGui::End();
}



void ui::graph_peak() {
    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Amplitude Graph");
    // ui::process_audio_data();

    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Rolling Peak History");
    ImGui::Separator();

    ImGui::PlotLines("##PeakWaveform", peakHistory.data(), peakHistory.size(), 0, 
        isPaused ? "FROZEN: Peak Envelope" : "Live Peak Envelope", 0.0f, 1.0f, ImVec2(-1, 120));

    ImGui::End();
}


void ui::process_audio_data() {
    if (isPaused) return;
    if (ImGui::GetTime() < autoResumeTime) return;

    std::vector<float> audioData = mic.getAudio();
    if (audioData.empty()) return;

    // if (displayData.size() != audioData.size()) {
    //     displayData.resize(audioData.size());
    // }

    // --- SAFETY CHECK 1: Initialize main graph array ---
    if (displayData.size() != audioData.size()) {
        displayData.resize(audioData.size(), 0.0f);
    }

    // --- SAFETY CHECK 2: Initialize Peak array (FIXES SEGFAULT!) ---
    if (peakHistory.empty()) {
        peakHistory.resize(300, 0.0f);
    }

    float currentPeak = 0.0f;

    for (size_t i = 0; i < audioData.size(); i++) {
        float sample = audioData[i];
        
        if (std::abs(sample) < globalNoiseGate) {
            sample = 0.0f; 
        }
        
        displayData[i] = sample * globalGain; 

        if (std::abs(displayData[i]) > currentPeak) {
            currentPeak = std::abs(displayData[i]);
        }
    }

    peakHistory.erase(peakHistory.begin());
    peakHistory.push_back(currentPeak);
}





// Basically all the buttons that'll be designed will be in this place, and defined in dashboard.h, and then called in main.cpp. 