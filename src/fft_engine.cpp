
#include "fourier/fft_engine.h"

// Not necessary as its already defined in the C++ library, but just in case.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


fft::fft() {

}

void fft::calculatefft(CArray &x) {
    const size_t N = x.size();
    if (N <= 1) return;

    CArray even = x[std::slice(0, N/2, 2)];    // x[0], x[2], x[4], ..
    CArray odd = x[std::slice(1, N/2, 2)];   // x[1], x[3], x[5], ..

    calculatefft(even);
    calculatefft(odd);

    for (size_t k = 0; k < N/2; ++k) {
        complex t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N/2] = even[k] - t;
    }
}


void fft::process(const std::vector<float>& audioInput, std::vector<float>& outputMagnitudes) {
    size_t N = audioInput.size();
    CArray data(N);
    
    for (size_t i = 0; i < N; ++i) {
        data[i] = complex(audioInput[i], 0.0); 
    }   

    calculatefft(data);

    outputMagnitudes.resize(N/2);
    for (size_t i = 0; i < N/2; ++i) {
        // outputMagnitudes[i] = std::abs(data[i]);
        outputMagnitudes[i] = static_cast<float>(std::abs(data[i]));
    }
}

// traditional fast fourier without optimization
void fft::calculatefft_On2(const std::vector<float>& audioInput, std::vector<float>& outputMagnitudes){
    size_t N = audioInput.size();

    if(N==0) return;

    outputMagnitudes.assign(N/2 ,0.0f);   // fill zero
    for(size_t k = 0 ; k< N/2; ++k){
        complex sum(0.0,0.0);
        for(size_t n =0 ; n<N ; ++n){
            double angle = -2.0* M_PI * k * n/N;
            complex euler_part = std::polar(1.0,angle);
            sum+= complex(audioInput[n],0.0) * euler_part;
        }
        outputMagnitudes[k] = static_cast<float>(std::abs(sum) / N);
    }

}

