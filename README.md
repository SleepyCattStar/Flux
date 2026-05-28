# Flux: Real-Time DSP Audio Analyzer

## Overview
Flux is a multi-threaded, real-time digital signal processing (DSP) application built in C++. It captures live audio from standard microphone inputs and performs instantaneous frequency domain analysis. The core engine features a custom implementation of the Cooley-Tukey Fast Fourier Transform (FFT) algorithm, bypassing the need for heavy external DSP libraries while maintaining a flawless 60 FPS graphical interface.

## Core Features
* **Live Time-Domain Analysis:** Real-time visualization of raw audio waveforms with adjustable digital gain and dynamic noise gating.
* **Frequency Domain Extraction (FFT):** Live spectrum histogram computing frequency magnitudes using an optimized $O(N \log N)$ algorithm.
* **Fundamental Frequency (F0) Tracking:** Automated peak detection that calculates and isolates the dominant frequency pitch (in Hz) from live audio streams, filtering out ambient static and harmonic resonance.
* **Algorithmic Profiling:** Built-in multi-threaded benchmarking suite that compares the execution speed of the $O(N \log N)$ FFT against a Naive $O(N^2)$ Discrete Fourier Transform (DFT) in real-time.
* **Rolling Peak Envelope:** Continuous tracking of maximum amplitude peaks over time to monitor signal strength.
* **Multithreaded Architecture:** Heavy mathematical calculations are moved to background threads and safely managed using standard mutexes, keeping the user interface smooth and completely responsive.

---

## Technical Architecture

### 1. The Audio Pipeline (`audio_capture`)
Handles the hardware-level interfacing with the system microphone. It continuously buffers raw audio samples at a standard sample rate (e.g., 44100 Hz). The data is passed to the UI thread where global gain and noise gating thresholds are applied before math execution.

### 2. The Math Engine (`fft_engine`)
The mathematical core of the application. It processes the cleaned time-domain buffer (`displayData`) and converts it into frequency bins (`fftMagnitudes`).

* **Naive DFT (For Benchmarking):**
  Calculates the frequency spectrum using the standard $O(N^2)$ complex summation:
  $$X_k = \sum_{n=0}^{N-1} x_n \cdot e^{-i \frac{2\pi}{N} k n}$$
* **Cooley-Tukey FFT (Production Engine):**
  A highly optimized divide-and-conquer algorithm operating at $O(N \log N)$ time complexity. 
* **Frequency Conversion:**
  The engine maps standard array indices (bins) back to real-world physics using the DSP formula:
  $$\text{Frequency (Hz)} = \frac{\text{Bin Index} \times \text{Sample Rate}}{\text{FFT Size}}$$

### 3. The UI Controller (`dashboard`)
Built on top of Dear ImGui and OpenGL. It utilizes responsive grid mathematics to scale the layout dynamically based on the user's monitor resolution (`ImGui::GetMainViewport()`). The UI is strictly locked (`ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize`) to maintain the integrity of the data visualization.

---


### 4. Building from Source

**1. Clone the repository**
```bash
git clone [https://github.com/SleepyCattStar/Flux.git](https://github.com/SleepyCattStar/Flux.git)
cd Flux
```

**2. Generate build files using CMake**
```bash
mkdir build
cd build
cmake ..
```

**3. Compile the executable**
```bash
make
```

**4. Run the application**
```bash
./FourierAnalyzer
```

## Download / Installation

If you don't want to build the project from the source . Pre-compiled binaries are available for Windows and Linux.

1. Go to the [Releases Page](https://github.com/SleepyCattStar/Flux/releases/latest).
2. Download the executable for your operating system (`FourierAnalyzer.exe` for Windows, or the Linux binary).
3. Extract the file and run it directly. No installation required!