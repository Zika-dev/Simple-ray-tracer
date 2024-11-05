#pragma once
#include "Scene.hpp"
#include <thread>
#include <mutex>
#include <atomic>
#include <string.h>
#include "Viewport.hpp"

class RenderSettings {
public:
	RenderSettings(Scene& scene, SDL_Renderer* renderer, Viewport& viewport);

	void draw();


private:
	Scene& scene;

	vec2i renderSize;

	vec2 aspectRatio;

	std::string fileName;

	SDL_Renderer* renderer;

	Viewport& viewport;
};