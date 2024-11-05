#pragma once

#include <vector>
#include <memory>
#include <string>

namespace hmp {
    enum class token_type {
        unknown,

        open_bracket,   // [
        close_bracket,  // ]

        open_par,       // (
        close_par,      // )

        comma,          // ,
        colon,          // :

        identifier,     // #key
        string,         // "str"
        integer,        // 0 1 2 3..
        floating,       // 0.1 9.20 4.33..
        global_call,    // point(0, 1) rect(0, 0, 6, 3)..
        void_val,       // void
        concat,         // &
        concat_space,   // &&
        constant        // RETURN QUOTE..
    };

    struct token {
        token_type type;
        std::string value;

        token();
        token(token_type type);
        token(token_type type, std::string value);
    };

    std::ostream& operator<<(std::ostream&, token const&);

    // Turns a string of utf-8 characters into a sequence of tokens.
    int tokenize(std::string const&, std::vector<token>&);
}