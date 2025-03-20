#include "fecs.h"

startup_system_t make_startup(GenericSystem system)
{
    return { std::move(system) };
}

shutdown_system_t make_shutdown(GenericSystem system)
{
    return { std::move(system) };
}

update_system_t make_update(UpdateSystem system)
{
    return { std::move(system) };
}

fixed_update_system_t make_fixed_update(UpdateSystem system)
{
    return { std::move(system) };
}
