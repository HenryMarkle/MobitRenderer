#include <string>
#include <filesystem>
#include <unordered_map>
#include <exception>
#include <stdexcept>
#include <regex>
#include <iostream>

#include <raylib.h>

#include <MobitRenderer/castlibs.h>

namespace mr {

CastMember::CastMember(
    std::string const &name,
    std::string const &library,
    std::filesystem::path const &texture_path
) : 
    name(name), 
    library(library), 
    texture_path(texture_path), 
    loaded(false) 
{}

CastMember::~CastMember() {
    unload_texture();
}

CastMember *CastLib::member_or_throw(const std::string &_name) const {
    try {
        return members.at(_name);
    } catch (std::out_of_range &e) {
        throw std::runtime_error(
            std::string("member '"+_name+"' of cast library '"+name+"' not found")
        );
    }
}

void CastLib::load_members() {
    if (loaded) return;

    std::regex pattern("^"+ name + R"(_\d+_[-\w ]+\.png$)");

    for (const auto &entry : std::filesystem::directory_iterator(directory)) {
        const auto e_name = entry.path().filename().string();

        if (!std::regex_match(e_name, pattern)) {
            continue;
        }

        std::string member_stem = entry.path().stem().string();

        size_t first_pos = member_stem.find('_');
        size_t second_pos = member_stem.find('_', first_pos + 1);

        std::string member_name = member_stem.substr(second_pos + 1);

        auto *member = new CastMember(
            member_name,
            name,
            entry.path()
        );

        members[member->get_name()] = member;
    }

    loaded = true;
}

CastLib::CastLib(
    std::string const&name,
    std::filesystem::path const &directory
) : name(name), directory(directory), loaded(false) {
    if (!std::filesystem::exists(directory))
        throw std::invalid_argument(
            std::string("the directory to the cast library '"+name+"' does not exist")
        );
}
CastLib::CastLib(
    std::string &&_name,
    std::filesystem::path const &directory
) : name(std::move(_name)), directory(directory), loaded(false) {
    if (!std::filesystem::exists(directory))
        throw std::invalid_argument(
            std::string("the directory to the cast library '"+name+"' does not exist")
        );
}
CastLib::CastLib(
    const char *_name,
    std::filesystem::path const &directory
) : name(std::string(_name)), directory(directory), loaded(false) {
    if (!std::filesystem::exists(directory))
        throw std::invalid_argument(
            std::string("the directory to the cast library '"+name+"' does not exist")
        );
}

CastLib::~CastLib() {
    unload_members();
}

CastMember *CastLibs::member(const std::string &name) const noexcept {
    for (auto &pair : _libs) {
        const auto *lib = pair.second;

        auto *m = lib->member(name);
        if (m != nullptr) return m;
    }

    return nullptr;
}
CastMember *CastLibs::member_or_throw(const std::string &_name) const {
    for (auto &pair : _libs) {
        const auto *lib = pair.second;

        auto *m = lib->member(_name);
        if (m != nullptr) return m;
    }

    throw std::runtime_error(
        std::string("cast member '"+_name+"' not found in any library")
    );
}
CastLib *CastLibs::lib(std::string const &name) const noexcept {
    auto l = _libs.find(name);
    if (l == _libs.end()) return nullptr;
    return l->second;
}
CastLib *CastLibs::lib_or_throw(std::string const &name) const {
    auto l = _libs.find(name);
    if (l == _libs.end()) throw std::out_of_range(std::string("cast library \""+name+"\" not found"));
    return l->second;
}

void CastLibs::load_all_members() {
    for (auto pair : _libs) pair.second->load_members();
}

void CastLibs::unload_all_members() {
    for (auto pair : _libs) pair.second->unload_members();
}

void CastLibs::reload_all_members() {
    for (auto pair : _libs) pair.second->reload_members();
}

void CastLibs::unregister_all() {
    if (!registered) return;
    for (auto &pair : _libs) delete pair.second;
    _libs.clear();
    registered = false;
}
void CastLibs::register_all() {
    if (registered) unregister_all();

    static std::regex pattern(R"(^[a-zA-Z0-9 ]+_\d+_[-\w ]+\.png$)");

    for (const auto &entry : std::filesystem::directory_iterator(cast_dir)) {
        const auto e_name = entry.path().filename().string();

        if (!std::regex_match(e_name, pattern)) {
            continue;
        }

        std::string member_stem = entry.path().stem().string();

        size_t first_pos = member_stem.find('_');
        std::string lib_name = member_stem.substr(0, first_pos);

        if (_libs.find(lib_name) != _libs.end()) continue;

        //                               v    Lessons of the past: do not move lib_name here.
        _libs[lib_name] = new CastLib(lib_name, cast_dir);
    }

    registered = true;
}

CastLibs::CastLibs(std::filesystem::path const &cast_dir) :
    cast_dir(cast_dir), registered(false)
{}
CastLibs::CastLibs(std::filesystem::path &&cast_dir) :
    cast_dir(std::move(cast_dir)), registered(false)
{}

CastLibs::~CastLibs() {
    for (auto &pair : _libs) delete pair.second;
}

};