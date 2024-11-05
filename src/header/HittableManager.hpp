#pragma once
#include "Hittable.hpp"
#include <vector>
#include <memory>

class HittableManager {
public:
	HittableManager();
	~HittableManager();

	void add(std::shared_ptr<Hittable> hittable);

	void remove(int index);

	void clear();

	bool hit(const Ray& ray, Interval rayT, hitRecord& rec) const;

	std::shared_ptr<Hittable> get(int index) const;

	int size() const;

private:
	std::vector<std::shared_ptr<Hittable>> hittables;
};