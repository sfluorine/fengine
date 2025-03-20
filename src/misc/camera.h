#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class camera_t {
public:
    glm::mat4 get_projection_matrix(float fov,
                                    float window_width,
                                    float window_height) const;

    glm::mat4 get_view_matrix() const;

    void process_mouse();

    void process_keyboard(float delta_time);

private:
    void update_camera();

private:
    float m_yaw { -90.0f };
    float m_pitch { 0.0f };
    float m_sensitivity { 0.2f };
    float m_speed { 5.0f };

    glm::vec3 m_position {};
    glm::vec3 m_front {};
    glm::vec3 m_right {};
    glm::vec3 m_up {};
};
