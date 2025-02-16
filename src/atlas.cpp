#include <filesystem>
#include <vector>
#include <memory>

#ifdef IS_DEBUG_BUILD
#include <iostream>
#endif

#include <MobitRenderer/atlas.h>
#include <MobitRenderer/managed.h>

namespace mr {

Atlas::Atlas(std::shared_ptr<Dirs> dirs) : _directories(dirs) {}
Atlas::Atlas(Atlas &&other) noexcept : _directories(std::move(other._directories)) {}
Atlas &Atlas::operator=(Atlas &&other) noexcept {
  if (this == &other) return *this;
  _directories = std::move(other._directories);
  return *this;
}

void GE_Textures::reload() {
  const auto &textures_dir = _directories->get_assets() / "Level" / "Geometry";

  _bathive    = texture((textures_dir /         "bathive.png").string().c_str());
  _forbid     = texture((textures_dir / "forbidflychains.png").string().c_str());
  _wormgrass  = texture((textures_dir /            "worm.png").string().c_str());
  _rock       = texture((textures_dir /            "rock.png").string().c_str());
  _spear      = texture((textures_dir /           "spear.png").string().c_str());
  _waterfall  = texture((textures_dir /       "waterfall.png").string().c_str());


  _passage      = texture((textures_dir /     "passage.png").string().c_str());
  _garbage_worm = texture((textures_dir / "garbageworm.png").string().c_str());
  _scav         = texture((textures_dir /        "scav.png").string().c_str());
  _den          = texture((textures_dir /         "den.png").string().c_str());
  _wack         = texture((textures_dir /        "wack.png").string().c_str());

  _entry_loose  = texture((textures_dir / "entryloose.png").string().c_str());
  _entry_l      = texture((textures_dir / "entryl.png").string().c_str());
  _entry_t      = texture((textures_dir / "entryt.png").string().c_str());
  _entry_b      = texture((textures_dir / "entryb.png").string().c_str());
  _entry_r      = texture((textures_dir / "entryr.png").string().c_str());

  _crack      = texture((textures_dir /   "cracku.png").string().c_str());
  _crack_c    = texture((textures_dir /   "crackc.png").string().c_str());
  _crack_t    = texture((textures_dir /   "crackt.png").string().c_str());
  _crack_b    = texture((textures_dir /   "crackb.png").string().c_str());
  _crack_l    = texture((textures_dir /   "crackl.png").string().c_str());
  _crack_r    = texture((textures_dir /   "crackr.png").string().c_str());
  _crack_h    = texture((textures_dir /   "crackh.png").string().c_str());
  _crack_v    = texture((textures_dir /   "crackv.png").string().c_str());
  _crack_bl   = texture((textures_dir /  "crackbl.png").string().c_str());
  _crack_tl   = texture((textures_dir /  "cracktl.png").string().c_str());
  _crack_tr   = texture((textures_dir /  "cracktr.png").string().c_str());
  _crack_br   = texture((textures_dir /  "crackbr.png").string().c_str());
  _crack_blt  = texture((textures_dir / "crackblt.png").string().c_str());
  _crack_ltr  = texture((textures_dir / "crackltr.png").string().c_str());
  _crack_trb  = texture((textures_dir / "cracktrb.png").string().c_str());
  _crack_rbl  = texture((textures_dir / "crackrbl.png").string().c_str());

  _ui_category_one = texture((textures_dir / "Interface" / "category 1.png").string().c_str());
  _ui_category_two = texture((textures_dir / "Interface" / "category 2.png").string().c_str());
  _ui_category_three = texture((textures_dir / "Interface" / "category 3.png").string().c_str());
  _ui_category_four = texture((textures_dir / "Interface" / "category 4.png").string().c_str());
  _ui_crack = texture((textures_dir / "Interface" / "crack.png").string().c_str());
  _ui_path = texture((textures_dir / "Interface" / "path.png").string().c_str());
  _ui_platform = texture((textures_dir / "Interface" / "platform.png").string().c_str());

  {
    _cracked_map.clear();
  
    static const uint8_t left   =  2;
    static const uint8_t top    =  4;
    static const uint8_t right  =  8;
    static const uint8_t bottom = 16;

    _cracked_map[0] = _crack.get_ptr();
    _cracked_map[left] = _crack_l.get_ptr();
    _cracked_map[top] = _crack_t.get_ptr();
    _cracked_map[right] = _crack_r.get_ptr();
    _cracked_map[bottom] = _crack_b.get_ptr();

    _cracked_map[left | top] = _crack_tl.get_ptr();
    _cracked_map[top | right] = _crack_tr.get_ptr();
    _cracked_map[right | bottom] = _crack_br.get_ptr();
    _cracked_map[bottom | left] = _crack_bl.get_ptr();

    _cracked_map[top | bottom] = _crack_v.get_ptr();
    _cracked_map[left | right] = _crack_h.get_ptr();

    _cracked_map[left | top | right] = _crack_ltr.get_ptr();
    _cracked_map[top | right | bottom] = _crack_trb.get_ptr();
    _cracked_map[right | bottom | left] = _crack_rbl.get_ptr();
    _cracked_map[bottom | left | top] = _crack_blt.get_ptr();

    _cracked_map[left | top | right | bottom] = _crack_c.get_ptr();
  }
}


GE_Textures::GE_Textures(std::shared_ptr<Dirs> dirs) : Atlas(dirs) {}

GE_Textures &GE_Textures::operator=(GE_Textures &&other) noexcept {
  if (this == &other)
    return *this;

  Atlas::operator=(std::move(other));

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

GE_Textures::GE_Textures(GE_Textures &&other) noexcept : Atlas(std::move(other)) {
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
  const auto &textures_dir = _directories->get_assets() / "Level" / "Light";

  if (!std::filesystem::exists(textures_dir) || !std::filesystem::is_directory(textures_dir)) {
    #ifdef IS_DEBUG_BUILD
    std::cout 
      << "Warning: light editor's textures directory does not exist: " 
      << textures_dir 
      << std::endl;
    #endif
    
    return;
  }

  _brushes.clear();

  for (const auto &entry : std::filesystem::directory_iterator(textures_dir)) {
    if (!std::filesystem::is_regular_file(entry.path())) continue;
    if (entry.path().extension() != ".png") continue;

    _brushes.push_back(texture(entry.path().string().c_str()));
  }
}

LE_Textures &LE_Textures::operator=(LE_Textures &&other) noexcept {
  if (this == &other) return *this;

  Atlas::operator=(std::move(other));

  _brushes = std::move(other._brushes);

  return *this;
}
LE_Textures::LE_Textures(std::shared_ptr<Dirs> dirs) : Atlas(dirs) {}
LE_Textures::LE_Textures(LE_Textures &&other) noexcept : Atlas(std::move(other)) {_brushes = std::move(other._brushes);}
LE_Textures::~LE_Textures() {}

void CE_Textures::reload() {
  const auto &textures_dir = _directories->get_assets() / "Level" / "Cameras";

  if (!std::filesystem::exists(textures_dir) || !std::filesystem::is_directory(textures_dir)) {
    #ifdef IS_DEBUG_BUILD
    std::cout 
      << "Warning: cameras editor's textures directory does not exist: " 
      << textures_dir 
      << std::endl;
    #endif
    
    return;
  }

  _camera_graf = texture((textures_dir / "graf.png").string().c_str());
}
CE_Textures &CE_Textures::operator=(CE_Textures &&other) noexcept {
  if (this == &other) return *this;

  Atlas::operator=(std::move(other));

  _camera_graf = std::move(other._camera_graf);

  return *this;
}
CE_Textures::CE_Textures(std::shared_ptr<Dirs> dirs) : Atlas(dirs) {}
CE_Textures::CE_Textures(CE_Textures &&other) noexcept : Atlas(std::move(other)) {
  _camera_graf = std::move(other._camera_graf);
}
CE_Textures::~CE_Textures() {}

};