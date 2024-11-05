#include "HittableManager.hpp"
#include "Log.hpp"

HittableManager::HittableManager() {}

HittableManager::~HittableManager() {}

void HittableManager::add(std::shared_ptr<Hittable> hittable) {
	hittables.push_back(hittable);

	APP_LOG_INFO("Added hittable to manager");
}

void HittableManager::remove(int index) {
	hittables.erase(hittables.begin() + index);

	APP_LOG_INFO("Removed hittable from manager");
}

void HittableManager::clear() {
	hittables.clear();
}

bool HittableManager::hit(const Ray& ray, Interval rayT, hitRecord& rec) const {
	hitRecord tempRec;
	bool hitAnything = false;
	double closestSoFar = rayT.max;

	for (const auto& hittable : hittables) { // Iterate through all hittables
		if (hittable->hit(ray, Interval(rayT.min, closestSoFar), tempRec)) { // If the ray hits the hittable within the ray's range
			hitAnything = true;
			closestSoFar = tempRec.t; // Update the closest intersection point
			rec = tempRec; // Update the hit record
		}
	}

	return hitAnything;
}

std::shared_ptr<Hittable> HittableManager::get(int index) const {
	return hittables[index];
}

int HittableManager::size() const {
	return hittables.size();
}