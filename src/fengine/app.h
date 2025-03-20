#pragma once

#include <memory>

#include "fecs.h"

class app_t;

using PluginResult = std::expected<void, std::string>;

class plugin_t {
public:
    virtual ~plugin_t() = default;

    virtual PluginResult build(app_t* app) = 0;
};

template<typename T, typename... Args>
    requires std::is_base_of_v<plugin_t, T>
std::unique_ptr<T> make_plugin(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

struct app_state_t {
    float fixed_time_step { 1.0f / 60.0f };

    bool can_run { true };
    bool running { false };
};

class app_t {
public:
    void add_plugin(std::unique_ptr<plugin_t> plugin);

    void add_system(startup_system_t system);

    void add_system(shutdown_system_t system);

    void add_system(update_system_t system);

    void add_system(fixed_update_system_t system);

    void run();

    registry_t get_registry();

private:
    app_state_t m_app_state;

    entt::registry m_rg;

    std::vector<startup_system_t> m_startup_systems;
    std::vector<shutdown_system_t> m_shutdown_systems;
    std::vector<update_system_t> m_update_systems;
    std::vector<fixed_update_system_t> m_fixed_update_systems;
};