#include "Scene.hpp"
#include "Log.hpp"

Scene::Scene(Camera& camera, HittableManager& hittableManager, MaterialManager& materialManager, HDRI* hdri) : renderCamera(camera), hittableManager(hittableManager), viewCamera(camera.getRenderer()), materialManager(materialManager), hdri(hdri) {

	viewCamera.aspectRatioComponents = renderCamera.aspectRatioComponents;
	viewCamera.width = 200;
	viewCamera.FOV = 60;
	viewCamera.cameraLookFrom = Point3(0, 1, 2);
	viewCamera.cameraLookAt = Point3(0, 0, -1);
	viewCamera.cameraUp = vec3(0, 1, 0);

	viewCamera.maxSamples = 10;
	viewCamera.maxDepth = 10;
	viewCamera.SSAA = false;
	viewCamera.shading = false;
	viewCamera.gammaCorrection = false;
	viewCamera.gamma = 2.2;
	viewCamera.varianceThreshold = 0.1;
	viewCamera.adaptiveSampling = true;
	viewCamera.adaptiveSamplingDepth = 2;
	viewCamera.denoise = false;

	camera.initialize();
	viewCamera.initialize();
}

std::vector<Color>& Scene::render() {

	colorBuffer.clear();

	renderCamera.FOV = viewCamera.FOV;
	renderCamera.cameraLookFrom = viewCamera.cameraLookFrom;
	renderCamera.cameraLookAt = viewCamera.cameraLookAt;
	renderCamera.cameraUp = viewCamera.cameraUp;

	colorBuffer = renderCamera.render(hittableManager, hdri, true);

	return colorBuffer;
}

std::vector<Color> Scene::renderView() {
	return viewCamera.render(hittableManager, hdri);
}

Camera& Scene::getRenderCamera() {
	return renderCamera;
}

Camera& Scene::getViewCamera() {
	return viewCamera;
}

std::vector<Color>& Scene::getColorBuffer() {
	return colorBuffer;
}

HittableManager& Scene::getHittableManager() {
	return hittableManager;
}

MaterialManager& Scene::getMaterialManager() {
	return materialManager;
}

void Scene::addHittable(std::shared_ptr<Hittable> hittable) {
	hittableManager.add(hittable);
}

int Scene::getInitialRayCount() const {
	int totalRayCount = 0;
	int resolution = renderCamera.getRenderSize().x * renderCamera.getRenderSize().y;

	if(renderCamera.SSAA)
		totalRayCount = resolution * renderCamera.maxSamples;
	else
		totalRayCount = resolution;

	return totalRayCount;
}

int Scene::getSelectedObject() const {
	return selectedObject;
}

void Scene::setSelectedObject(int selectedObject) {
	this->selectedObject = selectedObject;
}

HDRI* Scene::getHdri() const {
	return hdri;
}

void Scene::setHdri(HDRI* hdri) {
	this->hdri = hdri;
}