#ifndef PERLIN_H
#define PERLIN_H

class perlin {
public:
    perlin() {
        for (int i = 0; i < grid_size; i++) {
            gradient[i] = unit_vector(vec3::random(-1, 1));
        }

        generate_permutation(perm_x);
        generate_permutation(perm_y);
        generate_permutation(perm_z);
    }

    double noise(const point3& p) const {
        auto u = p.x() - std::floor(p.x());
        auto v = p.y() - std::floor(p.y());
        auto w = p.z() - std::floor(p.z());

        auto i = int(std::floor(p.x()));
        auto j = int(std::floor(p.y()));
        auto k = int(std::floor(p.z()));

        vec3 corners[2][2][2];

        for (int di = 0; di < 2; di++)
            for (int dj = 0; dj < 2; dj++)
                for (int dk = 0; dk < 2; dk++)
                    corners[di][dj][dk] = gradient[
                        perm_x[(i + di) & 255] ^
                        perm_y[(j + dj) & 255] ^
                        perm_z[(k + dk) & 255]
                    ];

        return trilinear_interp(corners, u, v, w);
    }

    double turbulence(const point3& p, int octaves) const {
        auto total = 0.0;
        auto temp_p = p;
        auto amplitude = 1.0;

        for (int i = 0; i < octaves; i++) {
            total += amplitude * noise(temp_p);
            amplitude *= 0.5;
            temp_p *= 2;
        }

        return std::fabs(total);
    }

private:
    static const int grid_size = 256;
    vec3 gradient[grid_size];
    int perm_x[grid_size];
    int perm_y[grid_size];
    int perm_z[grid_size];

    static void generate_permutation(int* perm) {
        for (int i = 0; i < grid_size; i++)
            perm[i] = i;

        shuffle_array(perm, grid_size);
    }

    static void shuffle_array(int* arr, int n) {
        for (int i = n - 1; i > 0; i--) {
            int swap_idx = random_int(0, i);
            int temp = arr[i];
            arr[i] = arr[swap_idx];
            arr[swap_idx] = temp;
        }
    }

    static double trilinear_interp(const vec3 corners[2][2][2], double u, double v, double w) {
        // Hermite smoothing
        auto su = u * u * (3 - 2 * u);
        auto sv = v * v * (3 - 2 * v);
        auto sw = w * w * (3 - 2 * w);

        auto sum = 0.0;

        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++) {
                    vec3 weight(u - i, v - j, w - k);
                    sum += (i * su + (1 - i) * (1 - su))
                         * (j * sv + (1 - j) * (1 - sv))
                         * (k * sw + (1 - k) * (1 - sw))
                         * dot(corners[i][j][k], weight);
                }

        return sum;
    }
};

#endif