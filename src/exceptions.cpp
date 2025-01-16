#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <string>
#include <MobitRenderer/exceptions.h>

namespace mr {
// file_read_error::file_read_error(const std::string& message) :
// message_(message) {} const char* file_read_error::what() const noexcept {
//     return message_.c_str();
// }

parse_failure::parse_failure(const std::string &msg) : msg_(msg) {}
const char *parse_failure::what() const noexcept { return msg_.c_str(); }

deserialization_failure::deserialization_failure(const std::string &msg)
    : msg_(msg) {}
const char *deserialization_failure::what() const noexcept {
  return msg_.c_str();
}

dex_error::dex_error(const std::string &msg) : msg_(msg) {}
const char *dex_error::what() const noexcept { return msg_.c_str(); }

malformed_geometry::malformed_geometry(const std::string &msg)
    : deserialization_failure(msg) {}
}; // namespace mr
