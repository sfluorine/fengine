#include "shader.h"

#include <glad/glad.h>
#include <cstring>
#include <expected>
#include <filesystem>
#include <fstream>
#include <print>
#include <sstream>
#include <string>
#include <string_view>

shader_t::~shader_t()
{
    if (m_program != 0)
        glDeleteProgram(m_program);
}

shader_t::shader_t(shader_t&& other)
    : m_program(other.m_program)
{
    other.m_program = 0;
}

shader_t& shader_t::operator=(shader_t&& other)
{
    m_program = other.m_program;
    other.m_program = 0;

    return *this;
}

void shader_t::bind() const
{
    glUseProgram(m_program);
}

std::expected<void, std::string>
shader_t::load_shader(const fs::path& shader_source_path)
{
    if (!fs::exists(shader_source_path)) {
        return std::unexpected(
            std::format("can't read a non-existent shader file '{}'",
                        shader_source_path.c_str()));
    }

    std::ifstream stream(shader_source_path);

    if (!stream) {
        return std::unexpected(std::format("can't read a shader file '{}'",
                                           shader_source_path.c_str()));
    }

    std::stringstream ss;
    ss << stream.rdbuf();

    std::string file_content = ss.str();
    std::string_view source = file_content;

    if (!source.starts_with("#version")) {
        return std::unexpected(std::format(
            "({}): expected shader version at the first line of the file",
            shader_source_path.c_str()));
    }

    std::string_view version = source.substr(0, source.find_first_of('\n'));
    source = source.substr(version.length() + 1);

    auto vsegment_start = source.find("#segment vertex");

    if (vsegment_start == source.npos) {
        return std::unexpected(std::format(
            "({}): no vertex segment is provided", shader_source_path.c_str()));
    }

    auto vsegment_source = vsegment_start + std::strlen("#segment vertex");

    auto fsegment_start = source.find("#segment fragment");

    if (fsegment_start == source.npos) {
        return std::unexpected(
            std::format("ERROR ({}): no fragment segment is provided",
                        shader_source_path.c_str()));
    }

    auto fsegment_source = fsegment_start + std::strlen("#segment fragment");

    std::string_view vsegment;
    std::string_view fsegment;

    if (vsegment_start < fsegment_start) {
        vsegment
            = source.substr(vsegment_source, fsegment_start - vsegment_source);

        fsegment = source.substr(fsegment_source);
    } else {
        fsegment
            = source.substr(fsegment_source, vsegment_start - fsegment_source);

        vsegment = source.substr(vsegment_source);
    }

    std::string vertex_source = std::string(version) + std::string(vsegment);
    std::string fragment_source = std::string(version) + std::string(fsegment);

    auto vsc = vertex_source.c_str();
    auto fsc = fragment_source.c_str();

    int32_t success = 0;
    char info_log[256];

    uint32_t vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vsc, nullptr);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vshader, 256, nullptr, info_log);
        auto result = std::unexpected(
            std::format("({}): vertex shader compile error!\n{}",
                        shader_source_path.c_str(),
                        info_log));

        glDeleteShader(vshader);
        return result;
    }

    uint32_t fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fsc, nullptr);
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fshader, 256, nullptr, info_log);
        auto result = std::unexpected(
            std::format("({}): fragment shader compile error!\n{}",
                        shader_source_path.c_str(),
                        info_log));

        glDeleteShader(vshader);
        glDeleteShader(fshader);
        return result;
    }

    uint32_t program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(fshader, 256, nullptr, info_log);
        auto result = std::unexpected(
            std::format("({}): shader program link error!\n{}",
                        shader_source_path.c_str(),
                        info_log));

        glDeleteShader(vshader);
        glDeleteShader(fshader);
        glDeleteProgram(program);
        return result;
    }

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    m_program = program;

    return {};
}

uint32_t shader_t::get_id() const
{
    return m_program;
}
