#include <filesystem>
#include <vector>
#include <memory>

#ifdef IS_DEBUG_BUILD
#include <iostream>
#endif

#include <MobitRenderer/atlas.h>
#include <MobitRenderer/managed.h>

namespace mr {

const texture &GE_Textures::bathive() const { return _bathive; }
const texture &GE_Textures::forbid() const { return _forbid ; }
const texture &GE_Textures::wormgrass() const { return _wormgrass ; }
const texture &GE_Textures::rock() const { return _rock ; }
const texture &GE_Textures::spear() const { return _spear ; }
const texture &GE_Textures::waterfall() const { return _waterfall ; }

const texture &GE_Textures::passage() const { return _passage ; }
const texture &GE_Textures::garbageworm() const { return _garbage_worm; }
const texture &GE_Textures::scav() const { return _scav; }
const texture &GE_Textures::den() const { return _den; }
const texture &GE_Textures::wack() const { return _wack; }

const texture &GE_Textures::entry_loose() const { return _entry_loose;}
const texture &GE_Textures::entry_left() const { return _entry_l;}
const texture &GE_Textures::entry_top() const { return _entry_t;}
const texture &GE_Textures::entry_right() const { return _entry_r;}
const texture &GE_Textures::entry_bottom() const { return _entry_b;}

const texture &GE_Textures::crack() const { return _crack; }
const texture &GE_Textures::crack_top() const { return _crack_t; }
const texture &GE_Textures::crack_right() const { return _crack_r; }
const texture &GE_Textures::crack_bottom() const { return _crack_b; }
const texture &GE_Textures::crack_left() const { return _crack_l; }
const texture &GE_Textures::crack_vertical() const { return _crack_v; }
const texture &GE_Textures::crack_horizontal() const { return _crack_h; }
const texture &GE_Textures::crack_bl() const { return _crack_bl; }
const texture &GE_Textures::crack_tl() const { return _crack_tl; }
const texture &GE_Textures::crack_br() const { return _crack_br; }
const texture &GE_Textures::crack_tr() const { return _crack_tr; }
const texture &GE_Textures::crack_blt() const { return _crack_blt; }
const texture &GE_Textures::crack_ltr() const { return _crack_ltr; }
const texture &GE_Textures::crack_trb() const { return _crack_trb; }
const texture &GE_Textures::crack_rbl() const { return _crack_rbl; }

void GE_Textures::reload() {
  _bathive    = texture((_textures_dir /         "bathive.png").string().c_str());
  _forbid     = texture((_textures_dir / "forbidflychains.png").string().c_str());
  _wormgrass  = texture((_textures_dir /            "worm.png").string().c_str());
  _rock       = texture((_textures_dir /            "rock.png").string().c_str());
  _spear      = texture((_textures_dir /           "spear.png").string().c_str());
  _waterfall  = texture((_textures_dir /       "waterfall.png").string().c_str());


  _passage      = texture((_textures_dir /     "passage.png").string().c_str());
  _garbage_worm = texture((_textures_dir / "garbageworm.png").string().c_str());
  _scav         = texture((_textures_dir /        "scav.png").string().c_str());
  _den          = texture((_textures_dir /         "den.png").string().c_str());
  _wack         = texture((_textures_dir /        "wack.png").string().c_str());

  _entry_loose  = texture((_textures_dir / "entryloose.png").string().c_str());
  _entry_l      = texture((_textures_dir / "entryl.png").string().c_str());
  _entry_t      = texture((_textures_dir / "entryt.png").string().c_str());
  _entry_b      = texture((_textures_dir / "entryb.png").string().c_str());
  _entry_r      = texture((_textures_dir / "entryr.png").string().c_str());

  _crack      = texture((_textures_dir /   "cracku.png").string().c_str());
  _crack_t    = texture((_textures_dir /   "crackt.png").string().c_str());
  _crack_b    = texture((_textures_dir /   "crackb.png").string().c_str());
  _crack_l    = texture((_textures_dir /   "crackl.png").string().c_str());
  _crack_r    = texture((_textures_dir /   "crackr.png").string().c_str());
  _crack_h    = texture((_textures_dir /   "crackh.png").string().c_str());
  _crack_v    = texture((_textures_dir /   "crackv.png").string().c_str());
  _crack_bl   = texture((_textures_dir /  "crackbl.png").string().c_str());
  _crack_tl   = texture((_textures_dir /  "cracktl.png").string().c_str());
  _crack_tr   = texture((_textures_dir /  "cracktr.png").string().c_str());
  _crack_br   = texture((_textures_dir /  "crackbr.png").string().c_str());
  _crack_blt  = texture((_textures_dir / "crackblt.png").string().c_str());
  _crack_ltr  = texture((_textures_dir / "crackltr.png").string().c_str());
  _crack_trb  = texture((_textures_dir / "cracktrb.png").string().c_str());
  _crack_rbl  = texture((_textures_dir / "crackrbl.png").string().c_str());
}


GE_Textures::GE_Textures(const std::filesystem::path &d) {
  _textures_dir = d / "Level" / "Geometry";
}

GE_Textures &GE_Textures::operator=(GE_Textures &&other) noexcept {
  if (this == &other)
    return *this;

  _textures_dir = std::move(other._textures_dir);

  _bathive = std::move(other._bathive);
  _forbid = std::move(other._forbid);
  _wormgrass = std::move(other._wormgrass);
  _rock = std::move(other._rock);
  _spear = std::move(other._spear);
  _waterfall = std::move(other._waterfall);

  _passage = std::move(other._passage);
  _garbage_worm = std::move(other._garbage_worm);
  _scav = std::move(other._scav);
  _den = std::move(other._den);
  _wack = std::move(other._wack);

  _crack = std::move(other._crack);
  _crack_t = std::move(other._crack_t);
  _crack_b = std::move(other._crack_b);
  _crack_l = std::move(other._crack_l);
  _crack_r = std::move(other._crack_r);
  _crack_h = std::move(other._crack_h);
  _crack_v = std::move(other._crack_v);
  _crack_bl = std::move(other._crack_bl);
  _crack_tl = std::move(other._crack_tl);
  _crack_tr = std::move(other._crack_tr);
  _crack_br = std::move(other._crack_br);
  _crack_blt = std::move(other._crack_blt);
  _crack_ltr = std::move(other._crack_ltr);
  _crack_trb = std::move(other._crack_trb);
  _crack_rbl = std::move(other._crack_rbl);

  return *this;
}

GE_Textures::GE_Textures(GE_Textures &&other) noexcept {
  _textures_dir = std::move(other._textures_dir);

  _bathive = std::move(other._bathive);
  _forbid = std::move(other._forbid);
  _wormgrass = std::move(other._wormgrass);
  _rock = std::move(other._rock);
  _spear = std::move(other._spear);
  _waterfall = std::move(other._waterfall);

  _passage = std::move(other._passage);
  _garbage_worm = std::move(other._garbage_worm);
  _scav = std::move(other._scav);
  _den = std::move(other._den);
  _wack = std::move(other._wack);

  _crack = std::move(other._crack);
  _crack_t = std::move(other._crack_t);
  _crack_b = std::move(other._crack_b);
  _crack_l = std::move(other._crack_l);
  _crack_r = std::move(other._crack_r);
  _crack_h = std::move(other._crack_h);
  _crack_v = std::move(other._crack_v);
  _crack_bl = std::move(other._crack_bl);
  _crack_tl = std::move(other._crack_tl);
  _crack_tr = std::move(other._crack_tr);
  _crack_br = std::move(other._crack_br);
  _crack_blt = std::move(other._crack_blt);
  _crack_ltr = std::move(other._crack_ltr);
  _crack_trb = std::move(other._crack_trb);
  _crack_rbl = std::move(other._crack_rbl);
}

GE_Textures::~GE_Textures() {}


void LE_Textures::reload() {
  if (!std::filesystem::is_directory(_textures_dir)) {
    #ifdef IS_DEBUG_BUILD
    std::cout 
      << "Warning: light editor's textures directory does not exist: " 
      << _textures_dir 
      << std::endl;
    #endif
    
    return;
  }

  _brushes.clear();

  for (const auto &entry : std::filesystem::directory_iterator(_textures_dir)) {
    if (!std::filesystem::is_regular_file(entry.path())) continue;
    if (entry.path().extension() != ".png") continue;

    _brushes.push_back(texture(entry.path().string().c_str()));
  }
}

LE_Textures &LE_Textures::operator=(LE_Textures &&other) noexcept {
  if (this == &other) return *this;

  _brushes = std::move(other._brushes);

  return *this;
}
LE_Textures::LE_Textures(const std::filesystem::path &path) {
  _textures_dir = path / "Level" / "Light";
}
LE_Textures::LE_Textures(LE_Textures &&other) noexcept {
  _brushes = std::move(other._brushes);
}
LE_Textures::~LE_Textures() {}

};