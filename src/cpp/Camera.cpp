#include "Camera.hpp"
#include "Log.hpp"
#include "Material.hpp"
#include "HittableManager.hpp"
#include <vector>
#include <tgmath.h>

Camera::Camera(SDL_Renderer* renderer) : renderer(renderer) {
	if (renderer == nullptr) {
		APP_LOG_CRITICAL("Renderer is null");
	}
}

std::vector<Color> Camera::render(const HittableManager& world, HDRI* hdri, bool preview) {

	std::vector<Color> renderBuffer(renderSize.x * renderSize.y);

	rendering = true;
	tracedRays = 0;

	initialize();

	Interval luminanceInterval = Interval(0.01, 0.1);
	double varianceSmoothingFactor = 0.9;

	int tileSize = 16;

	// Split the scene into tiles and trace rays for each tile
	for (int tileY = 0; tileY < renderSize.y; tileY += tileSize) { 
		for (int tileX = 0; tileX < renderSize.x; tileX += tileSize) {
			for (int y = tileY; y < tileY + tileSize && y < renderSize.y; y++) {
				for (int x = tileX; x < tileX + tileSize && x < renderSize.x; x++) { // For each pixel in the tile
					Color pixelColor;

					if (SSAA) { // If SSAA is enabled, trace multiple rays per pixel
						if (adaptiveSampling) { // If adaptive sampling is enabled, trace more rays for pixels with high variance
							std::vector<Color> samples;

							for (int sample = 0; sample < adaptiveSamplingDepth; ++sample) { // Initial samples for approximate variance
								Ray ray = getRay(vec2i(x, y));
								Color color = rayColor(ray, maxDepth, world, hdri);
								pixelColor += color;
								samples.push_back(color);

								++tracedRays;
							}

							// Compute variance and mean
							Color mean = computeMean(samples);
							double variance = computeVariance(samples, mean);
							double luminance = computeLuminance(mean);
							int additionalSamples = 0;
							int adaptiveSamplingMinSamples = 1;

							while (adaptiveSamplingMinSamples > additionalSamples && additionalSamples < maxSamples)
							{
								Ray ray = getRay(vec2i(x, y));
								Color color = rayColor(ray, maxDepth, world, hdri);
								pixelColor += color;
								samples.push_back(color);

								++tracedRays;
								++additionalSamples;

								mean = computeMean(samples);
								variance = varianceSmoothingFactor * variance + (1 - varianceSmoothingFactor) * computeVariance(samples, mean);
								luminance = computeLuminance(mean);

								if (variance > varianceThreshold) // Add more samples if variance is above threshold
								{
									adaptiveSamplingMinSamples++;
								}

								if (luminanceInterval.contains(luminance)) // If the pixel is dark, add more samples
								{
									adaptiveSamplingMinSamples++;
								}
							}

							pixelSamplesScale = 1.0 / samples.size();
						}
						else
						{
							for (int sample = 0; sample < maxSamples; ++sample) {
								Ray ray = getRay(vec2i(x, y));
								Color color = rayColor(ray, maxDepth, world, hdri);
								pixelColor += color;

								++tracedRays;
							}
						}

						pixelColor *= pixelSamplesScale;
						int index = y * renderSize.x + x;
						renderBuffer[index] = pixelColor;
					}
					else
					{
						vec3 pixelCenter = pixelZero + (x * pixelDeltaU) + (y * pixelDeltaV);
						vec3 rayDirection = pixelCenter - center;
						Ray ray = { center, rayDirection };
						pixelColor = rayColor(ray, maxDepth, world, hdri);

						int index = y * renderSize.x + x;
						renderBuffer[index] = pixelColor;

						++tracedRays;
					}
				}
			}

			if (preview) { // If preview is enabled, update the preview buffer
				std::lock_guard<std::mutex> lock(renderPreviewMutex);
				previewBuffer = renderBuffer;
			}
		}
	}

	rendering = false;

	previewBuffer.clear();

	return renderBuffer;
}

// used for Super Sampling Anti-Aliasing (SSAA)
Ray Camera::getRay(vec2i pPosition) { // Get a ray around a pixel
	Ray ray;

	vec3 offset = randomSquare(); // Random offset

	vec3 sample = pixelZero + ((pPosition.x + offset.x) * pixelDeltaU) + ((pPosition.y + offset.y) * pixelDeltaV); // Ray origin offset by random amount

	ray.origin = center; // Ray origin is the center of the camera, direction is the sample offset from the center
	ray.direction = sample - ray.origin;

	return ray;
}

vec3 Camera::randomSquare() {
	return vec3(randomDouble() - 0.5, randomDouble() - 0.5, 0); // Square centered at origin
}

SDL_Texture* Camera::createRenderTexture() {
	if (renderTexture != nullptr) {
		SDL_DestroyTexture(renderTexture);
	}
	renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, (int)(width / (aspectRatioComponents.x / aspectRatioComponents.y)));

	APP_LOG_INFO("Render texture created: {0}x{1}", width, (int)(width / (aspectRatioComponents.x / aspectRatioComponents.y)));

	return renderTexture;
}

void Camera::initialize() {

	pixelSamplesScale = 1.0 / maxSamples;

	double aspectRatio = aspectRatioComponents.x / aspectRatioComponents.y;

	center = cameraLookFrom;

	height = (int)(width / aspectRatio);

	renderSize = { width, height };

	double focalLength = (cameraLookFrom - cameraLookAt).length();

	double h = tan(degreesToRadians(FOV / 2));
	float viewportHeight = 2.0 * h * focalLength;
	vec2 viewportSize = { viewportHeight * ((float)width / height), viewportHeight };

	w = unitVector(cameraLookFrom - cameraLookAt); // Camera backwards vector
	u = unitVector(crossProduct(cameraUp, w)); // Camera right vector
	v = crossProduct(w, u); // Camera up vector

	// Vectors across the viewport
	vec3 viewportU = viewportSize.x * u;
	vec3 viewportV = viewportSize.y * -v;

	pixelDeltaU = viewportU / width;
	pixelDeltaV = viewportV / height;

	vec3 viewportUpperLeft = center - (w * focalLength) - viewportU / 2 - viewportV / 2;

	pixelZero = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);
}

Color Camera::rayColor(const Ray& ray, int depth, const HittableManager& world, HDRI* hdri = nullptr) const {
	hitRecord record;

	if(depth <= 0) { // If the depth is 0, return black
		return Color(0, 0, 0);
	}

	if (world.hit(ray, Interval(0.001, infinity), record)) { // Interval is for ignoring bad rays caused by floating point errors
		if (shading) {
			Ray scattered;
			Color attenuation;

			if (record.material->scatter(ray, record, attenuation, scattered)) { // If the material scatters the ray
				return attenuation * rayColor(scattered, depth - 1, world, hdri); // Recursively call rayColor with the scattered ray to get the average color
			}
			else // Ray is absorbed
			{
				return Color(0, 0, 0);
			}
		}
		else
		{
			return 0.5 * (record.normal + Color(1, 1, 1)); // Return the normal of the hit point
		}
	}

	vec3 unitDirection = unitVector(ray.direction);

	if(hdri != nullptr){ // If an HDRI is provided
		int hdrWidth = hdri->width;
		int hdrHeight = hdri->height;
		float* hdrData = hdri->data;

		// Compute spherical coordinates
		float u = 0.5f + atan2(unitDirection.z, unitDirection.x) / (2 * M_PI);
		float v = 0.5f - asin(unitDirection.y) / M_PI; // Negative because the image is flipped vertically

		// Map UV coordinates to pixel indices
		int pixelX = static_cast<int>(u * hdrWidth) % hdrWidth;   // Wrap horizontally
		int pixelY = static_cast<int>(v * hdrHeight) % hdrHeight; // Wrap vertically

		// Get the pixel index
		int pixelIndex = (pixelY * hdrWidth + pixelX) * 3;  // Each pixel has 3 color components (RGB)

		Color pixelColor(hdrData[pixelIndex], hdrData[pixelIndex + 1], hdrData[pixelIndex + 2]);

		// Reinhard tone mapping
		Color mappedColor = pixelColor / (pixelColor + Color(1, 1, 1));

		if (hdrData != nullptr) {
			return mappedColor;
		}
		else {
			APP_LOG_CRITICAL("HDR data is null");
			return Color(0, 0, 0);
		}
	}
	else // Background color gradient
	{
		auto a = 0.5 * (unitDirection.y + 1.0);
		return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(1, 0.7, 0.5); // Lerp between white and blue
	}
}