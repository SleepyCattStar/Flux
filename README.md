# Flux: Real-Time DSP Audio Analyzer


<h1> Overview </h1>
Flux is a multi-threaded, real-time digital signal processing (DSP) application built in C++. It captures live audio from standard microphone inputs and performs instantaneous frequency domain analysis. The core engine features a custom implementation of the Cooley-Tukey Fast Fourier Transform (FFT) algorithm, bypassing the need for heavy external DSP libraries while maintaining a flawless 60 FPS graphical interface.