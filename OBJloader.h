#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "vec3.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


static std::vector<point3> load_obj(const char* filename) {
    // Vertices for locations
    std::vector<point3> positions;
    std::vector<point3> normals;

    // Faces
    std::vector<int> vertex_position_indices;
    std::vector<int> vertex_normal_indices;

    // All vertices
    std::vector<point3> vertices;

    std::ifstream ifs(filename);
    std::string line = "";

    if (!ifs.is_open()) {
        throw std::invalid_argument("Cannot open file");
    }

    while (std::getline(ifs, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") { //vertex
            vec3 temp_vec3;
            ss >> temp_vec3.e[0] >> temp_vec3.e[1] >> temp_vec3.e[2];
            positions.push_back(temp_vec3);
        }
        else if (prefix == "vn") { // vertex normal
            vec3 temp_vec3;
            ss >> temp_vec3.e[0] >> temp_vec3.e[1] >> temp_vec3.e[2];
            normals.push_back(temp_vec3);
        }
        else if (prefix == "f") { //face
            // Face formats
            // f v1 v2 v3
            // f v1/vt1 v2/vt2 v3/vt3
            // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
            // f v1//vn1 v2//vn2 v3//vn3

            std::string vertex_data;
            std::vector<int> face_indices;

            while (ss >> vertex_data) {
                // Replace slashes with spaces
                for (auto& c : vertex_data) {
                    if (c == '/') c = ' ';
                }

                std::stringstream vertex_ss(vertex_data);
                int pos_idx = 0;
                vertex_ss >> pos_idx;

                face_indices.push_back(pos_idx - 1);
            }

            // Triangulate
            // Connecting each vertex starting from the first one in
            for (size_t i = 1; i + 1 < face_indices.size(); i++) {
                vertex_position_indices.push_back(face_indices[0]);
                vertex_position_indices.push_back(face_indices[i]);
                vertex_position_indices.push_back(face_indices[i + 1]);
            }
        }
    }

    ifs.close();

    for (int idx : vertex_position_indices) {
        if (idx >= 0 && idx < static_cast<int>(positions.size())) {
            vertices.push_back(positions[idx]);
        }
    }

    return vertices;
}

#endif