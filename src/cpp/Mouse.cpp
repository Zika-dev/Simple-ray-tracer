#include "Mouse.hpp"
#include "Log.hpp"

Mouse::Mouse() {

}

vec2 Mouse::getDelta() {
	return delta;
}

void Mouse::setDelta(vec2 delta) {
	this->delta = delta;
}

Mouse mouse;