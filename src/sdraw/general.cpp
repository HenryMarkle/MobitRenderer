#include <cstdint>
#include <memory>
#include <queue>
#include <string>

#include <raylib.h>
#include <rlgl.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/atlas.h>
#include <MobitRenderer/dex.h>
#include <MobitRenderer/draw.h>
#include <MobitRenderer/sdraw.h>

namespace mr::sdraw {

void draw_texture_darkest(
    const Texture2D &texture, 
    const Rectangle &source, 
    const Rectangle &destination
) noexcept {
    BeginBlendMode(BLEND_CUSTOM);
    rlSetBlendFactors(1, 1, 0x8007);
    DrawTexturePro(
        texture,
        source,
        destination,
        Vector2{0, 0},
        0,
        WHITE
    );
    EndBlendMode();
}

};