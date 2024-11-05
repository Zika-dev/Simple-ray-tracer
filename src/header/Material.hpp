#pragma once
#include "utils.hpp"
#include <string>

struct hitRecord;

enum class MaterialType {
	LAMBERTIAN,
	SPECULAR,
	REFRACTIVE,
	UNKNOWN
};

class Material {
public:
	virtual ~Material() = default;

	virtual bool scatter(const Ray& rayIn, const hitRecord& record, Color& attenuation, Ray& scattered) const {
		return false;
	}

	virtual void changeAlbedo(const Color& newAlbedo) {
		APP_LOG_WARN("Albedo change not supported for this material");
	}

	virtual void changeFuzz(double newFuzz) {
		APP_LOG_WARN("Fuzz change not supported for this material");
	}

	virtual void changeRefractionIndex(double newRefractionIndex) {
		APP_LOG_WARN("Refraction index change not supported for this material");
	}

	virtual void setName(const std::string& newName) {
		APP_LOG_WARN("Name change not supported for this material");
	}

	virtual std::string getName() {
		APP_LOG_WARN("Name retrieval not supported for this material");
		return "Unknown";
	}

	virtual Color getAlbedo() {
		APP_LOG_WARN("Albedo retrieval not supported for this material");
		return Color(0.0, 0.0, 0.0);
	}

	virtual float getFuzz() {
		APP_LOG_WARN("Fuzz retrieval not supported for this material");
		return 0.0f;
	}


	virtual float getRefractionIndex() {
		APP_LOG_WARN("Refraction index retrieval not supported for this material");
		return 0.0f;
	}

	virtual MaterialType getType() {
		APP_LOG_WARN("Type retrieval not supported for this material");
		return MaterialType::UNKNOWN;
	}
};

class Lambertian : public Material {
	public:
		Lambertian(std::string name ,const Color& albedo) : name(name), albedo(albedo) {}

		bool scatter(const Ray& rayIn, const hitRecord& record, Color& attenuation, Ray& scattered) const override {


			vec3 scatterDirection = record.normal + randomUnitVector(); // Random unit vector with Lambertian approximation

			if (nearZero(scatterDirection)) { // If scatterDirection is near zero, then scatterDirection is opposite of normal
				scatterDirection = record.normal; // This is to prevent NaN values
			}

			scattered = Ray(record.point, scatterDirection);
			attenuation = albedo;
			return true;
		}

		void changeAlbedo(const Color& newAlbedo) override {
			albedo = newAlbedo;
		}

		void setName(const std::string& newName) override {
			name = newName;
		}

		std::string getName() override {
			return name;
		}

		Color getAlbedo() override {
			return albedo;
		}

		MaterialType getType() override {
			return MaterialType::LAMBERTIAN;
		}

	private:
		std::string name;
		Color albedo;
};

class Specular : public Material {
	public:
		Specular(std::string name, const Color& albedo, double fuzz) : name(name), albedo(albedo), fuzz(fuzz) {}

		bool scatter(const Ray& rayIn, const hitRecord& record, Color& attenuation, Ray& scattered) const override {
			vec3 reflected = reflect(unitVector(rayIn.direction), record.normal);

			reflected = unitVector(reflected) + (fuzz * randomUnitVector()); // Fuzziness

			scattered = Ray(record.point, reflected);

			attenuation = albedo;
			return (dotProduct(scattered.direction, record.normal) > 0);
		}

		void changeAlbedo(const Color& newAlbedo) override {
			albedo = newAlbedo;
		}

		void changeFuzz(double newFuzz) override {
			fuzz = newFuzz;
		}

		void setName(const std::string& newName) override {
			name = newName;
		}

		std::string getName() override {
			return name;
		}

		Color getAlbedo() override {
			return albedo;
		}

		float getFuzz() override {
			return fuzz;
		}

		MaterialType getType() override {
			return MaterialType::SPECULAR;
		}

	private:
		std::string name;
		Color albedo;
		float fuzz;
};

class Refractive : public Material {
	public:
		Refractive(std::string name ,double refractionIndex) : name(name), refractionIndex(refractionIndex) {}

		bool scatter(const Ray& rayIn, const hitRecord& record, Color& attenuation, Ray& scattered) const override {
			attenuation = Color(1.0, 1.0, 1.0); // Glass does not attenuate light
			double refractionRatio = record.frontFace ? (1.0 / refractionIndex) : refractionIndex; // If frontFace is false we are exiting the object

			vec3 unitDirection = unitVector(rayIn.direction);

			double cosTheta = std::fmin(dotProduct(-unitDirection, record.normal), 1.0); // Cosine of the angle between the ray and the normal
			double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta); // Sine of the angle between the ray and the normal

			bool cannotRefract = refractionRatio * sinTheta > 1.0; // Total internal reflection

			vec3 direction;

			if (cannotRefract || reflectance(cosTheta, refractionIndex) > randomDouble()) {
				direction = reflect(unitDirection, record.normal);
			}
			else
			{
				direction = refract(unitDirection, record.normal, refractionRatio);
			}

			scattered = Ray(record.point, direction);

			return true;
		}

		void changeRefractionIndex(double newRefractionIndex) override {
			APP_LOG_INFO("Refraction index changed to {0}", newRefractionIndex);
			refractionIndex = newRefractionIndex;
		}

		void setName(const std::string& newName) override {
			name = newName;
		}

		std::string getName() override {
			return name;
		}

		float getRefractionIndex() override {
			return refractionIndex;
		}

		MaterialType getType() override {
			return MaterialType::REFRACTIVE;
		}

	private:
		std::string name;
		float refractionIndex; // Refraction index in vacuum or the ratio of the material's refractive index over the enclosing medium's refractive index

		static double reflectance(double cosine, double refraction_index) { // Schlick's approximation
			auto r0 = (1 - refraction_index) / (1 + refraction_index);
			r0 = pow(r0, 2);
			return r0 + (1 - r0) * std::pow((1 - cosine), 5);
		}
};