#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    point3 center;
    double radius;

    sphere() {}
    sphere(point3 cen, double r) : center(cen), radius(r) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        vec3 oc = r.origin() - center;
        auto a = dot(r.direction(), r.direction());
        auto half_b = dot(oc, r.direction());
        auto c = dot(oc, oc) - radius*radius;
        auto discriminant = half_b*half_b - a*c;

        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        rec.normal = (rec.p - center) / radius;

        return true;
    }
};

#endif
