#pragma once

#include <glad/glad.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_video.h>

#include "app.h"
#include "fecs.h"

struct sdl_context_t {
    SDL_Window* window;
    SDL_GLContext context;
};

struct window_creation_info_t {
    const char* title;
    int32_t width;
    int32_t height;
};

class window_sdl_t : public plugin_t {

public:
    window_sdl_t(const char* title,
                 int32_t width,
                 int32_t height,
                 UpdateSystem event_handler);

    window_sdl_t(window_sdl_t&& other);

    window_sdl_t(const window_sdl_t& other) = delete;

    virtual ~window_sdl_t() override = default;

    virtual PluginResult build(app_t* app) override;

    window_creation_info_t get_creation_info() const;

    static SystemResult setup(registry_t rg);

    static SystemResult shutdown(registry_t rg);

private:
    window_creation_info_t m_info;
    UpdateSystem m_event_handler;
};