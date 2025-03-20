#include "camera.h"

#include <SDL3/SDL.h>

glm::mat4 camera_t::get_projection_matrix(float fov,
                                          float window_width,
                                          float window_height) const
{
    return glm::perspective(
        glm::radians(fov), window_width / window_height, 0.1f, 100.0f);
}

glm::mat4 camera_t::get_view_matrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void camera_t::update_camera()
{
    m_front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    m_front.y = glm::sin(glm::radians(m_pitch));
    m_front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));

    m_front = glm::normalize(m_front);
    m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void camera_t::process_mouse()
{
    glm::vec2 mouse_offset;
    {
        float offset_x, offset_y;
        SDL_GetRelativeMouseState(&offset_x, &offset_y);
        mouse_offset = glm::vec2(offset_x, offset_y) * m_sensitivity;
    }

    m_yaw += mouse_offset.x;
    m_pitch -= mouse_offset.y;

    if (m_pitch > 89.0f)
        m_pitch = 89.0f;

    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    update_camera();
}

void camera_t::process_keyboard(float delta_time)
{
    auto state = SDL_GetKeyboardState(nullptr);
    glm::vec3 direction(0.0f);

    if (state[SDL_SCANCODE_W])
        direction += m_front;

    if (state[SDL_SCANCODE_S])
        direction -= m_front;

    if (state[SDL_SCANCODE_A])
        direction -= m_right;

    if (state[SDL_SCANCODE_D])
        direction += m_right;

    // normalize the direction vector. this is because if we press W and A
    // simultaneously the speed is (speed * sqrt(2)).
    if (glm::length(direction) > 0.0f)
        direction = glm::normalize(direction);

    m_position += direction * m_speed * delta_time;
}
