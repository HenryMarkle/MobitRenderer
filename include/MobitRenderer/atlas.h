#pragma once

#include <filesystem>
#include <vector>

#include <MobitRenderer/managed.h>

namespace mr {

class GE_Textures {

private:

std::filesystem::path _textures_dir;

texture _bathive, _forbid, _wormgrass, _rock, _spear, _waterfall;
texture _passage, _garbage_worm, _scav,  _den, _wack;
texture _entry_loose, _entry_l, _entry_t, _entry_r, _entry_b;

texture 
    _crack,
    
    _crack_t,
    _crack_b,
    _crack_l,
    _crack_r,
    _crack_c,

    _crack_h,
    _crack_v,
    
    _crack_bl,
    _crack_tl,
    _crack_br,
    _crack_tr,
    
    _crack_blt,
    _crack_ltr,
    _crack_trb,
    _crack_rbl;

public:

const texture &bathive() const; 
const texture &forbid() const; 
const texture &wormgrass() const; 
const texture &rock() const; 
const texture &spear() const; 
const texture &waterfall() const;

const texture &passage() const; 
const texture &garbageworm() const; 
const texture &scav() const; 
const texture &den() const; 
const texture &wack() const;

const texture &entry_loose() const;
const texture &entry_left() const;
const texture &entry_top() const;
const texture &entry_right() const;
const texture &entry_bottom() const;

const texture &crack() const;
const texture &crack_top() const;
const texture &crack_right() const;
const texture &crack_bottom() const;
const texture &crack_left() const;
const texture &crack_vertical() const;
const texture &crack_horizontal() const;
const texture &crack_bl() const;
const texture &crack_tl() const;
const texture &crack_br() const;
const texture &crack_tr() const;
const texture &crack_blt() const;
const texture &crack_ltr() const;
const texture &crack_trb() const;
const texture &crack_rbl() const;

void reload();

GE_Textures &operator=(GE_Textures &&) noexcept;
GE_Textures &operator=(GE_Textures const&) = delete;

GE_Textures(const std::filesystem::path &assets_dir);
GE_Textures(GE_Textures &&) noexcept;
GE_Textures(GE_Textures const&) = delete;
~GE_Textures();

};

class LE_Textures {

private:

    std::filesystem::path _textures_dir;
    std::vector<texture> _brushes;

public:

    inline const std::vector<texture> &brushes() const noexcept { return _brushes; }

    void reload();

    LE_Textures &operator=(LE_Textures &&) noexcept;
    LE_Textures &operator=(const LE_Textures&) = delete;

    LE_Textures(const std::filesystem::path &assets_dir);
    LE_Textures(LE_Textures &&) noexcept;
    LE_Textures(const LE_Textures&) = delete;
    ~LE_Textures();

};

};