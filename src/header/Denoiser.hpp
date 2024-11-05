#pragma once
#include "SDL2/SDL.h"
#include "OpenImageDenoise/oidn.hpp"
#include "utils.hpp"

using namespace Utils;

class Denoiser {
	public:
		Denoiser();

		void denoise(SDL_Texture* texture, std::vector<Color>& colorBuffer, int width, int height, bool gammaCorrection = true, double gamma = 2.2);

	private:
		oidn::DeviceRef device;
		oidn::FilterRef filter;
};