#pragma once

#include <cmath>
#include <limits>
#include <random>
#include "Log.hpp"
#include <SDL2/SDL.h>
#include <Windows.h>
#include <commdlg.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

namespace Utils {

	double randomDouble();
	double randomDouble(double min, double max);

	struct vec2i {
		int x, y;

		vec2i(int x, int y) : x(x), y(y) {}

		vec2i() : x(0), y(0) {}

		vec2i operator+(const vec2i& other) {
			return { x + other.x, y + other.y };
		}

		vec2i operator-(const vec2i& other) {
			return { x - other.x, y - other.y };
		}

		vec2i operator*(const vec2i& other) {
			return { x * other.x, y * other.y };
		}

		vec2i operator*(int scalar) {
			return { x * scalar, y * scalar };
		}

		vec2i operator/(const vec2i& other) {
			return { x / other.x, y / other.y };
		}

		bool operator==(const vec2i& other) {
			return x == other.x && y == other.y;
		}
	};

	struct vec2 {
		float x, y;

		vec2(float x, float y) : x(x), y(y) {}

		vec2() : x(0), y(0) {}

		vec2 operator+(const vec2& other) {
			return { x + other.x, y + other.y };
		}

		vec2 operator-(const vec2& other) {
			return { x - other.x, y - other.y };
		}

		vec2 operator*(const vec2& other) {
			return { x * other.x, y * other.y };
		}

		vec2 operator*(float scalar) {
			return { x * scalar, y * scalar };
		}

		vec2 operator/(const vec2& other) {
			return { x / other.x, y / other.y };
		}

		vec2 operator/(float scalar) {
			return { x / scalar, y / scalar };
		}
	};

	// 3D
	struct vec3 {
		double x, y, z;

		vec3(double x, double y, double z) : x(x), y(y), z(z) {}

		vec3() : x(0), y(0), z(0) {}

		// Compound assignment operators
		vec3& operator+=(const vec3& other) {
			x += other.x; y += other.y; z += other.z;
			return *this;
		}

		vec3& operator+=(double other) {
			x += other; y += other; z += other;
			return *this;
		}

		vec3& operator-=(const vec3& other) {
			x -= other.x; y -= other.y; z -= other.z;
			return *this;
		}

		vec3& operator-=(double other) {
			x -= other; y -= other; z -= other;
			return *this;
		}

		vec3& operator*=(const vec3& other) {
			x *= other.x; y *= other.y; z *= other.z;
			return *this;
		}

		vec3& operator*=(double scalar) {
			x *= scalar; y *= scalar; z *= scalar;
			return *this;
		}

		vec3& operator/=(const vec3& other) {
			x /= other.x; y /= other.y; z /= other.z;
			return *this;
		}

		vec3& operator/=(double scalar) {
			x /= scalar; y /= scalar; z /= scalar;
			return *this;
		}

		// Non-compound operators (rely on compound ones)
		vec3 operator+(const vec3& other) const {
			vec3 result = *this;
			result += other;
			return result;
		}

		vec3 operator+(double other) const {
			vec3 result = *this;
			result += other;
			return result;
		}

		vec3 operator-(const vec3& other) const {
			vec3 result = *this;
			result -= other;
			return result;
		}

		vec3 operator-(double other) const {
			vec3 result = *this;
			result -= other;
			return result;
		}

		vec3 operator-() const {
			return { -x, -y, -z };
		}

		vec3 operator*(const vec3& other) const {
			vec3 result = *this;
			result *= other;
			return result;
		}

		vec3 operator*(double scalar) const {
			vec3 result = *this;
			result *= scalar;
			return result;
		}

		vec3 operator/(const vec3& other) const {
			vec3 result = *this;
			result /= other;
			return result;
		}

		vec3 operator/(double scalar) const {
			vec3 result = *this;
			result /= scalar;
			return result;
		}

		double length() const {
			return { sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)) };
		}
	};

	using Color = vec3;
	using Point3 = vec3;

	inline vec3 operator*(double scalar, const vec3& v) {
		return vec3(v.x * scalar, v.y * scalar, v.z * scalar);
	}

	inline vec3 operator*(int scalar, const vec3& v) {
		return vec3(v.x * scalar, v.y * scalar, v.z * scalar);
	}

	inline vec3 randomVector() { // Random vector in unit sphere		
		return vec3(randomDouble(), randomDouble(), randomDouble());
	}

	inline vec3 randomVector(double min, double max) {
		return vec3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
	}

	inline vec3 unitVector(const vec3& v) {
		return v / v.length();
	}

	inline vec3 crossProduct(const vec3& v1, const vec3& v2) {
		return vec3(v1.y * v2.z - v1.z * v2.y,
						v1.z * v2.x - v1.x * v2.z,
						v1.x * v2.y - v1.y * v2.x);
	}

	inline vec3 randomUnitVector() {
		while (true) {
			Point3 p = randomVector(-1, 1); // Random point in unit sphere
			double length = p.length();
			double cubeArea = pow(length, 2); // Area around origin

			if (1e-160 < cubeArea && cubeArea <= 1) { // If the ray is inside of the unit sphere (1^2 = 1)
				return p / length; // Normalize the vector
			}
		}
	}

	inline double dotProduct(const vec3& v1, const vec3& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	inline vec3 randomPointOnHemisphere(const vec3& normal) {
		vec3 inUnitSphere = randomUnitVector(); // Random point in unit sphere
		if (dotProduct(inUnitSphere, normal) > 0.0) { // If point is in the same hemisphere as the normal (negative if opposite direction)
			return inUnitSphere;
		}
		else {
			return -inUnitSphere; // Flip the vector
		}
	
	}

	inline bool nearZero(const vec3& v) {
		const double s = 1e-8;
		return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
	}

	inline vec3 reflect(const vec3& v, const vec3& n) {
		return v - 2 * dotProduct(v, n) * n;
	}

	inline vec3 refract(const vec3& incident, const vec3& normal, double refractiveRatio) { // Snell's Law
		double cosTheta = fmin(dotProduct(-incident, normal), 1.0); // If dot product (cosine) is over 1, it will be clamped to 1
		vec3 rOutPerpendicular = refractiveRatio * (incident + cosTheta * normal); // Perpendicular component
		vec3 rOutParallel = -sqrt(fabs(1.0 - pow(rOutPerpendicular.length(), 2))) * normal; // Parallel component
		return rOutPerpendicular + rOutParallel; // Total refraction
	}

	struct Vector {
		Point3 origin;
		vec3 direction;

		Vector operator+(const Vector& other) {
			return { origin + other.origin, direction + other.direction };
		}

		Vector operator-(const Vector& other) {
			return { origin - other.origin, direction - other.direction };
		}

		Point3 at(const double t) const {
			return origin + direction * t;
		}
	};

	using Ray = Vector;

	struct HDRI {
		float* data;
		int width, height;
		std::string name;
	};

	// Constants

	constexpr double infinity = std::numeric_limits<double>::infinity();
	constexpr double pi = 3.1415926535897932385;

	// Utility functions

	inline double degreesToRadians(double degrees) {
		return degrees * pi / 180.0;
	}

	inline double radiansToDegrees(double radians) {
		return radians * 180.0 / pi;
	}

	inline double randomDouble() {
		static std::uniform_real_distribution<double> distribution(0.0, 1.0);
		static std::mt19937 generator;
		return distribution(generator);
	}

	inline double randomDouble(double min, double max) {
		return min + (max - min) * randomDouble();
	}

	inline float linearToGamma(double linear, double gamma) {
		if (linear > 0) {
			return pow(linear, 1 / gamma);
		}

		return 0;
	}

	inline void createTextureFromColors(SDL_Texture* texture, const std::vector<Color>& colors, vec2i size, bool gammaCorrection = true, double gamma = 2.2) {

		void* pixels;
		int pitch;
		if (SDL_LockTexture(texture, nullptr, &pixels, &pitch) != 0) {
			APP_LOG_CRITICAL("Failed to lock texture: {0}", SDL_GetError());
			return;
		}
		
		// Query width and height
		int w, h;
		SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

		if (size.x != w || size.y != h) {
			APP_LOG_CRITICAL("Texture size mismatch: {0}x{1} != {2}x{3}", size.x, size.y, w, h);
			return;
		}

		int width = size.x;
		int height = size.y;

		// Lock the surface for direct pixel access
		int bytesPerPixel = pitch / width;

		// Fill the surface with color data
		Uint32* pixelPtr = static_cast<Uint32*>(pixels);
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				int index = y * width + x;
				if (index < colors.size()) {
					// Map the double values to 0-255 range and create a pixel
					Uint8 r, g, b;
					if (gammaCorrection) {
						r = static_cast<Uint8>(linearToGamma(colors[index].x, gamma) * 255);
						g = static_cast<Uint8>(linearToGamma(colors[index].y, gamma) * 255);
						b = static_cast<Uint8>(linearToGamma(colors[index].z, gamma) * 255);
					}
					else {
						r = static_cast<Uint8>(colors[index].x * 255);
						g = static_cast<Uint8>(colors[index].y * 255);
						b = static_cast<Uint8>(colors[index].z * 255);
					}

					Uint32 pixel = SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), r, g, b);

					// Calculate the pixel's position in the buffer and set the pixel value
					pixelPtr[y * (pitch / bytesPerPixel) + x] = pixel;
				}
				else {
					// Handle out-of-bounds (optional)
					pixelPtr[y * (pitch / bytesPerPixel) + x] = SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32), 0, 0, 0); // Black
				}
			}
		}

		// Unlock the surface
		SDL_UnlockTexture(texture);
	}

	inline SDL_Surface* ConvertTextureToSurface(SDL_Texture* texture, SDL_Renderer* renderer) {
		// Get the width, height, and pixel format of the texture
		int width, height;
		Uint32 format;
		SDL_QueryTexture(texture, &format, nullptr, &width, &height);

		// Create an SDL surface that matches the texture's pixel format
		SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format);
		if (!surface) {
			APP_LOG_ERROR("Failed to create surface: {0}", SDL_GetError());
			return nullptr;
		}

		// Lock the texture to get access to its pixel data
		void* pixels;
		int pitch;
		if (SDL_LockTexture(texture, nullptr, &pixels, &pitch) != 0) {
			APP_LOG_ERROR("Failed to lock texture: {0}", SDL_GetError());
			SDL_FreeSurface(surface);
			return nullptr;
		}

		// Copy the texture pixel data into the surface
		memcpy(surface->pixels, pixels, pitch * height);

		// Unlock the texture after copying
		SDL_UnlockTexture(texture);

		return surface;
	}

	inline Color computeMean(const std::vector<Color> color) {
		Color mean;
		for (int i = 0; i < color.size(); i++) {
			//APP_LOG_INFO("Color: {0}, {1}, {2}", color[i].x, color[i].y, color[i].z);
			mean += color[i];
		}
		//APP_LOG_INFO("Mean: {0}, {1}, {2}", mean.x, mean.y, mean.z);
		return mean / color.size(); // Return the mean
	}

	inline double computeVariance(const std::vector<Color> color, Color mean) { // Compute the variance of a color
		double variance = 0;
		for (int i = 0; i < color.size(); i++) {
			variance += pow(color[i].x - mean.x, 2) + pow(color[i].y - mean.y, 2) + pow(color[i].z - mean.z, 2);
		}
		return variance / color.size(); // Return the variance
	}

	inline double computeLuminance(Color color) {
		return 0.2126 * color.x + 0.7152 * color.y + 0.0722 * color.z; // Luminance formula
	}

	inline std::vector<float> convertToOIDNFormat(const std::vector<Color>& colorBuffer, int width, int height) { // Convert the color buffer to OIDN format
		// Allocate a buffer for OIDN format (3 floats per pixel)
		std::vector<float> oidnBuffer(width * height * 3);

		// Copy and convert each pixel's color
		for (size_t i = 0; i < colorBuffer.size(); ++i) {
			oidnBuffer[i * 3 + 0] = static_cast<float>(colorBuffer[i].x); // Red channel
			oidnBuffer[i * 3 + 1] = static_cast<float>(colorBuffer[i].y); // Green channel
			oidnBuffer[i * 3 + 2] = static_cast<float>(colorBuffer[i].z); // Blue channel
		}

		return oidnBuffer;
	}

	inline float linearToSRGB(float value) {
		if (value <= 0.0031308f) {
			return 12.92f * value;
		}
		else {
			return 1.055f * std::pow(value, 1.0f / 2.4f) - 0.055f;
		}
	}

	inline void createTextureFromOIDNFormat(SDL_Texture* texture, const std::vector<float>& outputBuffer, int width, int height, bool gammaCorrection, float gamma) {
		if (!texture) {
			APP_LOG_ERROR("Texture is null");
			return;
		}

		// Convert float buffer to Uint8 format for SDL texture
		std::vector<uint8_t> textureData(width * height * 4); // RGB data

		if (gammaCorrection) {
			for (size_t i = 0; i < outputBuffer.size() / 3; ++i) {
				textureData[i * 4 + 3] = static_cast<uint8_t>(std::clamp(linearToGamma(outputBuffer[i * 3 + 0], 2.2) * 255.0f, 0.0f, 255.0f)); // Red channel
				textureData[i * 4 + 2] = static_cast<uint8_t>(std::clamp(linearToGamma(outputBuffer[i * 3 + 1], 2.2) * 255.0f, 0.0f, 255.0f)); // Green channel
				textureData[i * 4 + 1] = static_cast<uint8_t>(std::clamp(linearToGamma(outputBuffer[i * 3 + 2], 2.2) * 255.0f, 0.0f, 255.0f)); // Blue channel
				textureData[i * 4 + 0] = 255; // Alpha channel (fully opaque)
			}
		}
		else
		{
			for (size_t i = 0; i < outputBuffer.size() / 3; ++i) {
				textureData[i * 4 + 3] = static_cast<uint8_t>(std::clamp(outputBuffer[i * 3 + 0] * 255.0f, 0.0f, 255.0f)); // Red channel
				textureData[i * 4 + 2] = static_cast<uint8_t>(std::clamp(outputBuffer[i * 3 + 1] * 255.0f, 0.0f, 255.0f)); // Green channel
				textureData[i * 4 + 1] = static_cast<uint8_t>(std::clamp(outputBuffer[i * 3 + 2] * 255.0f, 0.0f, 255.0f)); // Blue channel
				textureData[i * 4 + 0] = 255; // Alpha channel (fully opaque)
			}
		}

		// Update the texture with the new pixel data
		void* pixels;
		int pitch;

		if (SDL_LockTexture(texture, nullptr, &pixels, &pitch) != 0) {
			APP_LOG_ERROR("Failed to lock texture: {0}", SDL_GetError());
			return;
		}

		memcpy(pixels, textureData.data(), textureData.size());

		SDL_UnlockTexture(texture);
	}

	inline std::string openFile(const char* filter) {

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = GetActiveWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn)) {
			return ofn.lpstrFile;
		}

		HWND activeWindow = GetActiveWindow(); // Get active window
		APP_LOG_WARN("[Utils] File dialog closed or failed!");

		return std::string();

	}

	inline std::string saveFile(const char* filter) {

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = GetActiveWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn)) {
			return ofn.lpstrFile;
		}

		HWND activeWindow = GetActiveWindow(); // Get active window
		APP_LOG_WARN("[Utils] File dialog closed or failed!");

		return std::string();
	}
}