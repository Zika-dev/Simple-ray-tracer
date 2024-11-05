#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "utils.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "MaterialManager.hpp"

using namespace Utils;

class Scene {
public:
	Scene(Camera& camera, HittableManager& hittableManager, MaterialManager& materialManager, HDRI* hdri = nullptr);

	std::vector<Color>& render(); // Actual rendering
	
	std::vector<Color> renderView(); // Rendering for the viewport

	Camera& getRenderCamera() ;

	Camera& getViewCamera();

	std::vector<Color>& getColorBuffer();

	HittableManager& getHittableManager();

	MaterialManager& getMaterialManager();

	void addHittable(std::shared_ptr<Hittable> hittable);

	int getInitialRayCount() const;

	int getSelectedObject() const;

	void setSelectedObject(int selectedObject);

	HDRI* getHdri() const;

	void setHdri(HDRI* hdri);

private:
	Camera& renderCamera;
	Camera	viewCamera;

	std::vector<Color> colorBuffer;

	HittableManager hittableManager;

	MaterialManager materialManager;

	HDRI* hdri;
	
	int selectedObject = -1;
};