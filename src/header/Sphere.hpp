#pragma once
#include "Hittable.hpp"
#include "Log.hpp"
#include "Material.hpp"

class Sphere : public Hittable {
	public:
		Sphere(const Point3& center, double radius, std::shared_ptr<Material> material) : center(center), radius(radius), material(material) {
		
		}

		bool hit(const Ray& ray, Interval rayT, hitRecord& record) const override {
			vec3 worldPos = center - ray.origin; // Center of the sphere in world space

			// Quadratic equation
			double a = pow(ray.direction.length(), 2); // direction^2

			double h = dotProduct(ray.direction, worldPos); // direction * worldPos

			double c = pow(worldPos.length(), 2) - pow(radius, 2); // worldPos^2 - radius^2

			double discriminant = h * h - a * c; // h^2 - 4ac

			if (discriminant < 0) {
				return false;
			}

			double discriminantSqrt = sqrt(discriminant);
			double root = (h - discriminantSqrt) / a; // Closest intersection point

			if (!rayT.surrounds(root)) { // If the closest intersection point is not in the ray's range
				root = (h + discriminantSqrt) / a; // Farthest intersection point

				if (!rayT.surrounds(root)) { // If the farthest intersection point is not in the ray's range
					return false;
				}
			}

			// Set the hit record
			record.point = ray.at(root);
			record.t = root;
			
			vec3 outwardNormal = (record.point - center) / radius; // Normalized normal vector (Where the ray hits the sphere)
			record.setFaceNormal(ray, outwardNormal); // Set the normal vector and front face boolean
			record.material = material;

			return true;
		}

		double getRadius() const {
			return radius;
		}

		Point3 getCenter() const {
			return center;
		}

		std::shared_ptr<Material> getMaterial() const override {
			return material;
		}

		void setMaterial(std::shared_ptr<Material> newMaterial) override {
			material = newMaterial;
		}

		void changePosition(const Point3& position) {
			center = position;
		}

		void changeRadius(double newRadius) {
			if(newRadius <= 0) {
				newRadius = 0.01;
				return;
			}
			radius = newRadius;
		}

	private:
		double radius;
		Point3 center;
		std::shared_ptr<Material> material;
};