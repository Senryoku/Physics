#ifndef _FPSCOUNTER_HPP_
#define _FPSCOUNTER_HPP_

#include <sstream>
#include "SFML/Graphics.hpp"

class FPSCounter
{
	public:
		FPSCounter();

		void update();
		sf::Text getText() { return myText; }
		std::string getString() { return myText.getString(); }
	protected:
		sf::Clock		myClock;
		unsigned int	myFrames;
		sf::Font		myFont;
		sf::Text		myText;
	private:
};

#endif // _FPSCOUNTER_HPP_
