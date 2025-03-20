#include <app.h>
#include <renderer_2d.h>

struct velocity_t {
    glm::vec2 direction;
};

static inline velocity_t make_velocity(float x, float y)
{
    return { .direction = glm::vec2(x, y) };
}

static SystemResult sdl_event_handler(registry_t rg, float)
{
    auto app_state = rg.get_resource<app_state_t*>();

    SDL_Event event;
    while (SDL_WaitEventTimeout(&event, 1)) {
        if (event.type == SDL_EVENT_QUIT)
            app_state->running = false;

        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
            app_state->running = false;
    }

    return {};
}

static SystemResult setup(registry_t rg)
{
    rg.spawn_entity<quad_2d_t, velocity_t>(
        make_quad(glm::vec2(20.0f, 10.0f), glm::vec2(20.0f)),
        make_velocity(-500.0f, 500.0f));

    rg.spawn_entity<quad_2d_t, velocity_t>(
        make_quad(glm::vec2(70.0f, 10.0f), glm::vec2(20.0f)),
        make_velocity(500.0f, 500.0f));

    rg.spawn_entity<quad_2d_t, velocity_t>(
        make_quad(glm::vec2(130.0f, 10.0f), glm::vec2(20.0f)),
        make_velocity(-500.0f, 500.0f));

    rg.spawn_entity<quad_2d_t, velocity_t>(
        make_quad(glm::vec2(180.0f, 10.0f), glm::vec2(20.0f)),
        make_velocity(500.0f, 500.0f));

    return {};
}

static SystemResult update(registry_t rg, float dt)
{
    auto& info = rg.get_resource<window_creation_info_t>();

    auto view = rg.get_view<quad_2d_t, velocity_t>();
    for (const auto& entity : view) {
        auto [quad, velocity] = view.get<quad_2d_t, velocity_t>(entity);

        quad.position += velocity.direction * dt;
        if (quad.position.x + quad.dimension.x
            >= static_cast<float>(info.width)) {
            quad.position.x = static_cast<float>(info.width) - quad.dimension.x;
            velocity.direction.x *= -1.0f;
        } else if (quad.position.x < 0.0f) {
            quad.position.x = 0.0f;
            velocity.direction.x *= -1.0f;
        }

        if (quad.position.y + quad.dimension.y
            >= static_cast<float>(info.height)) {
            quad.position.y
                = static_cast<float>(info.height) - quad.dimension.y;
            velocity.direction.y *= -1.0f;
        } else if (quad.position.y < 0.0f) {
            quad.position.y = 0.0f;
            velocity.direction.y *= -1.0f;
        }
    }

    return {};
}

int32_t main()
{
    app_t app;
    app.add_plugin(make_plugin<renderer_2d_t>(window_sdl_t(
        "Basic 2D Renderer (OpenGL)", 1280, 720, sdl_event_handler)));

    app.add_system(make_startup(setup));
    app.add_system(make_update(update));

    app.run();

    return 0;
}