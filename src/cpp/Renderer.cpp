#include "Renderer.hpp"
#include "utils.hpp"
#include <math.h>
#include "Log.hpp"
#include "Hittable.hpp"

Renderer::Renderer(SDL_Renderer* renderer, SDL_Texture* renderTexture, HittableManager& hittableManager) : renderer(renderer), renderTexture(renderTexture), hittableManager(hittableManager)  {

}

void Renderer::render() {

	//// Clear screen
	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	//SDL_RenderClear(renderer);

	//// Draw circle
	//SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

	//void* pixels;
	//int pitch;

	//if (SDL_LockTexture(renderTexture, NULL, &pixels, &pitch) != 0) {
	//	APP_LOG_CRITICAL("Failed to lock texture: {0}", SDL_GetError());
	//	return;
	//}

	//Uint32* pixelArray = static_cast<Uint32*>(pixels);

	//for (int y = 0; y < renderSize.y; y++) {
	//	for (int x = 0; x < renderSize.x; x++) {
	//		vec3 pixelCenter = pixelZero + (x * pixelDeltaU) + (y * pixelDeltaV);
	//		vec3 rayDirection = pixelCenter - cameraCenter;
	//		Ray ray = { cameraCenter, rayDirection };

	//		Color color = rayColor(ray, hittableManager);
	//		setPixel(pixelArray, pitch, x, y, color);
	//	}
	//}

	//SDL_UnlockTexture(renderTexture);
}