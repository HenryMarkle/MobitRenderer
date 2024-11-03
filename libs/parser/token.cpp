#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "token.hpp"

using namespace hmp;

token::token() {
    this->type = token_type::unknown;
    this->value = "";
}

token::token(token_type type) {
    this->type = type;
    this->value = "";
}

token::token(token_type type, std::string value) {
    this->type = type;
    this->value = value;
}

std::ostream& hmp::operator<<(std::ostream& stream, token const& _token) {
    
    switch (_token.type) {
        case token_type::unknown:
        return stream << "UNK";

        case token_type::string:
        return stream << "STR(" << _token.value << ")";

        case token_type::integer:
        return stream << "INT(" << _token.value << ")";

        case token_type::floating:
        return stream << "FLT(" << _token.value << ")";

        case token_type::identifier:
        return stream << "IDN(" << _token.value << ")";

        case token_type::global_call:
        return stream << "GCL(" << _token.value << ")";

        case token_type::void_val:
        return stream << "VOID";

        case token_type::constant:
        return stream << "CONST(" << _token.value << ")";

        case token_type::comma:
        return stream << ",";

        case token_type::colon:
        return stream << ":";

        case token_type::open_bracket:
        return stream << '[';

        case token_type::close_bracket:
        return stream << ']';

        case token_type::open_par:
        return stream << '(';

        case token_type::close_par:
        return stream << ')';

        case token_type::concat:
        return stream << '&';
    }
    
    return stream << "";
}

int hmp::tokenize(std::string const& input, std::vector<token>& tokens) {
    if (input.size() == 0) {
        #ifdef DEBUG_LOG
        std::cout << "DEBUG [tokenize]: ERROR: empty string" << std::endl;
        #endif
        
        return -1;
    }

    tokens.clear();

    auto cursor = input.begin();

    while (cursor != input.end()) {

        switch (*cursor) {
            case ',':
            tokens.push_back(token(token_type::comma));
            break;

            case ':':
            tokens.push_back(token(token_type::colon));
            break;

            case '[':
            tokens.push_back(token(token_type::open_bracket));
            break;

            case ']':
            tokens.push_back(token(token_type::close_bracket));
            break;

            case '(':
            tokens.push_back(token(token_type::open_par));
            break;

            case ')':
            tokens.push_back(token(token_type::close_par));
            break;

            case '&':
            tokens.push_back(token(token_type::concat));
            break;

            case '#':
            {
                auto begin = ++cursor;
                size_t count = 0;

                while (cursor != input.end() && (isalnum(*cursor) || *cursor == '_')) {
                    cursor++;
                    count++;
                }

                if (count == 0) {
                    #ifdef DEBUG_LOG
                    std::cout << "DEBUG [tokenize]: ERROR: empty key identifier" << std::endl;
                    #endif
                
                    return -2;
                }

                std::string key;
                key.reserve(count);

                for (size_t i = 0; i < count; i++) {
                    key.push_back(*begin);
                    begin++;
                }

                tokens.push_back(token(token_type::identifier, key));

                cursor--;
                // std::cout << "AT " << *cursor << std::endl;
            }
            break;

            case  ' ':
            case '\t':
            case '\n':
            case '\r':
            break;

            case '"':
            {
                auto begin = ++cursor;
                size_t count = 0;

                while (cursor != input.end() && *cursor != '"') {
                    cursor++;
                    count++;
                }

                if (cursor == input.end()) {
                    #ifdef DEBUG_LOG
                    std::cout << "DEBUG [tokenize]: ERROR: expected \"" << std::endl;
                    #endif
                    
                    return -3;
                }

                std::string str;

                str.reserve(count);


                for (size_t i = 0; i < count; i++) {
                    str.push_back(*begin);
                    begin++;
                }


                tokens.push_back(token(token_type::string, str));
            }
            break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                auto begin = cursor;
                size_t count = 0;

                bool floating = false;

                while (cursor != input.end() && (isdigit(*cursor) || *cursor == '.')) {

                    if (*cursor == '.') {
                        if (floating) {
                            #ifdef DEBUG_LOG
                            std::cout << "DEBUG [tokenize]: ERROR: unexpected second decimal point" << std::endl;
                            #endif
                        
                            return -5;
                        }
                        floating = true;
                    }

                    cursor++;
                    count++;
                }

                std::string number;
                number.reserve(count);

                for (size_t i = 0; i < count; i++) {
                    number.push_back(*begin);
                    begin++;
                }

                if (floating) tokens.push_back(token(token_type::floating, number));
                else tokens.push_back(token(token_type::integer, number));

                --cursor;
            }
            break;

            default:
            {
                auto begin = cursor;
                size_t count = 0;

                while (cursor != input.end() && (isalnum(*cursor) || *cursor == '_')) {
                    cursor++;
                    count++;
                }

                if (count == 0) {
                    #ifdef DEBUG_LOG
                    std::cout << "DEBUG [tokenize]: ERROR: unexpected character \"" << *cursor << "\"" << std::endl;
                    #endif
                    return -4;
                }

                std::string gcall;
                gcall.reserve(count);

                for (size_t i = 0; i < count; i++) {
                    gcall.push_back(*begin);
                    begin++;
                }

                if (gcall == "void") {
                    tokens.push_back(token(token_type::void_val));
                }
                else if (*cursor == '(') {
                    tokens.push_back(token(token_type::global_call, gcall));
                }
                else {
                    tokens.push_back(token(token_type::constant, gcall));
                }

                cursor--;
            }
            break;
        }

        cursor++;
    }

    return 0;
}
