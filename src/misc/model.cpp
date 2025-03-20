#include <glad/glad.h>

#include <tiny_obj_loader.h>

#include <optional>
#include <print>
#include <unordered_map>

#include "model.h"

namespace std
{

template<>
struct hash<vertex_t> {
    size_t operator()(const vertex_t& vertex) const
    {
        size_t h1 = hash<float> {}(vertex.position.x)
            ^ (hash<float> {}(vertex.position.y) << 1)
            ^ (hash<float> {}(vertex.position.z) << 2);
        size_t h2 = hash<float> {}(vertex.normal.x)
            ^ (hash<float> {}(vertex.normal.y) << 3)
            ^ (hash<float> {}(vertex.normal.z) << 4);
        size_t h3
            = hash<float> {}(vertex.uv.x) ^ (hash<float> {}(vertex.uv.y) << 5);
        return h1 ^ h2 ^ h3;
    }
};

}

std::optional<model_t> load_model(const fs::path& path)
{
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path)) {
        if (!reader.Error().empty()) {
            std::println(stderr,
                         "ERROR: failed to load model '{}': {}",
                         path.string(),
                         reader.Error());
            return {};
        }
    }

    if (!reader.Warning().empty()) {
        std::println(stderr,
                     "WARNING: from tinyobjloader in '{}': {}",
                     path.string(),
                     reader.Warning());
    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    const auto& materials = reader.GetMaterials();

    struct mesh_data_t {
        std::vector<vertex_t> vertices;
        std::vector<uint32_t> indices;
    };

    std::unordered_map<uint32_t, mesh_data_t> material_meshes;
    std::unordered_map<vertex_t, uint32_t> vertex_to_index;

    for (int32_t s = 0; s < shapes.size(); s++) {
        int32_t index_offset = 0;

        for (int32_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int32_t fv = shapes[s].mesh.num_face_vertices[f];
            int32_t mat_id = shapes[s].mesh.material_ids[f];

            if (!material_meshes.contains(mat_id))
                material_meshes[mat_id] = mesh_data_t {};

            auto& mesh = material_meshes[mat_id];

            for (int32_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                vertex_t vertex;
                vertex.position
                    = glm::vec3(attrib.vertices[3 * idx.vertex_index + 0],
                                attrib.vertices[3 * idx.vertex_index + 1],
                                attrib.vertices[3 * idx.vertex_index + 2]);

                if (idx.normal_index >= 0) {
                    vertex.normal
                        = glm::vec3(attrib.normals[3 * idx.normal_index + 0],
                                    attrib.normals[3 * idx.normal_index + 1],
                                    attrib.normals[3 * idx.normal_index + 2]);
                }

                if (idx.texcoord_index >= 0) {
                    vertex.uv = glm::vec2(
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]);
                }

                if (!vertex_to_index.contains(vertex)) {
                    uint32_t index = mesh.vertices.size();
                    vertex_to_index[vertex] = index;
                    mesh.vertices.push_back(vertex);
                }

                mesh.indices.push_back(vertex_to_index[vertex]);
            }

            index_offset += fv;
        }
    }

    model_t model;

    int32_t mat_index = 0;
    for (auto& [mat_id, mesh_data] : material_meshes) {
        mesh_t mesh;

        mesh.vertices = std::move(mesh_data.vertices);
        mesh.indices = std::move(mesh_data.indices);

        glGenVertexArrays(1, &mesh.vao);
        glBindVertexArray(mesh.vao);

        glGenBuffers(1, &mesh.vbo);
        glGenBuffers(1, &mesh.ebo);

        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(vertex_t) * mesh.vertices.size(),
                     mesh.vertices.data(),
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(uint32_t) * mesh.indices.size(),
                     mesh.indices.data(),
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex_t),
            reinterpret_cast<const void*>(offsetof(vertex_t, position)));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex_t),
            reinterpret_cast<const void*>(offsetof(vertex_t, normal)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex_t),
            reinterpret_cast<const void*>(offsetof(vertex_t, uv)));

        glBindVertexArray(0);

        mesh.mat_index = mat_index++;

        material_t material;
        material.diff_color = glm::vec3(materials[mat_id].diffuse[0],
                                        materials[mat_id].diffuse[1],
                                        materials[mat_id].diffuse[2]);
        material.diff_texture = 0;

        model.materials.push_back(material);
        model.meshes.push_back(std::move(mesh));
    }

    return model;
}
