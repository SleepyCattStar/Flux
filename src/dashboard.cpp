// Docs->    ReadMe of https://github.com/ocornut/imgui

#include "DashboardUI/dashboard.h"         // audio mic, is declared here/
#include "fourier/audio_capture.h"     // for the class audio, class instance mic, and the function start(), stop()
#include <iostream>
#include<fstream>   // to make export data to csv file 
#include<string>    
#include <filesystem>  

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

    static bool isPaused = false;
    static std::vector<float> displayData(2048, 0.0f);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            // if (ImGui::MenuItem("Export Snapshot to CSV")) {
            //     std::ofstream file("waveform_snapshot.csv");
            //     if (file.is_open()) {
            //         file << "Index,Amplitude\n";
            //         for (size_t i = 0; i < displayData.size(); i++) {
            //             file << i << "," << displayData[i] << "\n";
            //         }
            //         file.close();
            //         std::cout << "[SYSTEM] Graph data exported to waveform_snapshot.csv\n";
            //     }
            //     else
            //         {
            //             std::cout << "[ERROR] Failed to export graph data.\n";
            //         }
            // }


            // Bwetter File handling and proper folder structure
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

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem(isPaused ? "Resume Live Feed" : "Freeze Graph")) {
                isPaused = !isPaused; 
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // ImGui::Text("LIVE Audio Signal from  Microphone");
    ImGui::Separator();
    ImGui::Spacing();
    
    std::vector<float> audioData = mic.getAudio();

    if (audioData.size() > 0 || isPaused) {
        
        static float gain = 1.0f;
        static float noiseGate = 0.002f;

        ImGui::SliderFloat("Digital Gain", &gain, 0.0f, 10.0f, "x%.1f");
        ImGui::SliderFloat("Noise Gate", &noiseGate, 0.0f, 0.15f, "%.4f");
        ImGui::Spacing();

        if (!isPaused && audioData.size() > 0) {
            if (displayData.size() != audioData.size()) {
                displayData.resize(audioData.size());
            }
            
            for(size_t i = 0; i < audioData.size(); i++) {
                float sample = audioData[i];

                if (std::abs(sample) < noiseGate) {
                    sample = 0.0f; 
                }

                displayData[i] = sample * gain; 
            }
        }

        ImGui::PlotLines(
            "##Waveform",           
            displayData.data(),     
            displayData.size(),     
            0,                      
            isPaused ? "FROZEN: Raw Signals" : "Raw Signals", 
            -1.0f,                  
            1.0f,                   
            ImVec2(-1, 200)         
        );
    } else {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Waiting for audio data...");
    }

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

    // The State Machine reading the hardware driver
    if (mic.isActive()) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "ON ");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "OFF");
    }

    // std::cout<< "[SYSTEM] Microphone is " << (mic.isActive() ? "ON\n" : "OFF\n");   

    ImGui::End();
}

// void ui::audiocapture() {
//     ImGui::SetNextWindowPos(ImVec2(350, 450), ImGuiCond_FirstUseEver);
//     ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiCond_FirstUseEver);
//     ImGui::Begin("Signal Input");

//     ImGui::Text("MICROPHONE SETTINGS");
//     ImGui::Separator();
//     ImGui::Spacing();

//     // 1. Gray out the START button if the mic is ALREADY active
//     ImGui::BeginDisabled(mic.isActive()); 
//     if (ImGui::Button("Turn on Microphone")) {
//         mic.start();
//     }
//     ImGui::EndDisabled(); // Always close the disabled block!

//     ImGui::SameLine(); 

//     // 2. Gray out the STOP button if the mic is ALREADY offline
//     ImGui::BeginDisabled(!mic.isActive()); 
//     if (ImGui::Button("Turn off Microphone")) {
//         mic.stop();
//     }
//     ImGui::EndDisabled(); // Always close the disabled block!

//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();

//     ImGui::Text("Microphone Status: ");
//     ImGui::SameLine();

//     if (mic.isActive()) {
//         ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "ON ");
//     } else {
//         ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "OFF");
//     }

//     ImGui::End();
// }






// Basically all the buttons that'll be designed will be in this place, and defined in dashboard.h, and then called in main.cpp. 