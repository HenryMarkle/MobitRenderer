#pragma once

#include <bitset>
#include <cstdint>
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>

#include <MobitRenderer/definitions.h>

namespace mr {
    enum class GeoType : uint8_t
    {
        air                 = 0,
        solid               = 1,
        slope_ne            = 2,
        slope_nw            = 3,
        slope_es            = 4,
        slope_sw            = 5,
        platform            = 6,
        shortcut_entrance   = 7,
        glass               = 9
    };

    enum class GeoFeature : uint16_t
    {
        none               = 0,
        horizontal_pole    = 1 << 0,
        vertical_pole      = 1 << 1,
        bathive            = 1 << 2,
        shortcut_entrance  = 1 << 3,
        shortcut_path      = 1 << 4,
        room_entrance      = 1 << 5,
        dragon_den         = 1 << 6,
        place_rock         = 1 << 7,
        place_spear        = 1 << 8,
        cracked_terrain    = 1 << 9,
        forbid_fly_chains  = 1 << 10,
        garbage_wormHole   = 1 << 11,
        waterfall          = 1 << 12,
        wackA_moleHole     = 1 << 13,
        worm_grass         = 1 << 14,
        scavenger_hole     = 1 << 15,
    };

    inline GeoFeature operator|(GeoFeature lhs, GeoFeature rhs) {
        return static_cast<GeoFeature>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
    }

    inline GeoFeature operator&(GeoFeature lhs, GeoFeature rhs) {
        return static_cast<GeoFeature>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
    }

    inline GeoFeature operator^(GeoFeature lhs, GeoFeature rhs) {
        return static_cast<GeoFeature>(static_cast<uint8_t>(lhs) ^ static_cast<uint8_t>(rhs));
    }

    inline GeoFeature operator~(GeoFeature flag) {
        return static_cast<GeoFeature>(~static_cast<uint8_t>(flag));
    }

    // 4 bytes
    struct GeoCell {
        GeoType type;
        GeoFeature features;

        inline bool is_solid() { return type == GeoType::solid || type == GeoType::glass; }

        inline bool has_feature(GeoFeature feature)     { return (features & feature) == feature; }
        inline void switch_feature(GeoFeature feature)  { features = features ^ feature; }
        inline void enable(GeoFeature feature)          { features = features | feature; }
        inline void disable(GeoFeature feature)         { features = features & ~feature; }
        inline void clear_features()                    { features = GeoFeature::none; }
    };

    //

    enum TileType : uint8_t { _default, head, body, material };

    // 88 bytes
    struct TileCell {
        TileType type;

        std::string und_name;
        unsigned int head_pos_x, head_pos_y, head_pos_z;
    
        std::shared_ptr<TileDef> tile_def;
        std::shared_ptr<MaterialDef> material_def;
    };

    TileCell create_default_tile();
    TileCell create_head_tile(std::shared_ptr<TileDef> tile);
    TileCell create_body_tile(unsigned int x, unsigned int y, unsigned int z, std::shared_ptr<TileDef> tile = nullptr);
    TileCell create_material_tile(std::shared_ptr<MaterialDef> material);
    TileCell create_undefined_head_tile(std::string name);
    TileCell create_undefined_material_tile(std::string name);

    template <typename T>
    class Matrix {
        private:

        std::vector<T> matrix;          // Might change it to a shared pointer (shared_ptr<vector<T>>)
        uint16_t width, height, depth;

        uint64_t index(uint16_t x, uint16_t y, uint16_t z);

        public:

        uint16_t get_width()  const;
        uint16_t get_height() const;
        uint16_t get_depth()  const;

        bool is_in_bounds(uint16_t x, uint16_t y, uint16_t z) const;
        
        // Crash if index is out-of-bounds
        T& get(uint16_t x, uint16_t y, uint16_t z) const;
        const T& get_const(uint16_t x, uint16_t y, uint16_t z) const;

        // Return nullptr if index is out-of-bounds
        T* get_ptr(uint16_t x, uint16_t y, uint16_t z) const;
        const T* get_const_ptr(uint16_t x, uint16_t y, uint16_t z) const;

        void resize(int16_t left, int16_t top, int16_t right, int16_t bottom);

        Matrix<T>& operator=(const Matrix<T>&) = delete;
        Matrix<T>& operator=(Matrix<T>&&);

        Matrix(const Matrix<T>&) = delete;
        Matrix(Matrix<T>&&);

        Matrix(uint16_t width, uint16_t height, uint16_t depth = 3);
        ~Matrix();
    };

    template<typename T>
    uint64_t Matrix<T>::index(uint16_t x, uint16_t y, uint16_t z) { return x + (y * width) + (z * width * height); }

    template<typename T>
    Matrix<T>::Matrix(uint16_t _width, uint16_t _height, uint16_t _depth) {
        if (_width == 0 || _height == 0 || _depth == 0) throw std::invalid_argument("matrix dimensions cannot be zero");

        width = _width;
        height = _height;
        depth = _depth;

        matrix.reserve(width * height * depth);
    }

    template<typename T>
    Matrix<T>::Matrix(Matrix<T>&& m) : matrix(nullptr), width(0), height(0), depth(0) {
        matrix = std::move(m.matrix);
        
        width = m.width;
        height = m.height;
        depth = m.depth;

        m.width = 0;
        m.height = 0;
        m.depth = 0;
    }

    template<typename T>
    Matrix<T>& Matrix<T>::operator=(Matrix<T>&& other) {
        if (this == other) return *this;

        matrix = std::move(other.matrix);

        width = other.width;
        height = other.height;
        depth = other.depth;

        other.width = 0;
        other.height = 0;
        other.depth = 0;

        return *this;
    }

    template<typename T>
    Matrix<T>::~Matrix() { }

    template<typename T>
    uint16_t Matrix<T>::get_width() const { return width; }

    template<typename T>
    uint16_t Matrix<T>::get_height() const { return height; }

    template<typename T>
    uint16_t Matrix<T>::get_depth() const { return depth; }

    template<typename T>
    bool Matrix<T>::is_in_bounds(uint16_t x, uint16_t y, uint16_t z) const { 
        return x >= 0 && x < width &&
                y >= 0 && y < height &&
                z >= 0 && z < depth; 
    }

    template<typename T>
    T& Matrix<T>::get(uint16_t x, uint16_t y, uint16_t z) const {
        if (!is_in_bounds(x, y, z)) throw std::out_of_range("matrix index is out of bounds");

        return matrix[index(x, y, z)];
    }

    template<typename T>
    const T& Matrix<T>::get_const(uint16_t x, uint16_t y, uint16_t z) const {
        if (!is_in_bounds(x, y, z)) throw std::out_of_range("matrix index is out of bounds");

        return matrix[index(x, y, z)];
    }

    template<typename T>
    T* Matrix<T>::get_ptr(uint16_t x, uint16_t y, uint16_t z) const {
        if (!is_in_bounds(x, y, z)) return nullptr;

        return &matrix[index(x, y, z)];
    }

    template<typename T>
    const T* Matrix<T>::get_const_ptr(uint16_t x, uint16_t y, uint16_t z) const {
        if (!is_in_bounds(x, y, z)) return nullptr;

        return &matrix[index(x, y, z)];
    }

    // No depth resizing
    template<typename T>
    void Matrix<T>::resize(int16_t left, int16_t top, int16_t right, int16_t bottom) {
        if (left == 0 && top == 0 && right == 0 && bottom == 0) return;
        if (-left == width || -right == width || -top == height || -bottom == height) return;
    
        uint16_t new_width = width + left + right;
        uint16_t new_height = height + top + bottom;

        std::vector<T> new_matrix{};

        new_matrix.reserve(new_width * new_height * depth);

        // Copy over

        for (uint16_t x = 0; x < width; x++) {
            uint16_t new_x = x + left;

            if (new_x < 0 || new_x >= new_width) break;

            for (uint16_t y = 0; y < height; y++) {
                uint16_t new_y = y + top;

                if (new_y < 0 || new_y >= new_height) break;

                for (uint16_t z = 0; z < depth; z++) {
                    new_matrix[index(new_x, new_y, z)] = matrix[index(x, y, z)];
                }
            }
        }

        matrix = std::move(new_matrix);
    }
};