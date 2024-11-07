#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include <raylib.h>

#include <MobitRenderer/matrix.h>

namespace mr {
    struct LevelCamera {
        Vector2 position;

        int   top_left_angle,  top_right_angle,  bottom_right_angle,  bottom_left_angle;
        float top_left_radius, top_right_radius, bottom_right_radius, bottom_left_radius;

        inline void reset_angles() { top_left_angle  = top_right_angle  = bottom_right_angle  = bottom_left_angle  = 0; }
        inline void reset_radius() { top_left_radius = top_right_radius = bottom_right_radius = bottom_left_radius = 0; }
        inline void reset()        { reset_angles(); reset_radius(); }
    };

    struct EffectConfig {
        std::string name;
        std::vector<std::string> options;

        /// @brief Determines the type of the choice field.
        uint8_t choice_type;
        
        /// @brief If choice_type is 0, then choice is an index to options pointing to the choice; 
        /// otherwise, choice is just a number.
        uint8_t choice;
    };

    struct Effect {
        std::string name;
        std::vector<EffectConfig> config;
        Matrix<float> matrix;

        Effect& operator=(const Effect&) noexcept = delete;
        Effect(const Effect&) = delete;
    };

    /// @brief Stores data of a level project
    class Level {
        private:

        uint16_t width, height;

        Matrix<GeoCell>   geo_matrix;
        Matrix<TileCell> tile_matrix;
        std::vector<Effect>  effects;

        RenderTexture2D lightmap;

        public:

        int8_t water;
        bool light, terrain, front_water;
        std::vector<LevelCamera> cameras;
        
        uint16_t get_width()  const;
        uint16_t get_height() const;

        Matrix<GeoCell>&     get_geo_matrix()   const;
        Matrix<TileCell>&    get_tile_matrix()  const;
        std::vector<Effect>& get_effects()      const;
        
        void begin_lightmap_mode();
        void   end_lightmap_mode();

        const Matrix<GeoCell>&     get_const_geo_matrix()   const;
        const Matrix<TileCell>&    get_const_tile_matrix()  const;
        const std::vector<Effect>& get_const_effects()      const;

        void resize(int16_t left, int16_t top, int16_t right, int16_t bottom);
        
        Level() = delete;
        Level(
            uint16_t  width, 
            uint16_t height, 

            Matrix<GeoCell>&&          geo_matrix, 
            Matrix<TileCell>&&         tile_matrix, 
            std::vector<Effect>&&      effects, 
            std::vector<LevelCamera>&& cameras,
            
            Texture2D lightmap,

            int8_t water     = -1,
            bool light       = true,
            bool terrain     = false,
            bool front_water = false
        );
        ~Level();
    };
};
