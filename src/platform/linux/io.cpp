#include <MobitRenderer/io.h>

#include <filesystem>
#include <string>
#include <iostream>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

namespace mr {

std::filesystem::path get_executable_dir() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

    if (count == -1)
      throw "could not retrieve executable's path";
    result[count] = '\0';
    return std::filesystem::absolute(result).parent_path();
}

size_t get_path_max_len() {
    return PATH_MAX;
}

};