#pragma once
#include "Scene.hpp"
#include <mutex>
#include "Denoiser.hpp"

class RenderResult {

public:
	RenderResult(Scene& scene, std::mutex& renderPreviewMutex, Denoiser& denoiser);
	void draw();

private:
	Scene& scene;
	SDL_Texture* renderTexture;

	bool isRendering = false;
	bool updateTexture = false;

	int width, height;

	std::vector<Color> renderTextureData;
	std::vector<Color> previewTextureData;

	SDL_Texture* previewTexture = nullptr;

	std::mutex& renderPreviewMutex;

	Denoiser& denoiser;
};