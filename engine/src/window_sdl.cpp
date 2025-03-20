#include <fecs.h>
#include <window_sdl.h>

window_sdl_t::window_sdl_t(const char* title,
                           int32_t width,
                           int32_t height,
                           UpdateSystem event_handler)
{
    m_info.title = title;
    m_info.width = width;
    m_info.height = height;

    m_event_handler = std::move(event_handler);
}

window_sdl_t::window_sdl_t(window_sdl_t&& other)
    : m_info(other.m_info)
    , m_event_handler(std::move(other.m_event_handler))
{
}

PluginResult window_sdl_t::build(app_t* app)
{
    auto rg = app->get_registry();
    rg.put_resource<window_creation_info_t>(m_info);

    app->add_system(make_startup(setup));
    app->add_system(make_update(m_event_handler));
    app->add_system(make_shutdown(shutdown));

    return {};
}

window_creation_info_t window_sdl_t::get_creation_info() const
{
    return m_info;
}

SystemResult window_sdl_t::setup(registry_t rg)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return std::unexpected(
            std::format("cannot initialize sdl: {}", SDL_GetError()));
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window;

    {
        auto& info = rg.get_resource<window_creation_info_t>();

        window = SDL_CreateWindow(
            info.title, info.width, info.height, SDL_WINDOW_OPENGL);
        if (!window) {
            auto result = std::unexpected(
                std::format("cannot create sdl window: {}", SDL_GetError()));

            SDL_Quit();
            return result;
        }
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!SDL_GL_MakeCurrent(window, context)) {
        auto result = std::unexpected(std::format(
            "cannot make OpenGL context current: {}", SDL_GetError()));

        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return result;
    }

    if (!gladLoadGLLoader(
            reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        auto result = std::unexpected("cannot initialize glad");

        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return result;
    }

    rg.put_resource<sdl_context_t>(window, context);
    return {};
}

SystemResult window_sdl_t::shutdown(registry_t rg)
{
    auto& sdl_context = rg.get_resource<sdl_context_t>();

    SDL_GL_DestroyContext(sdl_context.context);
    SDL_DestroyWindow(sdl_context.window);
    SDL_Quit();

    return {};
}
