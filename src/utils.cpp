#include <raylib.h>

#include <MobitRenderer/utils.h>

namespace mr::utils {

void unload_shader(Shader &shader) {
    if (shader.id == 0) return;
    UnloadShader(shader);
    shader = Shader{0};
}

void unload_texture(Texture2D &texture) {
    if (texture.id == 0) return;
    UnloadTexture(texture);
    texture = Texture2D{0};
}

};