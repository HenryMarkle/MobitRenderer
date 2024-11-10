#include <MobitRenderer/exceptions.h>
#include <string>

namespace mr {
// file_read_error::file_read_error(const std::string& message) :
// message_(message) {} const char* file_read_error::what() const noexcept {
//     return message_.c_str();
// }

parse_failure::parse_failure(const std::string &msg) : msg_(msg) {}
const char *parse_failure::what() const noexcept { return msg_.c_str(); }
}; // namespace mr
