#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

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

void unload_rendertexture(RenderTexture2D &rt) {
  if (rt.id == 0) return;
  UnloadRenderTexture(rt);
  rt = {0};
}

};