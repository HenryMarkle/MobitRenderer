#pragma once

#include <exception>
#include <string>
#include <iostream>

namespace mr {
    class unsupported : public std::exception {

    };

    class file_read_error : public std::exception {
        // private:
        // std::string message_;
        
        // public:
        // explicit file_read_error(const std::string& message);
        // const char* what() const noexcept override;
    };
};