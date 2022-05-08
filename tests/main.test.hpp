#pragma once

#include <filesystem>

struct plugins_env {
    std::filesystem::path plugin_path;
};

const plugins_env& get_plugins_env();

