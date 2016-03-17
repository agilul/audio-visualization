// Copyright (c) 2016 Yat Hin Wong

#pragma once

#include <SFML/Audio.hpp>
#include "FTT.hpp"

const double FACTOR = 0.0003;

class Sampler
{
public:
	Sampler(const sf::Int16 *samples, sf::Uint64 count, unsigned int rate, unsigned int channels, size_t chunksize);
	~Sampler();

	void update(sf::Time &position, std::vector<sf::Uint8> &data);
	size_t size();

private:
	const sf::Int16* m_samples;
	sf::Uint64 m_count;
	unsigned int m_rate;
	unsigned int m_channels;
	size_t m_chunksize;
	size_t m_size;
};
