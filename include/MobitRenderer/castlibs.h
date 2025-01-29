#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>

#include <raylib.h>

namespace mr {

class CastMember {
    
private:
    
    const std::string name;
    const std::string library;
    const std::filesystem::path texture_path;
    Texture2D texture;
    bool loaded;

public:

    inline const std::string &get_name() const noexcept { return name; }
    inline const std::string &get_library() const noexcept { return library; }
    inline const std::filesystem::path &get_texture_path() const noexcept { return texture_path; }

    inline bool is_loaded() const noexcept { return loaded; }
    inline const Texture2D &get_texture() const noexcept { return texture; }
    inline const Texture2D &get_loaded_texture() {
        if (!loaded) {
            texture = LoadTexture(texture_path.string().c_str());
        }
        return texture;
    }
    inline void unload_texture() {
        if (!loaded) return;
        UnloadTexture(texture);
        loaded = false;
        texture = Texture2D{0};
    }
    inline void reload_texture() {
        unload_texture();
        texture = LoadTexture(texture_path.string().c_str());
        loaded = true;
    }

    CastMember &operator=(CastMember const&) = delete;
    CastMember &operator=(CastMember&&) noexcept = delete;
    
    CastMember(
        std::string const &name,
        std::string const &library,
        std::filesystem::path const &texture_path
    );
    CastMember(std::filesystem::path const &texture_path);
    CastMember(CastMember const&) = delete;
    CastMember(CastMember&&) noexcept = delete;
    ~CastMember();

};

class CastLib {

private:

    const std::string name;
    const std::filesystem::path directory;
    std::unordered_map<std::string, CastMember*> members;
    bool loaded;

public:

    inline const std::string &get_name() const noexcept { return name; }
    inline const std::filesystem::path &get_directory() const noexcept { return directory; }
    inline const std::unordered_map<std::string, CastMember*> &get_members() const noexcept { return members;  }
    inline bool is_loaded() const noexcept { return loaded; }

    /// @brief Looks up for a cast member by name.
    /// @return A pointer to the member if found; otherwise a nullptr is returned.
    inline CastMember *member(const std::string &name) const noexcept {
        auto iter = members.find(name);
        if (iter == members.end()) return nullptr;
        return iter->second;
    }

    /// @brief Looks up for a cast member by name.
    /// @return A pointer to the cast member.
    /// @throw std::runtime_error if the member is not found.
    CastMember *member_or_throw(const std::string &name) const;

    inline void unload_members() {
        if (!loaded) return;
        for (auto &m : members) delete m.second;
        members.clear();
        loaded = false;
    }

    /// @brief Loads members from the directory.
    /// @note The member's name must start with 
    /// the name of the library, followed by an offset
    /// number, then its name like so: Drought_98102_Stone.png
    void load_members();

    inline void reload_members() {
        unload_members();
        load_members();
    }

    CastLib(
        std::string const &name,
        std::filesystem::path const &directory
    );

    CastLib(
        std::string&&,
        std::filesystem::path const&
    );

    CastLib(
        const char*,
        std::filesystem::path const&
    );

    ~CastLib();

};

class CastLibs {

private:

    std::filesystem::path cast_dir;
    bool registered;
    std::unordered_map<std::string, CastLib*> _libs;

public:

    inline const std::filesystem::path &get_cast_dir() const noexcept { return cast_dir; }
    inline void set_cast_dir(std::filesystem::path &dir) { cast_dir = dir; }
    inline bool are_libs_registered() const noexcept { return registered; }
    CastLib *lib(std::string const&) const noexcept;
    CastLib *lib_or_throw(std::string const&) const;
    inline const std::unordered_map<std::string, CastLib*> &libs() const noexcept { return _libs; }    

    /// @brief Looks up a member in all libraries.
    /// @return returns the first instance with a matching name. 
    CastMember *member(const std::string &name) const noexcept;

    /// @brief Looks up a member in all libraries.
    /// @return returns the first instance with a matching name. 
    /// @throw std::runtime_error if the member is not found in any library.
    CastMember *member_or_throw(const std::string &name) const;

    void load_all_members();
    void unload_all_members();
    void reload_all_members();

    void unregister_all();
    void register_all();

    CastLibs &operator=(CastLibs&&) noexcept = delete;
    CastLibs &operator=(CastLibs const&) = delete;

    CastLibs(std::filesystem::path const &cast_dir);
    CastLibs(std::filesystem::path &&cast_dir);
    CastLibs(CastLibs&&) noexcept = delete;
    CastLibs(CastLibs const&) = delete;
    ~CastLibs();

};

};