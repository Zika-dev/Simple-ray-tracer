#include "Keyboard.hpp"
#include <cctype>
#include <algorithm>

struct key {
	std::string name;
	int code;
};

key keycodeMap[256] = {
	{"a", 4},
	{"b", 5},
	{"c", 6},
	{"d", 7},
	{"e", 8},
	{"f", 9},
	{"g", 10},
	{"h", 11},
	{"i", 12},
	{"j", 13},
	{"k", 14},
	{"l", 15},
	{"m", 16},
	{"n", 17},
	{"o", 18},
	{"p", 19},
	{"q", 20},
	{"r", 21},
	{"s", 22},
	{"t", 23},
	{"u", 24},
	{"v", 25},
	{"w", 26},
	{"x", 27},
	{"y", 28},
	{"z", 29},
	{"1", 30},
	{"2", 31},
	{"3", 32},
	{"4", 33},
	{"5", 34},
	{"6", 35},
	{"7", 36},
	{"8", 37},
	{"9", 38},
	{"0", 39},
	{"return", 40},
	{"escape", 41},
	{"backspace", 42},
	{"tab", 43},
	{"space", 44},
	{"-", 45},
	{"=", 46},
	{"[", 47},
	{"]", 48},
	{"\\", 49},
	{";", 51},
	{"'", 52},
	{"`", 53},
	{",", 54},
	{".", 55},
	{"/", 56},
	{"capslock", 57},
	{"f1", 58},
	{"f2", 59},
	{"f3", 60},
	{"f4", 61},
	{"f5", 62},
	{"f6", 63},
	{"f7", 64},
	{"f8", 65},
	{"f9", 66},
	{"f10", 67},
	{"f11", 68},
	{"f12", 69},
	{"printscreen", 70},
	{"scrolllock", 71},
	{"pause", 72},
	{"insert", 73},
	{"lshift",225}
};

Keyboard::Keyboard()
{
	memset(keyStates, false, sizeof(keyStates)); // Set all keys to false
}

Keyboard::~Keyboard()
{

}

void Keyboard::update(const SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		keyStates[event.key.keysym.scancode] = true;
		break;
	case SDL_KEYUP:
		keyStates[event.key.keysym.scancode] = false;
		break;
	default:
		break;
	}
}

bool Keyboard::isPressed(unsigned char key)
{
	return keyStates[key];
}

bool Keyboard::isReleased(unsigned char key)
{
	return !keyStates[key];
}

int Keyboard::stringToKey(std::string key) {
	for (int i = 0; i < 256; i++)
	{
		if (keycodeMap[i].name == key)
		{
			return keycodeMap[i].code;
		}
	}
	return -1;

}

Keyboard keyboard; // Global keyboard