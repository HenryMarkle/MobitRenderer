#include <optional>
#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <stack>
#include <unordered_map>

#include <MobitRenderer/parsing.h>

using std::string;
using std::vector;
using std::unordered_map;
using std::unique_ptr;
using std::make_unique;
using std::move;

namespace mr {

    int parse_tokens_helper(
        vector<token>::const_iterator& begin, 
        vector<token>::const_iterator const& end,
        unique_ptr<AST_Node>& ptr
    )
    {
        auto cursor = begin;


        // while (cursor != end) {
            
            
        //     cursor++;
        // }
        switch (cursor->type) {
            case token_type::void_val:
            {
                begin = cursor + 1;
                ptr = make_unique<AST_Void>(AST_Void());
                return 0;;
            }

            case token_type::integer:
            {
                int integer = std::stoi(cursor->value);
                ptr = make_unique<AST_Integer>(AST_Integer(integer));
                begin = cursor + 1;
                return 0;
            }

            case token_type::floating:
            {
                float floating = std::stof(cursor->value);
                ptr = make_unique<AST_Float>(AST_Float(floating));
                begin = cursor + 1;
                return 0;
            }

            case token_type::string:
            {
                auto str_cur = cursor;
                bool concatenated = false;

                string str = cursor->value;

                str_cur++;

                while (str_cur != end && str_cur->type != token_type::comma) {
                    switch (str_cur->type) {
                        case token_type::concat:
                        if (concatenated) { ptr = nullptr; return 300; }
                        concatenated = true;
                        break;

                        case token_type::string:
                        if (!concatenated) { ptr = nullptr; return 301; }

                        str.append(str_cur->value);
                        concatenated = false;
                        break;

                        case token_type::constant:
                        if (!concatenated) { ptr = nullptr; return 302; }

                        if (str_cur->value == "ENTER") str.push_back('\n'); 
                        else if (str_cur->value == "PI") str.append("3.14");
                        else if (str_cur->value == "QUOTE") str.push_back('"');
                        else if (str_cur->value == "RETURN") str.push_back('\n');
                        else if (str_cur->value == "SPACE") str.push_back(' ');
                        else if (str_cur->value == "TAB") str.push_back('\t');

                        concatenated = false;
                        break;
                    }

                    str_cur++;
                }

                if (concatenated) {
                    ptr = nullptr;
                    return 303;
                }

                begin = cursor = str_cur++;

                ptr = make_unique<AST_String>(AST_String(move(str)));
                return 0;
            }

            case token_type::open_bracket:
            {
                auto next = cursor + 1;

                // empty linear list
                if (next != end && next->type == token_type::close_bracket) {
                    
                    ptr = make_unique<AST_LinearList>(AST_LinearList());
                    begin = next;
                    return 0;
                }

                // empty property list
                if (next != end && next->type == token_type::colon) {
                    auto after_next = next + 1;

                    if (after_next != end && after_next->type == token_type::close_bracket) {

                        ptr = make_unique<AST_PropertyList>(AST_PropertyList());
                        begin = after_next;
                        return 0;
                    }

                    ptr = nullptr;
                    return 400;
                }


                // find the closing bracket

                std::stack<char> depth;

                auto clt = begin;

                while (clt != end) {
                    
                    switch (clt->type) {
                        case token_type::open_bracket:
                        depth.push('[');
                        break;

                        case token_type::open_par:
                        depth.push('(');
                        break;

                        case token_type::close_par:
                        if (depth.top() != '(' || depth.size() <= 1) { ptr = nullptr; return 401; }
                        depth.pop();
                        break;

                        case token_type::close_bracket:
                        if (depth.top() != '[' || depth.size() < 1) { ptr = nullptr; return 402; }
                        depth.pop();
                        if (depth.size() == 0) goto stop_clt;
                        break;
                    }

                    clt++;
                }

                if (clt == end) { ptr = nullptr; return 403; }

                stop_clt:

                // check if linear or prp

                bool is_prop = false;
                if (next->type == token_type::identifier) is_prop = true; 

                vector<unique_ptr<AST_Node>> nodes;
                unordered_map<string, unique_ptr<AST_Node>> pairs;

                bool commad = true;

                while (next != clt) {
                    if (next->type == token_type::comma) {
                        if (commad) { ptr = nullptr; return 406; }
                        commad = true;
                        next++;
                        continue;
                    }

                    if (!commad) { 
                        ptr = nullptr; return 413;
                    }

                    if (is_prop) {
                        if (next->type != token_type::identifier) { ptr = nullptr; return 407;}
                        
                        auto colon_c = next + 1;
                        auto val_c = next + 2;
                        
                        if (colon_c == clt || colon_c->type != token_type::colon) { ptr = nullptr; return 408;}
                        if (val_c == clt || 
                            !(val_c->type == token_type::integer || 
                            val_c->type == token_type::floating || 
                            val_c->type == token_type::string || 
                            val_c->type == token_type::open_bracket || 
                            val_c->type == token_type::global_call ||
                            val_c->type == token_type::void_val ||
                            val_c->type == token_type::constant)
                        ) {
                            ptr = nullptr;
                            return 409;
                        }
                        
                        string key = next->value;
                        unique_ptr<AST_Node> node_res = nullptr;

                        int parse_res = parse_tokens_helper(val_c, clt, node_res);

                        if (parse_res) { ptr = nullptr; return 411; }
                        if (!node_res) { ptr = nullptr; return 412; }

                        pairs.insert_or_assign(move(key), move(node_res));

                        next = val_c;
                    }
                    else {
                        if (next == clt || 
                            !(next->type == token_type::integer || 
                            next->type == token_type::floating || 
                            next->type == token_type::string || 
                            next->type == token_type::open_bracket || 
                            next->type == token_type::global_call ||
                            next->type == token_type::void_val ||
                            next->type == token_type::constant)
                        ) {
                            return 410;
                        }

                        unique_ptr<AST_Node> node_res;

                        int parse_res = parse_tokens_helper(next, clt, node_res);

                        if (parse_res) { 
                            #ifdef DEBUG_LOG
                            std::cerr << "failed to parse subnode: " << parse_res << std::endl;
                            #endif
                        
                            ptr = nullptr; 
                            return 404;
                        }

                        if (node_res == nullptr) { ptr = nullptr; return 405; }

                        nodes.push_back(move(node_res));
                    }


                    commad = false;
                }

                begin = next + 1;

                if (is_prop) ptr = make_unique<AST_PropertyList>(AST_PropertyList(move(pairs)));
                else ptr = make_unique<AST_LinearList>(AST_LinearList(move(nodes)));

                return 0;
            }
        
            case token_type::global_call:
            {
                auto otl = cursor + 1;
                auto clt = otl + 1;

                if (otl == end || otl->type != token_type::open_par) {
                    ptr = nullptr;
                    return 500;
                }

                // empty arguments
                if (clt != end && clt->type == token_type::close_par) {
                    ptr = make_unique<AST_GlobalCall>(AST_GlobalCall(cursor->value));
                    return 0;
                }

                // find the closing parenthesis

                std::stack<char> depth;

                depth.push('(');

                while (clt != end) {
                    
                    switch (clt->type) {
                        case token_type::open_bracket:
                        depth.push('[');
                        break;

                        case token_type::open_par:
                        depth.push('(');
                        break;

                        case token_type::close_par:
                        if (depth.top() != '(' || depth.size() < 1) { ptr = nullptr; return 501; }
                        depth.pop();
                        if (depth.size() == 0) goto stop_clt_2;
                        break;

                        case token_type::close_bracket:
                        if (depth.top() != '[' || depth.size() <= 1) { ptr = nullptr; return 502; }
                        depth.pop();
                        break;
                    }

                    clt++;
                }

                if (clt == end) { ptr = nullptr; return 503; }

                stop_clt_2:

                vector<unique_ptr<AST_Node>> nodes;

                auto next = otl + 1;
                bool commad = true;

                while (next != clt) {
                    if (next->type == token_type::comma) {
                        if (commad) { ptr = nullptr; return 504; }
                        commad = true;
                        next++;
                        continue;
                    }

                    if (!commad) { 
                        ptr = nullptr; 
                        return 505;
                    }

                    if (next == clt || 
                            !(next->type == token_type::integer || 
                            next->type == token_type::floating || 
                            next->type == token_type::string || 
                            next->type == token_type::open_bracket || 
                            next->type == token_type::global_call ||
                            next->type == token_type::void_val ||
                            next->type == token_type::constant)
                        ) 
                    {
                        return 506;
                    }

                    unique_ptr<AST_Node> node_res;

                    int parse_res = parse_tokens_helper(next, clt, node_res);


                    if (parse_res) { 
                        #ifdef DEBUG_LOG
                        std::cerr << "failed to parse subnode: " << parse_res << std::endl;
                        #endif
                    
                        ptr = nullptr; 
                        return 507;
                    }

                    if (node_res == nullptr) { ptr = nullptr; return 408; }

                    nodes.push_back(move(node_res));

                    commad = false;
                }

                begin = next + 1;

                ptr = make_unique<AST_GlobalCall>(AST_GlobalCall(cursor->value, move(nodes)));

                return 0;
            }

            default:
            #ifdef DEBUG_LOG
            std::cerr << "unknown token: " << *cursor << std::endl;
            #endif
            ptr = nullptr;
            return 600;
        }

        return 1;
    }

    int parse_tokens(std::vector<token> const& tokens, std::unique_ptr<AST_Node>& result) {

        auto begin = tokens.begin();
        auto end = tokens.end();

        auto res = parse_tokens_helper(begin, end, result);

        return res;
    }
};

