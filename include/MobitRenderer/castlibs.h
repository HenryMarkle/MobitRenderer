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
        std::string const&name,
        std::filesystem::path const &directory
    );

    ~CastLib();

};

class CastLibs {

private:

    CastLib editor, internal, drought, dry, msc;

public:

    inline CastLib &get_editor() noexcept { return editor; }
    inline CastLib &get_internal() noexcept { return internal; }
    inline CastLib &get_drought() noexcept { return drought; }
    inline CastLib &get_dry() noexcept { return dry; }
    inline CastLib &get_msc() noexcept { return msc; }

    /// @brief Looks up a member in all libraries.
    /// @return returns the first instance with a matching name. 
    CastMember *member(const std::string &name) const noexcept;

    /// @brief Looks up a member in all libraries.
    /// @return returns the first instance with a matching name. 
    /// @throw std::runtime_error if the member is not found in any library.
    CastMember *member_or_throw(const std::string &name) const;

    inline void load_all() {
        editor.load_members();
        internal.load_members();
        drought.load_members();
        dry.load_members();
        msc.load_members();
    }
    
    inline void unload_all() {
        editor.unload_members();
        internal.unload_members();
        drought.unload_members();
        dry.unload_members();
        msc.unload_members();
    }

    inline void reload_all() {
        editor.reload_members();
        internal.reload_members();
        drought.reload_members();
        dry.reload_members();
        msc.reload_members();
    }

    CastLibs &operator=(CastLibs&&) noexcept = delete;
    CastLibs &operator=(CastLibs const&) = delete;

    CastLibs(std::filesystem::path const &cast_dir);
    CastLibs(CastLibs&&) noexcept = delete;
    CastLibs(CastLibs const&) = delete;
    ~CastLibs();

};

};