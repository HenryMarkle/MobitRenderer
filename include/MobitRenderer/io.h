#pragma once

#include <filesystem>
#include <iostream>

namespace mr {

std::filesystem::path get_executable_dir();
size_t get_path_max_len();

};