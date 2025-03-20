#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>

namespace fs = std::filesystem;

class shader_t {
public:
    shader_t() { };
    ~shader_t();

    shader_t(const shader_t& other) = delete;
    shader_t& operator=(const shader_t& other) = delete;

    shader_t(shader_t&& other);
    shader_t& operator=(shader_t&& other);

    uint32_t get_id() const;

    void bind() const;

    std::expected<void, std::string>
    load_shader(const fs::path& shader_source_path);

private:
    uint32_t m_program { 0 };
};
