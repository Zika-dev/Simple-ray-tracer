#pragma once
#include <SDL2/SDL.h>
#include <string>

class Keyboard
{
public:
	Keyboard();
	~Keyboard();

	void update(const SDL_Event& event);
	bool isPressed(unsigned char key);
	bool isReleased(unsigned char key);
	int stringToKey(std::string key);

private:
	bool keyStates[256];
};

extern Keyboard keyboard;