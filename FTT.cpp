// Copyright (c) 2016 Yat Hin Wong

#include "FTT.hpp"

void fft(CArray& x)
{
	const size_t N = x.size();
	if (N <= 1) return;

	CArray even = x[std::slice(0, N / 2, 2)];
	CArray odd = x[std::slice(1, N / 2, 2)];

	fft(even);
	fft(odd);

	for (size_t k = 0; k < N / 2; ++k)
	{
		Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
		x[k] = even[k] + t;
		x[k + N / 2] = even[k] - t;
	}
}
