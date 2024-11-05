#pragma once
#include "Scene.hpp"
#include <vector>
#include "HittableManager.hpp"
#include <mutex>

struct Object {
	vec2 pos;
	double y = -999; // For sorting
	int id = -1;
	vec2 screenPos; // For camera panning

	Object() = default;

	Object(vec2 pos, double y, int id, vec2 screenPos)
		: pos(pos), y(y), id(id), screenPos(screenPos) {}
};

struct Circle : public Object {
	double radius;

	Circle(vec2 pos, double y, int id, vec2 screenPos, double radius)
		: Object(pos, y, id, screenPos), radius(radius) {}

	Circle() = default;
};
struct _Camera : public Object {

	_Camera(vec2 pos, double y, int id, vec2 screenPos, double FOV)
		: Object(pos, y, id, screenPos) {}

	_Camera() = default;
};

class Editor {

	public:
		Editor(Scene& scene, SDL_Renderer* renderer, std::mutex& renderMuted, SDL_Window* window);
		void draw();
		void updateObjects();
		vec2i worldToScreen(vec2 worldPos);

	private:
		Scene& scene;
		SDL_Texture* renderTexture = nullptr;
		SDL_Renderer* renderer = nullptr;
		std::vector<Circle> objects; // Objects in world space
		//_Camera camera;
		HittableManager& hittableManager;
		float pixelsPerUnit = 25;
		int editorWidth = 600;
		int editorHeight = 400;

		std::mutex& renderMutex;
		SDL_Window* window;

		bool XZAxis = true;
};