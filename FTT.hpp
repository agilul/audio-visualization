// Copyright (c) 2016 Yat Hin Wong

#pragma once

#include <complex>
#include <valarray>

const double PI = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

void fft(CArray& x);
