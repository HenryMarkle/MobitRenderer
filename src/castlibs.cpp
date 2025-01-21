#include <string>
#include <filesystem>
#include <unordered_map>
#include <exception>
#include <regex>

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

    std::regex pattern("^"+ name + R"(_\d+_[\w| ]+\.png$)");

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

CastLib::~CastLib() {
    unload_members();
}

CastMember *CastLibs::member(const std::string &name) const noexcept {
    CastMember *m = nullptr;

    m = editor.member(name);
    if (m != nullptr) return m;

    m = internal.member(name);
    if (m != nullptr) return m;

    m = drought.member(name);
    if (m != nullptr) return m;

    m = dry.member(name);
    if (m != nullptr) return m;

    m = msc.member(name);
    if (m != nullptr) return m;

    return m;
}

CastMember *CastLibs::member_or_throw(const std::string &_name) const {
    CastMember *m = nullptr;

    m = editor.member(_name);
    if (m != nullptr) return m;

    m = internal.member(_name);
    if (m != nullptr) return m;

    m = drought.member(_name);
    if (m != nullptr) return m;

    m = dry.member(_name);
    if (m != nullptr) return m;

    m = msc.member(_name);
    if (m != nullptr) return m;

    throw std::runtime_error(
        std::string("cast member '"+_name+"' not found in any library")
    );
}

CastLibs::CastLibs(std::filesystem::path const &cast_dir) :
    editor   ("levelEditor"  , cast_dir),
    internal ("Internal"     , cast_dir),
    drought  ("Drought"      , cast_dir),
    dry      ("DryEditor"    , cast_dir),
    msc      ("MSC"          , cast_dir) 
{}

CastLibs::~CastLibs() {}

};