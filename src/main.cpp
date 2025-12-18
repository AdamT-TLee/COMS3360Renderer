#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"
#include "constant_medium.h"
#include "OBJloader.h"

void bouncing_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 10;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    cam.render(world);
}

void checkered_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));

    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(hittable_list(globe));
}

void perlin_spheres() {
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void quads() {
    hittable_list world;

    // Materials
    auto left_red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));
    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));

    // Quads
    world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
    world.add(make_shared<triangle>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), make_shared<lambertian>(checker)));
    world.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 80;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void simple_light() {
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);
    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));
    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
    world.add(make_shared<sphere>(point3(0, 7, 0), 2, difflight));
    world.add(make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), difflight));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 20;
    cam.lookfrom = point3(26, 3, 6);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void cornell_box() {
    hittable_list world;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));


    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    world.add(box1);

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    world.add(box2);


    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 10;
    cam.max_depth = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void cornell_smoke() {
    hittable_list world;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0), vec3(0, 0, 305), light));
    world.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    world.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
    world.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void final_scene(int image_width, int samples_per_pixel, int max_depth) {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    world.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    world.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.2);
    world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
    }

    world.add(make_shared<translate>(
            make_shared<rotate_y>(
                make_shared<bvh_node>(boxes2), 15),
            vec3(-100, 270, 395)
        )
    );

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = max_depth;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(478, 278, -600);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void test_obj_loader() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    try {
        std::vector<point3> vertices = load_obj("./cloud.obj");
        std::clog << "Loaded " << vertices.size() << " vertices from OBJ file\n";
        std::clog << "Number of triangles: " << vertices.size() / 3 << "\n";

        auto obj_material = make_shared<lambertian>(color(.65, .05, .05));

        for (size_t i = 0; i + 2 < vertices.size(); i += 3) {
            point3 v0 = vertices[i];
            point3 v1 = vertices[i + 1];
            point3 v2 = vertices[i + 2];

            vec3 u = v1 - v0;
            vec3 v = v2 - v0;

            world.add(make_shared<triangle>(v0, u, v, obj_material));
        }
    } catch (const std::exception &e) {
        std::cerr << "Error loading OBJ file: " << e.what() << "\n";
        return;
    }
    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 50;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 40;
    cam.lookfrom = point3(0, 1, 3);
    cam.lookat = point3(0, 1, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

//DEMO FEATURE REDNDERS FOR FINAL
//Scene 1: Depth of field
void depth_of_field_demo() {
    hittable_list world;

    auto ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto red = make_shared<lambertian>(color(0.8, 0.2, 0.2));
    auto blue = make_shared<lambertian>(color(0.2, 0.2, 0.8));
    auto green = make_shared<lambertian>(color(0.2, 0.8, 0.2));

    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));

    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, red));
    world.add(make_shared<sphere>(point3(-4, 1, -7), 1.0, blue));
    world.add(make_shared<sphere>(point3(3, 1, 4), 1.0, green));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 30;
    cam.lookfrom = point3(0, 2, 12);
    cam.lookat = point3(0, 1, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 2;
    cam.focus_dist = 12.0;

    cam.render(world);
}

// Scene 2: Configurable Camera Demo
void camera_fov_demo() {
    hittable_list world;

    auto ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto metal_mat = make_shared<metal>(color(0.8, 0.6, 0.2), 0.3);
    auto red = make_shared<lambertian>(color(0.8, 0.2, 0.2));

    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));

    world.add(make_shared<sphere>(point3(-2, 1, 0), 1.0, metal_mat));
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, red));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 50;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.lookfrom = point3(-1, 8, 0);
    cam.lookat = point3(0, 1, 0);
    cam.vup = vec3(0, 0, -1);

    cam.defocus_angle = 0;

    cam.render(world);
}

// Scene 3: Motion Blur
void motion_blur_demo() {
    hittable_list world;

    auto ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto moving_mat = make_shared<lambertian>(color(0.8, 0.2, 0.2));
    auto static_mat = make_shared<metal>(color(0.7, 0.7, 0.9), 0.1);

    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));

    auto center1 = point3(-2, 1, 0);
    auto center2 = center1 + vec3(0, 0.5, 0);
    world.add(make_shared<sphere>(center1, center2, 0.7, moving_mat));

    world.add(make_shared<sphere>(point3(2, 1, 0), 0.7, static_mat));
    world.add(make_shared<sphere>(point3(0, 0.5, -2), 0.5, static_mat));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 40;
    cam.lookfrom = point3(0, 2, 8);
    cam.lookat = point3(0, 1, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

// Scene 5: All Materials Demo and Perlin Noise
void materials_showcase() {
    hittable_list world;

    auto perlin = make_shared<noise_texture>(4);
    auto ground_mat = make_shared<lambertian>(perlin);
    auto lambertian_mat = make_shared<lambertian>(color(0.8, 0.3, 0.3));
    auto metal_mat = make_shared<metal>(color(0.8, 0.8, 0.9), 0.1);
    auto metal_fuzzy = make_shared<metal>(color(0.9, 0.7, 0.5), 0.5);
    auto glass_mat = make_shared<dielectric>(1.5);
    auto light_mat = make_shared<diffuse_light>(color(4, 4, 4));

    //Perlin Noise for ground
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_mat));

    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, glass_mat)); // Glass
    world.add(make_shared<sphere>(point3(-2, 1, 0), 1.0, metal_mat)); // Metal
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, lambertian_mat)); // Lambertian
    world.add(make_shared<sphere>(point3(2, 1, 0), 1.0, metal_fuzzy)); // Metal
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, light_mat)); // Light

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.05, 0.05, 0.05);

    cam.vfov = 40;
    cam.lookfrom = point3(0, 2, 12);
    cam.lookat = point3(0, 1, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void ray_tracer_final_image() {
    hittable_list world;

    // Ground
    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0, -1000, 0),1000,make_shared<lambertian>(checker)));

    // OBJ model - cloud
    try {
        std::vector<point3> vertices = load_obj("./cloud.obj");
        std::clog << "Loaded " << vertices.size() << " vertices from OBJ file\n";
        std::clog << "Number of triangles: " << vertices.size() / 3 << "\n";

        auto obj_material = make_shared<lambertian>(color(.65, .05, .05));

        for (size_t i = 0; i + 2 < vertices.size(); i += 3) {
            point3 v0 = vertices[i];
            point3 v1 = vertices[i + 1];
            point3 v2 = vertices[i + 2];

            vec3 u = v1 - v0;
            vec3 v = v2 - v0;

            world.add(make_shared<triangle>(v0, u, v, obj_material));
        }
    } catch (const std::exception &e) {
        std::cerr << "Error loading OBJ file: " << e.what() << "\n";
        return;
    }

    // Smoke sphere - Behind
    auto smoke_boundary = make_shared<sphere>(point3(0, 1.5, -3),1.5,make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(smoke_boundary,0.5,color(0.9, 0.9, 0.9)));

    // Earth sphere - behind and left
    auto earth_tex = make_shared<image_texture>("earthmap.jpg");
    auto earth_mat = make_shared<lambertian>(earth_tex);
    world.add(make_shared<sphere>(point3(-3, 1.5, -2),1.2,earth_mat));

    // Perlin noise sphere - behind and right
    auto perlin_tex = make_shared<noise_texture>(3.0);
    world.add(make_shared<sphere>(point3(3, 1.5, -2),1.2, make_shared<lambertian>(perlin_tex)));

    // Bouncing sphere in front
    auto bounce_mat = make_shared<lambertian>(color(0.8, 0.2, 0.2));
    point3 bounce_start(-1, 0.6, 1.5);
    point3 bounce_end = bounce_start + vec3(0, 0.6, 0);
    world.add(make_shared<sphere>(bounce_start,bounce_end,0.5,bounce_mat));

    // Light
    auto light = make_shared<diffuse_light>(color(6, 6, 6));
    world.add(make_shared<quad>(point3(-2, 5, 2),vec3(4, 0, 0),vec3(0, 0, -4),light));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 50;
    cam.max_depth = 50;
    cam.background = color(0.05, 0.05, 0.08);

    cam.vfov = 40;
    cam.lookfrom = point3(0, 2, 6);
    cam.lookat = point3(0, 1, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}


int main() {
    switch (15) {
        case 1: bouncing_spheres();
            break;
        case 2: checkered_spheres();
            break;
        case 3: earth();
            break;
        case 4: perlin_spheres();
            break;
        case 5: quads();
            break;
        case 6: simple_light();
            break;
        case 7: cornell_box();
            break;
        case 8: cornell_smoke();
            break;
        case 9: final_scene(800, 400, 40);
            break;
        case 10: test_obj_loader();
            break;

        //Feature demos for class
        case 11: depth_of_field_demo();
            break;
        case 12: camera_fov_demo();
            break; // Shows configurable FOV
        case 13: motion_blur_demo();
            break; // Shows motion blur
        case 14: materials_showcase();
            break; // Shows all materials + perlin
        case 15: ray_tracer_final_image();
            break;


        default: final_scene(400, 250, 4);
            break;
    }
}
