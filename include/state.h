#pragma once

#include <memory>
#include <filesystem>

#include "spdlog/spdlog.h"

#include "raylib.h"

#ifdef __linux__
#include <unistd.h>
#include <limits.h>
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

namespace mr {
    inline std::filesystem::path get_current_dir() {
        std::filesystem::path this_dir;

        #ifdef __linux__
        {
            char result[PATH_MAX];
            ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
            
            if (count == -1) throw "could not retrieve executable's path";
            result[count] = '\0';
            this_dir = std::filesystem::absolute(result).parent_path();
        }
        #endif

        #ifdef __APPLE__
        throw mr::unsupported();
        #endif

        #ifdef _WIN32
        throw mr::unsupported();
        #endif

        return this_dir;
    }

    class dirs {
    private:
        std::filesystem::path executable;
        std::filesystem::path assets, projects, levels, data, logs; 
        std::filesystem::path shaders, materials, tiles, props, cast;

    public:

        const std::filesystem::path& get_executable() const;

        const std::filesystem::path& get_assets() const;
        const std::filesystem::path& get_projects() const;
        const std::filesystem::path& get_levels() const;
        const std::filesystem::path& get_data() const;
        const std::filesystem::path& get_logs() const;

        const std::filesystem::path& get_shaders() const;
        const std::filesystem::path& get_materials() const;
        const std::filesystem::path& get_tiles() const;
        const std::filesystem::path& get_props() const;
        const std::filesystem::path& get_cast() const;

        dirs();
        dirs(const std::filesystem::path&);
    };

    class Textures {
        private:
        
        RenderTexture2D main_level_viewport;

        public:

        /// @brief Returns a read-only reference to the main level viewport.
        /// @warning Do not access this while modifying main_level_viewport.
        const RenderTexture2D& get_main_level_viewport() const;

        /// @brief Replaces the existing render texture with a new one.
        void new_main_level_viewport(int width, int height);

        /// @brief Creates a new render texture and draws the old render texture over.
        void resize_main_level_viewport(int width, int height);

        Textures(const dirs& _dirs);
        ~Textures();
    };

    class context {
        private:

        std::shared_ptr<spdlog::logger> logger;
        dirs directories;
        Textures textures;
        Camera2D camera;

        public:

        spdlog::logger* get_logger() const;
        void set_logger(const std::shared_ptr<spdlog::logger>);

        const dirs& get_dirs() const;
        void set_dirs(dirs);

        Camera2D& get_camera();
        void set_camera(Camera2D);

        Textures& get_textures();

        context();
        ~context();
    };
};