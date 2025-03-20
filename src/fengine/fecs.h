#pragma once

#include <expected>
#include <print>
#include <string>

#include <entt/entt.hpp>

class registry_t {
public:
    registry_t(entt::registry* rg)
        : m_rg(rg)
    {
    }

    template<typename... Args>
    void spawn_entity(Args&&... args)
    {
        auto entity = m_rg->create();
        ((m_rg->emplace<Args>(entity, std::forward<Args>(args))), ...);
    }

    template<typename T, typename... Args>
    void spawn_resource(Args&&... args)
    {
        m_rg->ctx().emplace<T>(std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto get_view()
    {
        return m_rg->view<Args...>();
    }

    template<typename T>
    T& get_single()
    {
        auto view = m_rg->view<T>();
        return view.template get<T>(view.front());
    }

    template<typename T>
    T* try_get_single()
    {
        auto view = m_rg->view<T>();
        return view.template try_get<T>(view.front());
    }

    template<typename T>
    T& get_resource()
    {
        return m_rg->ctx().get<T>();
    }

    template<typename T>
    T* try_get_resource()
    {
        return m_rg->ctx().find<T>();
    }

    template<typename T>
    bool erase_resource()
    {
        return m_rg->ctx().erase<T>();
    }

private:
    entt::registry* m_rg;
};

using SystemResult = std::expected<void, std::string>;
using GenericSystem = std::function<SystemResult(registry_t)>;
using UpdateSystem = std::function<SystemResult(registry_t, float)>;

class startup_system_t {
public:
    startup_system_t(GenericSystem system)
        : m_system(std::move(system))
    {
    }

    startup_system_t(startup_system_t&& other)
        : m_system(std::move(other.m_system))
    {
    }

    startup_system_t(const startup_system_t& other) = delete;

    SystemResult operator()(entt::registry* rg) const
    {
        return m_system(rg);
    }

private:
    GenericSystem m_system;
};

startup_system_t make_startup(GenericSystem system);

class shutdown_system_t {
public:
    shutdown_system_t(GenericSystem system)
        : m_system(std::move(system))
    {
    }

    shutdown_system_t(shutdown_system_t&& other)
        : m_system(std::move(other.m_system))
    {
    }

    shutdown_system_t(const shutdown_system_t& other) = delete;

    SystemResult operator()(entt::registry* rg) const
    {
        return m_system(rg);
    }

private:
    GenericSystem m_system;
};

shutdown_system_t make_shutdown(GenericSystem system);

class update_system_t {
public:
    update_system_t(UpdateSystem system)
        : m_system(std::move(system))
    {
    }

    update_system_t(update_system_t&& other)
        : m_system(std::move(other.m_system))
    {
    }

    update_system_t(const update_system_t& other) = delete;

    SystemResult operator()(entt::registry* rg, float dt) const
    {
        return m_system(rg, dt);
    }

private:
    UpdateSystem m_system;
};

update_system_t make_update(UpdateSystem system);

class fixed_update_system_t {
public:
    fixed_update_system_t(UpdateSystem system)
        : m_system(std::move(system))
    {
    }

    fixed_update_system_t(fixed_update_system_t&& other)
        : m_system(std::move(other.m_system))
    {
    }

    fixed_update_system_t(const fixed_update_system_t& other) = delete;

    SystemResult operator()(entt::registry* rg, float dt) const
    {
        return m_system(rg, dt);
    }

private:
    UpdateSystem m_system;
};

fixed_update_system_t make_fixed_update(UpdateSystem system);
