#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <unordered_map>

namespace mr {

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

    class AST_Node {
        public:
        virtual ~AST_Node() = default;
        virtual void print() const = 0;
    };

    class AST_Integer : public AST_Node {
        public:

        int number;

        AST_Integer(int);

        void print() const override;
    };

    class AST_Float : public AST_Node {
        public:

        float number;

        AST_Float(float);

        void print() const override;
    };

    class AST_String : public AST_Node {
        public:

        std::string str;

        AST_String(std::string const&);
        AST_String(std::string&&);
        AST_String(AST_String&) = delete;
        AST_String(AST_String&&);

        AST_String& operator=(AST_String const&) = delete;
        AST_String& operator=(AST_String&&) noexcept;
        
        void print() const override;
    };

    class AST_Void : public AST_Node {
        public:

        AST_Void();

        void print() const override;
    };

    class AST_GlobalCall : public AST_Node {
        public:

        std::string name;
        std::vector<std::unique_ptr<AST_Node>> arguments;

        AST_GlobalCall(std::string const& name);
        AST_GlobalCall(std::string&& name);
        AST_GlobalCall(std::string const& name, std::vector<std::unique_ptr<AST_Node>>&&);
        AST_GlobalCall(AST_GlobalCall&) = delete;
        AST_GlobalCall(AST_GlobalCall&&);

        AST_GlobalCall& operator=(AST_GlobalCall const&) = delete;
        AST_GlobalCall& operator=(AST_GlobalCall&&) noexcept;

        void print() const override;
    };

    class AST_LinearList : public AST_Node {
        public:
        std::vector<std::unique_ptr<AST_Node>> elements;

        AST_LinearList();
        AST_LinearList(std::vector<std::unique_ptr<AST_Node>>&&);
        AST_LinearList(AST_LinearList&) = delete;
        AST_LinearList(AST_LinearList&&);

        AST_LinearList& operator=(AST_LinearList const&) = delete;
        AST_LinearList& operator=(AST_LinearList&&) noexcept;

        void print() const override;
    };

    class AST_PropertyList : public AST_Node {
        public:

        std::unordered_map<std::string, std::unique_ptr<AST_Node>> elements;

        AST_PropertyList();
        AST_PropertyList(std::unordered_map<std::string, std::unique_ptr<AST_Node>>&&);
        AST_PropertyList(AST_PropertyList&) = delete;
        AST_PropertyList(AST_PropertyList&&);

        AST_PropertyList& operator=(AST_PropertyList const&) = delete;
        AST_PropertyList& operator=(AST_PropertyList&&) noexcept;

        void print() const override;
    };

    int parse_tokens(std::vector<token> const& tokens, std::unique_ptr<AST_Node>& result);
};