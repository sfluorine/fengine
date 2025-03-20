#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <filesystem>

namespace fs = std::filesystem;

struct material_t {
    glm::vec3 diff_color;
    uint32_t diff_texture;
};

struct vertex_t {
    glm::vec3 position {};
    glm::vec3 normal {};
    glm::vec2 uv {};

    bool operator==(const vertex_t& other) const
    {
        auto pos_result = position.x == other.position.x
            && position.y == other.position.y && position.z == other.position.z;
        auto norm_result = normal.x == other.normal.x
            && normal.y == other.normal.y && normal.z == other.normal.z;
        auto uv_result = uv.x == other.uv.x && uv.y == other.uv.y;

        return pos_result && norm_result && uv_result;
    }
};

struct mesh_t {
    mesh_t()
    {
    }

    mesh_t(std::vector<vertex_t> vertices,
           std::vector<uint32_t> indices,
           uint32_t vao,
           uint32_t vbo,
           uint32_t ebo,
           int32_t mat_index)
        : vertices(std::move(vertices))
        , indices(std::move(indices))
        , vao(vao)
        , vbo(vbo)
        , ebo(ebo)
        , mat_index(mat_index)
    {
    }

    mesh_t(mesh_t&& other)
        : vertices(std::move(other.vertices))
        , indices(std::move(other.indices))
        , vao(other.vao)
        , vbo(other.vbo)
        , ebo(other.ebo)
        , mat_index(other.mat_index)
    {
        other.vao = 0;
        other.vbo = 0;
        other.ebo = 0;
        other.mat_index = 0;
    }

    mesh_t(const mesh_t& other) = delete;

    mesh_t& operator=(mesh_t&& other)
    {
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);

        vao = other.vao;
        vbo = other.vbo;
        ebo = other.ebo;
        mat_index = other.mat_index;

        other.vao = 0;
        other.vbo = 0;
        other.ebo = 0;
        other.mat_index = 0;

        return *this;
    }

    mesh_t& operator=(const mesh_t& other) = delete;

    std::vector<vertex_t> vertices;
    std::vector<uint32_t> indices;

    uint32_t vao {};
    uint32_t vbo {};
    uint32_t ebo {};

    int32_t mat_index {};
};

struct model_t {
    model_t()
    {
    }

    model_t(model_t&& other)
        : meshes(std::move(other.meshes))
        , materials(std::move(other.materials))
    {
    }

    model_t(const model_t& other) = delete;

    model_t& operator=(model_t&& other)
    {
        meshes = std::move(other.meshes);
        materials = std::move(other.materials);

        return *this;
    }

    model_t& operator=(const model_t&& other) = delete;

    std::vector<mesh_t> meshes;
    std::vector<material_t> materials;
};

std::optional<model_t> load_model(const fs::path& path);
