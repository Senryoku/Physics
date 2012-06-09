#include "FPSCounter.hpp"

FPSCounter::FPSCounter() : myFrames(0), myText("FPS : 0", myFont) // Evite un bug de SFML avec la default font...
{
	myFont.loadFromFile("data/V5PRC___.TTF");
	myText.setCharacterSize(20);
	myText.setColor(sf::Color(255, 255, 255, 170));
	myText.setPosition(5.f, 5.f);
	myClock.restart();
}

void FPSCounter::update()
{
	myFrames++;
	if (myClock.getElapsedTime().asSeconds() >= 1.f)
	{
		std::ostringstream oss;
		oss << "FPS : " << myFrames;
		myText.setString(oss.str());

		myFrames = 0;
		myClock.restart();
	}
}
