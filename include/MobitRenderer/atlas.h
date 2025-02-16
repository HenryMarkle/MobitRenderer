#pragma once

#include <unordered_map>
#include <filesystem>
#include <vector>
#include <memory>

#include <MobitRenderer/managed.h>
#include <MobitRenderer/dirs.h>

namespace mr {

class Atlas {

protected:

std::shared_ptr<Dirs> _directories;

public:

virtual void reload() = 0;

Atlas &operator=(Atlas&&) noexcept;
Atlas &operator=(const Atlas&) = delete;

Atlas(std::shared_ptr<Dirs>);
Atlas(Atlas&&) noexcept;
Atlas(const Atlas&) = delete;
virtual ~Atlas() = default;

};

class GE_Textures : public Atlas {

private:

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

    texture 
        _ui_category_one,
        _ui_category_two,
        _ui_category_three,
        _ui_category_four,
        _ui_crack,
        _ui_path,
        _ui_platform;

    std::unordered_map<uint8_t, Texture2D const*> _cracked_map;

public:

    inline const Texture2D &bathive() const noexcept { return _bathive.get();  } 
    inline const Texture2D &forbid() const noexcept { return _forbid.get();  } 
    inline const Texture2D &wormgrass() const noexcept { return _wormgrass.get();  } 
    inline const Texture2D &rock() const noexcept { return _rock.get();  } 
    inline const Texture2D &spear() const noexcept { return _spear.get();  } 
    inline const Texture2D &waterfall() const noexcept { return _waterfall.get();  }

    inline const Texture2D &passage() const noexcept { return _passage.get(); } 
    inline const Texture2D &garbageworm() const noexcept { return _garbage_worm.get(); } 
    inline const Texture2D &scav() const noexcept { return _scav.get(); } 
    inline const Texture2D &den() const noexcept { return _den.get(); } 
    inline const Texture2D &wack() const noexcept { return _wack.get(); }

    inline const Texture2D &entry_loose() const noexcept { return _entry_loose.get(); }
    inline const Texture2D &entry_left() const noexcept { return _entry_l.get(); }
    inline const Texture2D &entry_top() const noexcept { return _entry_t.get(); }
    inline const Texture2D &entry_right() const noexcept { return _entry_r.get(); }
    inline const Texture2D &entry_bottom() const noexcept { return _entry_b.get(); }

    inline const Texture2D &crack() const noexcept { return _crack.get(); }
    inline const Texture2D &crack_top() const noexcept { return _crack_t.get(); }
    inline const Texture2D &crack_right() const noexcept { return _crack_r.get(); }
    inline const Texture2D &crack_bottom() const noexcept { return _crack_b.get(); }
    inline const Texture2D &crack_left() const noexcept { return _crack_l.get(); }
    inline const Texture2D &crack_vertical() const noexcept { return _crack_v.get(); }
    inline const Texture2D &crack_horizontal() const noexcept { return _crack_h.get(); }
    inline const Texture2D &crack_bl() const noexcept { return _crack_bl.get(); }
    inline const Texture2D &crack_tl() const noexcept { return _crack_tl.get(); }
    inline const Texture2D &crack_br() const noexcept { return _crack_br.get(); }
    inline const Texture2D &crack_tr() const noexcept { return _crack_tr.get(); }
    inline const Texture2D &crack_blt() const noexcept { return _crack_blt.get(); }
    inline const Texture2D &crack_ltr() const noexcept { return _crack_ltr.get(); }
    inline const Texture2D &crack_trb() const noexcept { return _crack_trb.get(); }
    inline const Texture2D &crack_rbl() const noexcept { return _crack_rbl.get(); }

    inline const Texture2D &ui_category_one() const noexcept { return _ui_category_one.get(); }
    inline const Texture2D &ui_category_two() const noexcept { return _ui_category_two.get(); }
    inline const Texture2D &ui_category_three() const noexcept { return _ui_category_three.get(); }
    inline const Texture2D &ui_category_four() const noexcept { return _ui_category_four.get(); }
    inline const Texture2D &ui_crack() const noexcept { return _ui_crack.get(); }
    inline const Texture2D &ui_path() const noexcept { return _ui_path.get(); }
    inline const Texture2D &ui_platform() const noexcept { return _ui_platform.get(); }

    inline const std::unordered_map<uint8_t, Texture2D const*> &cracked_map() const noexcept { return _cracked_map; }

    void reload() override;

    GE_Textures &operator=(GE_Textures &&) noexcept;
    GE_Textures &operator=(GE_Textures const&) = delete;

    GE_Textures(std::shared_ptr<Dirs> dirs);
    GE_Textures(GE_Textures &&) noexcept;
    GE_Textures(GE_Textures const&) = delete;
    ~GE_Textures();

};

class LE_Textures : public Atlas {

private:

    std::vector<texture> _brushes;

public:

    inline const std::vector<texture> &brushes() const noexcept { return _brushes; }

    void reload() override;

    LE_Textures &operator=(LE_Textures &&) noexcept;

    LE_Textures(std::shared_ptr<Dirs> dirs);
    LE_Textures(LE_Textures &&) noexcept;
    ~LE_Textures();

};

class CE_Textures : public Atlas {

private:

    texture _camera_graf;

public:

    inline const Texture2D &get_camera_graf() const noexcept { return _camera_graf.get(); }

    void reload() override;

    CE_Textures &operator=(CE_Textures&&) noexcept;

    CE_Textures(std::shared_ptr<Dirs> dirs);
    CE_Textures(CE_Textures&&) noexcept;
    ~CE_Textures();

};

};