#pragma once

#include<cmath> 
#include <vector>
#include<complex>
#include<valarray>


class fft{

    private:
        typedef std::complex<double> complex;
        typedef std::valarray<complex> CArray;

        void calculatefft(CArray &x);
        // abstraction.

    public:
        fft();
        void process(const std::vector<float>& audioInput, std::vector<float>& outputMagnitudes);
        void calculatefft_On2(const std::vector<float>& audioInput, std::vector<float>& outputMagnitudes);

};