#include "renderer_2d.h"
#include "fecs.h"
#include "window_sdl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static constexpr uint32_t MAX_QUAD_COUNT = 12000;
static constexpr uint32_t MAX_QUAD_VERTICES = MAX_QUAD_COUNT * 4;
static constexpr uint32_t MAX_QUAD_INDICES = MAX_QUAD_COUNT * 6;

static SystemResult init(render_data_2d_t* rd, window_creation_info_t info)
{
    if (auto result = rd->shader.load_shader("resources/shaders/basic.qsh");
        !result) {
        return std::unexpected(result.error());
    }

    auto projection = glm::ortho(0.0f,
                                 static_cast<float>(info.width),
                                 static_cast<float>(info.height),
                                 0.0f);
    rd->shader.bind();

    glUniformMatrix4fv(
        glGetUniformLocation(rd->shader.get_id(), "u_projection"),
        1,
        GL_FALSE,
        glm::value_ptr(projection));

    glGenVertexArrays(1, &rd->quad_vao);
    glBindVertexArray(rd->quad_vao);

    glGenBuffers(1, &rd->quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, rd->quad_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 MAX_QUAD_VERTICES * sizeof(quad_vertex_t),
                 nullptr,
                 GL_DYNAMIC_DRAW);

    uint32_t offset = 0;
    uint32_t indices[MAX_QUAD_INDICES];
    for (uint32_t i = 0; i < MAX_QUAD_INDICES; i += 6) {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;
        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;
        offset += 4;
    }

    glGenBuffers(1, &rd->quad_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rd->quad_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(quad_vertex_t),
        reinterpret_cast<const void*>(offsetof(quad_vertex_t, position)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(quad_vertex_t),
        reinterpret_cast<const void*>(offsetof(quad_vertex_t, uv)));

    glBindVertexArray(0);

    rd->quad_vertices = new quad_vertex_t[MAX_QUAD_VERTICES];
    rd->quad_vertices_ptr = rd->quad_vertices;

    return {};
}

static void drawing_start(render_data_2d_t* rd)
{
    rd->quad_index_count = 0;
    rd->quad_vertices_ptr = rd->quad_vertices;
}

static void drawing_end(render_data_2d_t* rd)
{
    if (rd->quad_index_count) {
        auto size = (rd->quad_vertices_ptr - rd->quad_vertices)
            * sizeof(quad_vertex_t);

        glBindBuffer(GL_ARRAY_BUFFER, rd->quad_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, rd->quad_vertices);

        rd->shader.bind();
        glBindVertexArray(rd->quad_vao);

        glDrawElements(
            GL_TRIANGLES, rd->quad_index_count, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
    }
}

renderer_2d_t::renderer_2d_t(window_sdl_t window)
    : m_window(std::move(window))
{
}

PluginResult renderer_2d_t::build(app_t* app)
{
    if (auto result = m_window.build(app); !result)
        return result;

    auto rg = app->get_registry();
    rg.spawn_resource<window_creation_info_t>(m_window.get_creation_info());

    app->add_system(make_startup(setup));

    app->add_system(make_update(begin_drawing));
    app->add_system(make_update(fetch_quads));
    app->add_system(make_update(end_drawing));

    app->add_system(make_shutdown(shutdown));

    return {};
}

SystemResult renderer_2d_t::setup(registry_t rg)
{
    auto info = rg.get_resource<window_creation_info_t>();

    render_data_2d_t rd;
    if (auto result = init(&rd, info); !result)
        return result;

    glViewport(0, 0, info.width, info.height);

    rg.spawn_resource<render_data_2d_t>(std::move(rd));
    return {};
}

SystemResult renderer_2d_t::begin_drawing(registry_t rg, float)
{
    auto& sdl_context = rg.get_resource<sdl_context_t>();
    auto& rd = rg.get_resource<render_data_2d_t>();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawing_start(&rd);

    return {};
}

SystemResult renderer_2d_t::fetch_quads(registry_t rg, float)
{
    auto& rd = rg.get_resource<render_data_2d_t>();

    auto view = rg.get_view<quad_2d_t>();
    for (const auto& entity : view) {
        const auto& [position, dimension] = view.get<quad_2d_t>(entity);

        if (rd.quad_index_count >= MAX_QUAD_INDICES) {
            drawing_end(&rd);
            drawing_start(&rd);
        }

        rd.quad_vertices_ptr->position = glm::vec2(position.x, position.y);
        rd.quad_vertices_ptr++;

        rd.quad_vertices_ptr->position
            = glm::vec2(position.x, position.y + dimension.y);
        rd.quad_vertices_ptr++;

        rd.quad_vertices_ptr->position
            = glm::vec2(position.x + dimension.x, position.y + dimension.y);
        rd.quad_vertices_ptr++;

        rd.quad_vertices_ptr->position
            = glm::vec2(position.x + dimension.x, position.y);
        rd.quad_vertices_ptr++;

        rd.quad_index_count += 6;
    }

    return {};
}

SystemResult renderer_2d_t::end_drawing(registry_t rg, float)
{
    auto& sdl_context = rg.get_resource<sdl_context_t>();
    auto& rd = rg.get_resource<render_data_2d_t>();

    drawing_end(&rd);
    SDL_GL_SwapWindow(sdl_context.window);

    return {};
}

SystemResult renderer_2d_t::shutdown(registry_t rg)
{
    auto& render_data = rg.get_resource<render_data_2d_t>();

    delete render_data.quad_vertices;

    glDeleteBuffers(1, &render_data.quad_ebo);
    glDeleteBuffers(1, &render_data.quad_vbo);
    glDeleteVertexArrays(1, &render_data.quad_vao);

    glDeleteProgram(render_data.shader.get_id());

    return {};
}
