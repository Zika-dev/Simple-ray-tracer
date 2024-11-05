#pragma once

#include "utils.hpp"

using namespace Utils;

class Mouse {

public:

	Mouse();

	vec2 getDelta();

	void setDelta(vec2 delta);

private:
	vec2 delta;
};

extern Mouse mouse;