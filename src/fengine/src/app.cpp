#include <chrono>

#include <app.h>

void app_t::run()
{
    if (!m_app_state.can_run)
        return;

    using clock = std::chrono::high_resolution_clock;

    m_rg.ctx().emplace<app_state_t*>(&m_app_state);

    for (const auto& system : m_startup_systems) {
        if (auto result = system(&m_rg); !result) {
            std::println(stderr, "ERROR: {}", result.error());
            return;
        }
    }

    float time_acc { 0.0f };

    auto last_time = clock::now();

    m_app_state.running = true;
    while (m_app_state.running) {
        auto current_time = clock::now();
        auto delta_time
            = std::chrono::duration<float, std::chrono::seconds::period>(
                  current_time - last_time)
                  .count();

        last_time = current_time;
        time_acc += delta_time;

        while (time_acc >= m_app_state.fixed_time_step) {
            for (const auto& system : m_fixed_update_systems) {
                if (auto result = system(&m_rg, m_app_state.fixed_time_step);
                    !result) {
                    std::println(stderr, "ERROR: {}", result.error());
                    m_app_state.running = false;
                    goto end;
                }
            }

            time_acc -= m_app_state.fixed_time_step;
        }

        for (const auto& system : m_update_systems) {
            if (auto result = system(&m_rg, delta_time); !result) {
                std::println(stderr, "ERROR: {}", result.error());
                m_app_state.running = false;
                goto end;
            }
        }
    }

end:

    for (const auto& system : m_shutdown_systems) {
        if (auto result = system(&m_rg); !result) {
            std::println(stderr, "ERROR: {}", result.error());
            return;
        }
    }
}

void app_t::add_plugin(std::unique_ptr<plugin_t> plugin)
{
    if (auto result = plugin->build(this); !result) {
        std::println(stderr, "ERROR: failed to add plugin: {}", result.error());
        m_app_state.can_run = false;
    }
}

void app_t::add_system(startup_system_t system)
{
    m_startup_systems.push_back(std::move(system));
}

void app_t::add_system(shutdown_system_t system)
{
    m_shutdown_systems.push_back(std::move(system));
}

void app_t::add_system(fixed_update_system_t system)
{
    m_fixed_update_systems.push_back(std::move(system));
}

void app_t::add_system(update_system_t system)
{
    m_update_systems.push_back(std::move(system));
}

registry_t app_t::get_registry()
{
    return { &m_rg };
}
