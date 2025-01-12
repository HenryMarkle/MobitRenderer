#include <MobitRenderer/io.h>

#include <windows.h>
#include <string>
#include <iostream>
#include <filesystem>

namespace mr {

std::filesystem::path get_executable_dir() {
    char buffer[MAX_PATH]; // MAX_PATH is 260
    DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);

    if (length == 0 || length == MAX_PATH) {
        throw std::runtime_error("Failed to get executable path");
    }

    return std::filesystem::absolute(buffer).parent_path();
}

size_t get_path_max_len() {
    return MAX_PATH;
}

};
