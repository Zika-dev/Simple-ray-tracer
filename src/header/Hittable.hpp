#pragma once
#include "utils.hpp"
#include "Interval.hpp"

class Material;

using namespace Utils;

struct hitRecord {
	Point3 point;
	vec3 normal;
	std::shared_ptr<Material> material;
	double t;
	bool frontFace;

	hitRecord() : point(), normal(), t(0), frontFace(false) {}

	void setFaceNormal(const Ray& ray, const vec3& outwardNormal) {
		frontFace = dotProduct(ray.direction, outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

class Hittable {
	public:
		virtual bool hit(const Ray& ray, Interval rayT, hitRecord& rec) const = 0;
		virtual ~Hittable() = default;

		virtual void setMaterial(std::shared_ptr<Material> newMaterial) {
			APP_LOG_WARN("Material change not supported for this hittable");
		}

		virtual std::shared_ptr<Material> getMaterial() const {
			APP_LOG_WARN("Material retrieval not supported for this hittable");
			return nullptr;
		}
};