# FENGINE

> A modular, plugin based, ECS-driven game engine.

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
FENGINE is a modular and plugin based game engine with a retained-mode OpenGL renderer, ECS, and a custom asset pipeline.

## Example
```cpp
#include <app.h>
#include <renderer_2d.h>

#include <random>

struct velocity_t {
    float x;
    float y;
};

static velocity_t make_velocity(glm::vec2 direction)
{
    return { .x = direction.x, .y = direction.y };
}

static SystemResult sdl_event_handler(registry_t rg, float)
{
    auto app_state = rg.get_resource<app_state_t*>();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
            app_state->running = false;

        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
            app_state->running = false;
    }

    return {};
}

static SystemResult setup(registry_t rg)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    auto info = rg.get_resource<window_creation_info_t>();

    auto dimension = glm::vec2(20.0f);
    auto position = glm::vec2(info.width / 2.0f - dimension.x / 2.0f,
                              info.height / 2.0f - dimension.y / 2.0f);

    float coefficient = 1000.0f;
    float x = std::min(dist(gen), 0.1f);
    float y = std::min(dist(gen), 0.1f);

    rg.spawn_entity(make_quad(position, dimension),
                    make_velocity(glm::vec2(x * coefficient, y * coefficient)));
    return {};
}

static SystemResult update(registry_t rg, float dt)
{
    auto info = rg.get_resource<window_creation_info_t>();

    float window_width = static_cast<float>(info.width);
    float window_height = static_cast<float>(info.height);

    auto view = rg.get_view<quad_2d_t, velocity_t>();
    for (auto entity : view) {
        auto [quad, velocity] = view.get(entity);

        quad.position.x += velocity.x * dt;
        quad.position.y += velocity.y * dt;

        if (quad.position.x + quad.dimension.x >= window_width) {
            quad.position.x = window_width - quad.dimension.x;
            velocity.x *= -1;
        } else if (quad.position.x <= 0.0f) {
            quad.position.x = 0.0f;
            velocity.x *= -1;
        }

        if (quad.position.y + quad.dimension.y >= window_height) {
            quad.position.y = window_height - quad.dimension.y;
            velocity.y *= -1;
        } else if (quad.position.y <= 0.0f) {
            quad.position.y = 0.0f;
            velocity.y *= -1;
        }
    }

    return {};
}

int32_t main()
{
    app_t app;
    app.add_plugin(make_plugin<renderer_2d_t>(
        window_sdl_t("hello window", 800, 600, sdl_event_handler)));

    app.add_system(make_startup(setup));
    app.add_system(make_update(update));
    app.run();

    return 0;
}
```
## Building
Ensure you have a C++ compiler that supports C++26.
All dependencies are included, so no manual installation is required.
To build, simply run:
`$ ./build.sh`