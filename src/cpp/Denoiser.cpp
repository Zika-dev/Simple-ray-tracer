#include "Denoiser.hpp"
#include "Log.hpp"

Denoiser::Denoiser() {
	device = oidn::newDevice();
	device.commit();
	filter = device.newFilter("RT");
}

void Denoiser::denoise(SDL_Texture* texture, std::vector<Color>& colorBuffer, int width, int height, bool gammaCorrection, double gamma) {
	if (texture == nullptr) {
		APP_LOG_ERROR("Texture is null");
		return;
	}

	std::vector<float> colorBuf = convertToOIDNFormat(colorBuffer, width, height);
	std::vector<float> outputBuffer = colorBuf;

	filter.setImage("color", colorBuf.data(), oidn::Format::Float3, width, height);
	filter.setImage("output", outputBuffer.data(), oidn::Format::Float3, width, height);
	filter.commit();

	filter.execute();

	const char* errorMessage;
	if (device.getError(errorMessage) != oidn::Error::None) {
		APP_LOG_ERROR("Error: {0}", errorMessage);
		return;
	}

	createTextureFromOIDNFormat(texture, outputBuffer, width, height, gammaCorrection, gamma);
}