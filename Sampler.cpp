// Copyright (c) 2016 Yat Hin Wong

#include "Sampler.hpp"

Sampler::Sampler(const sf::Int16 *samples, sf::Uint64 count, unsigned int rate, unsigned int channels, size_t chunksize)
	: m_samples(samples), m_count(count), m_rate(rate), m_channels(channels), m_chunksize(chunksize)
{
	m_size = chunksize / m_channels / 2;
}

Sampler::~Sampler()
{
}

void Sampler::update(sf::Time &position, std::vector<sf::Uint8> &data)
{
	// retrieve the current position of the music track in the buffer
	size_t currentSample = static_cast<size_t>(position.asSeconds() * m_rate * m_channels);

	if (currentSample < m_chunksize || currentSample > m_count - 2) return;

	// adjust the selected samples to a format suitable to the FFT function
	std::vector<Complex> vec(m_chunksize / m_channels);
	for (int i = 0; i < vec.size(); ++i)
	{
		// if there are multiple channels, average them
		double val = 0.0;
		for (int j = 0; j < (int)m_channels; ++j)
		{
			val += m_samples[currentSample - m_chunksize + i + j];
		}
		val /= m_channels;

		vec[i] = Complex(val);
	}
	CArray chunk(vec.data(), vec.size());

	// perform FFT
	fft(chunk);

	// write back to a short vector values between 0 and 255
	for (int i = 0, j = 0; i < m_size; ++i)
	{
		sf::Uint8 val = std::min(std::max((int)std::round(
			std::sqrt(chunk[i].real() * chunk[i].real() + chunk[i].imag() * chunk[i].imag()) * FACTOR
			), 0), 255);
		data[j++] = val;
		data[j++] = val;
		data[j++] = val;
		data[j++] = 255;
	}
}

size_t Sampler::size()
{
	return m_size;
}
