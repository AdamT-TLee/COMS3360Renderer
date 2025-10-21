#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "vec3.h"

class quad : public hittable {
public:
    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
      : Q(Q), u(u), v(v), mat(mat)
    {
        vec3 n = cross(u, v);
        normal = unit_vector(n);
        D = dot(Q, normal);
        w = n / dot(n,n);
        set_bounding_box();
    }

    virtual void set_bounding_box() {
        // Compute the bounding box of all four vertices.
        auto bbox_diagonal1 = aabb(Q, Q + u + v);
        auto bbox_diagonal2 = aabb(Q + u, Q + v);
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override { return bbox; }


    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        //n * d
        double denom = dot(r.direction(), normal);

        //if parallel to the ray
        // How the book does it probably cleaner
        if (std::fabs(denom) < 1e-8)
            return false;

        // Return false if the hit point parameter t is outside the ray interval.
        //t = (D - n * P)/ (n * d)
        double t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t)) {
            return false;
        }

        // Determine if the hit point lies within the planar shape using its plane coordinates.
        auto intersection = r.at(t);
        vec3 planar_hitpt_vector = intersection - Q; // p = P - Q
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec)) {
            return false;
        }

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const {
        interval criteria = interval(0,1);

        if(!criteria.contains(a) || !criteria.contains(b)) {
            return false;
        }

        rec.u = a;
        rec.v = b;
        return true;
    }

private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w;
};

class triangle : public quad {
public:
    triangle(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : quad(Q, u, v, mat) {}

    virtual bool is_interior(double a, double b, hit_record& rec) const override {
        if (a > 0 && b > 0 && a + b < 1) {
            rec.u = a;
            rec.v = b;
            return true;
        }

        return false;
    }
};

#endif