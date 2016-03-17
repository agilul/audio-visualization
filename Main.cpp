// Copyright (c) 2016 Yat Hin Wong

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Sampler.hpp"

const float TARGET_FPS = 30.0;
const size_t N_SAMPLES = 2048;
const std::string MUSIC_FILE = "music.ogg";
const std::string SHADER_FILE = "audiovis.frag";

int main()
{
	// setting up a window with a quad to draw on
	sf::RenderWindow window(sf::VideoMode(800, 600), "Audio Visualization");
	sf::RectangleShape rectangle(sf::Vector2f(window.getSize()));
	
	// loading music file samples on memory
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile(MUSIC_FILE)) return -1;
	sf::Sound sound;
	sound.setBuffer(buffer);
	sound.play();

	// initialising the FFT wrapper class
	Sampler sampler(buffer.getSamples(), buffer.getSampleCount(), buffer.getSampleRate(), buffer.getChannelCount(), N_SAMPLES);

	// the FFT results will be written on a texture that will be passed to the GPU
	sf::Texture spectrum;
	spectrum.create((unsigned int)sampler.size(), 1);
	std::vector<sf::Uint8> pixels(spectrum.getSize().x * 4);
	spectrum.update(pixels.data());

	// time since application start, will be fed to GPU as well
	float timePassed = 0.0f;

	// loading a fragment shader from file
	if (!sf::Shader::isAvailable()) return -1;
	sf::Shader shader;
	if (!shader.loadFromFile(SHADER_FILE, sf::Shader::Fragment)) return -1;

	// setting shader uniform variables
	shader.setParameter("uRes", rectangle.getSize());
	shader.setParameter("uTime", timePassed);
	shader.setParameter("uSpectrum", spectrum);

	// used to keep a steady FPS
	sf::Clock clock;
	sf::Time frame = sf::seconds(1.0f / TARGET_FPS);

	while (window.isOpen())
	{
		sf::Time elapsed = clock.restart();

		// processInput
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// applies a FFT to the last N_SAMPLES music samples
		sampler.update(sound.getPlayingOffset(), pixels);

		// updates the texture with FFT results
		spectrum.update(pixels.data());

		timePassed += elapsed.asSeconds();
		shader.setParameter("uTime", timePassed);

		// render
		window.clear();
		window.draw(rectangle, &shader);
		window.display();

		// sleeps if necessary, to reduce CPU usage
		sf::Time sleepTime = frame - clock.getElapsedTime();
		if (sleepTime > sf::Time::Zero)
		{
			sf::sleep(sleepTime);
		}
	}

	return 0;
}
