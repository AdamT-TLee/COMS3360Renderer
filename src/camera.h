#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"

class camera {
  public:
    double aspect_ratio      = 1.0;  // Image width over height ratio
    int    image_width       = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Number of random samples per pixel
    int    max_depth         = 10;   // Maximum ray bounce depth
    color  background;               // Scene background color

    double vfov = 90;                         // Vertical field of view
    point3 lookfrom = point3(0, 0, 0);        // Camera position
    point3 lookat   = point3(0, 0, -1);       // Point camera is looking at
    vec3   vup      = vec3(0, 1, 0);          // Camera-relative up direction

    double defocus_angle = 0;  // Defocus blur angle
    double focus_dist    = 10; // Distance to focus plane

    void render(const hittable& world) {
        init();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int row = 0; row < image_height; row++) {
            std::clog << "\rScanlines remaining: " << (image_height - row) << ' ' << std::flush;
            for (int col = 0; col < image_width; col++) {
                color pixel_color(0,0,0);
                for (int s = 0; s < samples_per_pixel; s++) {
                    ray r = generate_ray(col, row);
                    pixel_color += trace_ray(r, max_depth, world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }

        std::clog << "\rDone.                 \n";
    }

  private:
    int    image_height;         // Rendered image height
    double pixel_samples_scale;  // Color scaling factor for pixel samples
    point3 cam_center;           // Camera center point
    point3 first_pixel_loc;      // Location of top-left pixel
    vec3   pixel_delta_u;        // Horizontal offset between pixels
    vec3   pixel_delta_v;        // Vertical offset between pixels
    vec3   u, v, w;              // Camera coordinate frame basis vectors
    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius

    void init() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;
        cam_center = lookfrom;

        // Calculate viewport dimensions
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_h = 2.0 * h * focus_dist;
        auto viewport_w = viewport_h * (double(image_width)/image_height);

        // Calculate camera basis vectors
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate viewport edge vectors
        vec3 viewport_u = viewport_w * u;
        vec3 viewport_v = viewport_h * -v;

        // Calculate pixel deltas
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate upper left pixel location
        auto viewport_top_left = cam_center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        first_pixel_loc = viewport_top_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate defocus disk basis vectors
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray generate_ray(int i, int j) const {
        // Generate a camera ray for pixel at location i,j with random sampling

        auto offset = random_in_square();
        auto pixel_sample = first_pixel_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? cam_center : sample_defocus_disk();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 random_in_square() const {
        // Return random vector in the unit square centered at origin
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 sample_defocus_disk() const {
        // Sample a random point on the defocus disk
        auto p = random_in_unit_disk();
        return cam_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color trace_ray(const ray& r, int depth, const hittable& world) const {
        // Return black if we've exceeded the ray bounce limit
        if (depth <= 0)
            return color(0,0,0);

        hit_record rec;

        // If ray doesn't hit anything, return background color
        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        ray scattered;
        color attenuation;
        color color_emitted = rec.mat->emitted(rec.u, rec.v, rec.p);

        // If material doesn't scatter light, return only emitted color
        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_emitted;

        color color_scattered = attenuation * trace_ray(scattered, depth-1, world);

        return color_emitted + color_scattered;
    }
};

#endif