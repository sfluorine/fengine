#pragma once

#include <app.h>
#include <fecs.h>
#include <window_sdl.h>

#include <shader/shader.h>

#include <glm/glm.hpp>

struct quad_2d_t {
    glm::vec2 position;
    glm::vec2 dimension;
};

inline quad_2d_t make_quad(glm::vec2 position, glm::vec2 dimension)
{
    return { .position = position, .dimension = dimension };
}

struct quad_vertex_t {
    glm::vec2 position;
    glm::vec2 uv;
};

struct render_data_2d_t {
    shader_t shader;

    uint32_t quad_vao;
    uint32_t quad_vbo;
    uint32_t quad_ebo;

    quad_vertex_t* quad_vertices;
    quad_vertex_t* quad_vertices_ptr;

    uint32_t quad_index_count { 0 };
};

class renderer_2d_t : public plugin_t {
public:
    renderer_2d_t(window_sdl_t window);

    virtual ~renderer_2d_t() override = default;

    virtual PluginResult build(app_t* app) override;

    static SystemResult setup(registry_t rg);

    static SystemResult begin_drawing(registry_t rg, float);

    static SystemResult fetch_quads(registry_t rg, float);

    static SystemResult end_drawing(registry_t rg, float);

    static SystemResult shutdown(registry_t rg);

private:
    window_sdl_t m_window;
};