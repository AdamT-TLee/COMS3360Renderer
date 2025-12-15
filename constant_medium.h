#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
public:
    constant_medium(shared_ptr<hittable> boundary, double density, shared_ptr<texture> tex)
      : boundary(boundary), neg_inv_density(-1.0/density),
        phase_mat(make_shared<isotropic>(tex))
    {}

    constant_medium(shared_ptr<hittable> boundary, double density, const color& albedo)
      : boundary(boundary), neg_inv_density(-1.0/density),
        phase_mat(make_shared<isotropic>(albedo))
    {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record entry_rec, exit_rec;

        // Find where ray enters the boundary
        if (!boundary->hit(r, interval::universe, entry_rec))
            return false;

        // Find where ray exits the boundary
        if (!boundary->hit(r, interval(entry_rec.t + 0.0001, infinity), exit_rec))
            return false;

        // Clamp to valid ray interval
        if (entry_rec.t < ray_t.min) entry_rec.t = ray_t.min;
        if (exit_rec.t > ray_t.max) exit_rec.t = ray_t.max;

        if (entry_rec.t >= exit_rec.t)
            return false;

        if (entry_rec.t < 0)
            entry_rec.t = 0;

        auto ray_len = r.direction().length();
        auto dist_in_medium = (exit_rec.t - entry_rec.t) * ray_len;
        auto scatter_dist = neg_inv_density * std::log(random_double());

        if (scatter_dist > dist_in_medium)
            return false;

        rec.t = entry_rec.t + scatter_dist / ray_len;
        rec.p = r.at(rec.t);
        rec.normal = vec3(1, 0, 0);  // arbitrary direction
        rec.front_face = true;
        rec.mat = phase_mat;

        return true;
    }

    aabb bounding_box() const override { return boundary->bounding_box(); }

private:
    shared_ptr<hittable> boundary;
    double neg_inv_density;
    shared_ptr<material> phase_mat;
};

#endif