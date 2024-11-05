#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include <MobitRenderer/parsing.h>

namespace mr {

    void AST_Integer::print() const {
        std::cout << "Int(" << number << ")";
    }

    AST_Integer::AST_Integer(int number) : number(number) {}



    void AST_Float::print() const {
        std::cout << "Float(" << number << ")";
    }

    AST_Float::AST_Float(float number) : number(number) {}



    void AST_String::print() const {
        std::cout << "String(" << str << ")";
    }

    AST_String::AST_String(std::string const& str) : str(str) {}

    AST_String::AST_String(std::string&& str) : str(std::move(str)) {}
    AST_String::AST_String(AST_String&& other) : str(std::move(other.str)) {}

    AST_String& AST_String::operator=(AST_String&& other) noexcept {
        if (this == &other) return *this;

        str.clear();

        str = std::move(other.str);

        return *this;
    }


    void AST_Void::print() const {
        std::cout << "Void";
    }

    AST_Void::AST_Void() {}



    void AST_GlobalCall::print() const {
        std::cout << name << "("; 
        for (auto& node : arguments) {
            node->print();
            std::cout << ", ";
        }
        std::cout << ")";
    }

    AST_GlobalCall::AST_GlobalCall(std::string const& name) : name(name), arguments(std::vector<std::unique_ptr<mr::AST_Node>>()) {}
    AST_GlobalCall::AST_GlobalCall(std::string&& name) : name(std::move(name)), arguments(std::vector<std::unique_ptr<mr::AST_Node>>()) {}
    AST_GlobalCall::AST_GlobalCall(std::string const& name, std::vector<std::unique_ptr<AST_Node>>&& arguments) : name(name), arguments(std::move(arguments)) {}
    AST_GlobalCall::AST_GlobalCall(AST_GlobalCall&& other) {
        name = std::move(other.name);
        arguments = std::move(other.arguments);
    }

    AST_GlobalCall& AST_GlobalCall::operator=(AST_GlobalCall&& other) noexcept {
        if (this == &other) return *this;

        name.clear();
        arguments.clear();

        name = std::move(other.name);
        arguments = std::move(other.arguments);

        return *this;
    }


    void AST_LinearList::print() const {
        std::cout << "[ ";

        for (auto& e : elements) {
            e->print();
            std::cout << ", ";
        }

        std::cout << " ]";
    }

    AST_LinearList::AST_LinearList() : elements(std::vector<std::unique_ptr<mr::AST_Node>>()) {}
    AST_LinearList::AST_LinearList(std::vector<std::unique_ptr<AST_Node>>&& elements) : elements(std::move(elements)) {}
    AST_LinearList::AST_LinearList(AST_LinearList&& list) : elements(std::move(list.elements)) {}
    AST_LinearList& AST_LinearList::operator=(AST_LinearList&& other) noexcept {
        if (this == &other) return *this;

        elements.clear();

        elements = std::move(other.elements);

        return *this;
    }


    void AST_PropertyList::print() const {
        std::cout << "[ ";

        for (auto& pair : elements) {
            std::cout << "#" << pair.first;
            std::cout << ": ";
            pair.second->print();

            std::cout << ", ";
        }

        std::cout << " ]";
    }

    AST_PropertyList::AST_PropertyList() : elements(std::unordered_map<std::string, std::unique_ptr<AST_Node>>()) {}
    AST_PropertyList::AST_PropertyList(std::unordered_map<std::string, std::unique_ptr<AST_Node>>&& um) : elements(std::move(um)) {}
    AST_PropertyList::AST_PropertyList(AST_PropertyList&& other) : elements(std::move(other.elements)) {}
    AST_PropertyList& AST_PropertyList::operator=(AST_PropertyList&& other) noexcept {
        if (this == &other) return *this;

        elements.clear();

        elements = std::move(other.elements);

        return *this;
    }
};

