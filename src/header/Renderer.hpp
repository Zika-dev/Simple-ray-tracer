#pragma once

#include <SDL2/SDL.h>
#include "utils.hpp"
#include <utility>
#include <vector>
#include "HittableManager.hpp"

using namespace Utils;

class Renderer {
private:
	SDL_Renderer* renderer;
	SDL_Texture* renderTexture;

	std::vector<Vector> rays;

	HittableManager hittableManager;

public:
	Renderer(SDL_Renderer* renderer, SDL_Texture* renderTexture, HittableManager& hittableManager);

	void render();
};