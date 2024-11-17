
#include <any>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <queue>
#include <string>
#include <typeinfo>

#include <raylib.h>

#include <MobitRenderer/draw.h>

namespace mr::debug {

void f3::newline(uint32_t lines) noexcept {
  if (lines == 0)
    return;

  cursor.y += lines * font_size + margin_h * 2;
}
void f3::reset() noexcept { cursor = Vector2{0, 0}; }
void f3::set_font(Font font) noexcept { this->font = font; }

void f3::print(const char *str, bool same_line) noexcept {
  auto measured = MeasureTextEx(font, str, font_size, 0.3f);
  auto with_margin =
      Vector2{.x = measured.x + 2 * margin_v, .y = measured.y + 2 * margin_h};

  if (!same_line) {
    cursor.y += with_margin.y;
  }

  DrawRectangleRec(Rectangle{.x = cursor.x,
                             .y = cursor.y,
                             .width = with_margin.x,
                             .height = with_margin.y},
                   background);

  DrawTextEx(font, str,
             Vector2{.x = cursor.x + margin_v, .y = cursor.y + margin_h},
             font_size, 0.3f, WHITE);

  cursor.x += with_margin.x;
}
void f3::print(const std::string &str, bool same_line) noexcept {
  auto measured = MeasureTextEx(font, str.c_str(), font_size, 0.3f);
  auto with_margin =
      Vector2{.x = measured.x + 2 * margin_v, .y = measured.y + 2 * margin_h};

  if (!same_line) {
    cursor.y += with_margin.y;
  }

  DrawRectangleRec(Rectangle{.x = cursor.x,
                             .y = cursor.y,
                             .width = with_margin.x,
                             .height = with_margin.y},
                   background);

  DrawTextEx(font, str.c_str(),
             Vector2{.x = cursor.x + margin_v, .y = cursor.y + margin_h},
             font_size, 0.3f, ORANGE);

  cursor.x += with_margin.x;
}
void f3::print(int number, bool same_line) noexcept {
  const char *formatted = TextFormat("%d", number);
  auto measured = MeasureTextEx(font, formatted, font_size, 0.3f);
  auto with_margin =
      Vector2{.x = measured.x + 2 * margin_v, .y = measured.y + 2 * margin_h};

  if (!same_line) {
    cursor.y += with_margin.y;
  }

  DrawRectangleRec(Rectangle{.x = cursor.x,
                             .y = cursor.y,
                             .width = with_margin.x,
                             .height = with_margin.y},
                   background);

  DrawTextEx(font, formatted,
             Vector2{.x = cursor.x + margin_v, .y = cursor.y + margin_h},
             font_size, 0.3f, SKYBLUE);

  cursor.x += with_margin.x;
}
void f3::print(size_t number, bool same_line) noexcept {
  const char *formatted = TextFormat("%ld", number);
  auto measured = MeasureTextEx(font, formatted, font_size, 0.3f);
  auto with_margin =
      Vector2{.x = measured.x + 2 * margin_v, .y = measured.y + 2 * margin_h};

  if (!same_line) {
    cursor.y += with_margin.y;
  }

  DrawRectangleRec(Rectangle{.x = cursor.x,
                             .y = cursor.y,
                             .width = with_margin.x,
                             .height = with_margin.y},
                   background);

  DrawTextEx(font, formatted,
             Vector2{.x = cursor.x + margin_v, .y = cursor.y + margin_h},
             font_size, 0.3f, SKYBLUE);

  cursor.x += with_margin.x;
}
void f3::print(float number, bool same_line) noexcept {
  const char *formatted = TextFormat("%f", number);
  auto measured = MeasureTextEx(font, formatted, font_size, 0.3f);
  auto with_margin =
      Vector2{.x = measured.x + 2 * margin_v, .y = measured.y + 2 * margin_h};

  if (!same_line) {
    cursor.y += with_margin.y;
  }

  DrawRectangleRec(Rectangle{.x = cursor.x,
                             .y = cursor.y,
                             .width = with_margin.x,
                             .height = with_margin.y},
                   background);

  DrawTextEx(font, formatted,
             Vector2{.x = cursor.x + margin_v, .y = cursor.y + margin_h},
             font_size, 0.3f, SKYBLUE);

  cursor.x += with_margin.x;
}
void f3::print(double number, bool same_line) noexcept {
  const char *formatted = TextFormat("%lf", number);
  auto measured = MeasureTextEx(font, formatted, font_size, 0.3f);
  auto with_margin =
      Vector2{.x = measured.x + 2 * margin_v, .y = measured.y + 2 * margin_h};

  if (!same_line) {
    cursor.y += with_margin.y;
  }

  DrawRectangleRec(Rectangle{.x = cursor.x,
                             .y = cursor.y,
                             .width = with_margin.x,
                             .height = with_margin.y},
                   background);

  DrawTextEx(font, formatted,
             Vector2{.x = cursor.x + margin_v, .y = cursor.y + margin_h},
             font_size, 0.3f, SKYBLUE);

  cursor.x += with_margin.x;
}
void f3::print(bool boolean, bool same_line) noexcept {
  const char *formatted = boolean ? "True" : "False";
  auto measured = MeasureTextEx(font, formatted, font_size, 0.3f);
  auto with_margin =
      Vector2{.x = measured.x + 2 * margin_v, .y = measured.y + 2 * margin_h};

  if (!same_line) {
    cursor.y += with_margin.y;
  }

  DrawRectangleRec(Rectangle{.x = cursor.x,
                             .y = cursor.y,
                             .width = with_margin.x,
                             .height = with_margin.y},
                   background);

  DrawTextEx(font, formatted,
             Vector2{.x = cursor.x + margin_v, .y = cursor.y + margin_h},
             font_size, 0.3f, boolean ? LIME : RED);

  cursor.x += with_margin.x;
}
void f3::print(void *ptr, bool same_line) noexcept {
  const char *formatted = TextFormat("%p", ptr);
  auto measured = MeasureTextEx(font, formatted, font_size, 0.3f);
  auto with_margin =
      Vector2{.x = measured.x + 2 * margin_v, .y = measured.y + 2 * margin_h};

  if (!same_line) {
    cursor.y += with_margin.y;
  }

  DrawRectangleRec(Rectangle{.x = cursor.x,
                             .y = cursor.y,
                             .width = with_margin.x,
                             .height = with_margin.y},
                   background);

  DrawTextEx(font, formatted,
             Vector2{.x = cursor.x + margin_v, .y = cursor.y + margin_h},
             font_size, 0.3f, MAGENTA);

  cursor.x += with_margin.x;
}
void f3::enqueue(std::any data, bool same_line) {
  queue.push(f3_queue_data{data, same_line});
}
void f3::print_queue() noexcept {
  while (!queue.empty()) {
    f3_queue_data &element = queue.front();

    if (element.data.type() == typeid(char *)) {
      char *str = std::any_cast<char *>(element.data);
      print(str, element.same_line);
    } else if (element.data.type() == typeid(std::string)) {
      std::string str = std::any_cast<std::string>(element.data);
      print(str, element.same_line);
    } else if (element.data.type() == typeid(int)) {
      int integer = std::any_cast<int>(element.data);
      print(integer, element.same_line);
    } else if (element.data.type() == typeid(size_t)) {
      size_t integer = std::any_cast<size_t>(element.data);
      print(integer, element.same_line);
    } else if (element.data.type() == typeid(float)) {
      float floating = std::any_cast<float>(element.data);
      print(floating, element.same_line);
    } else if (element.data.type() == typeid(double)) {
      double floating = std::any_cast<double>(element.data);
      print(floating, element.same_line);
    } else if (element.data.type() == typeid(bool)) {
      bool boolean = std::any_cast<bool>(element.data);
      print(boolean, element.same_line);
    } else if (element.data.type() == typeid(void *)) {
      void *ptr = std::any_cast<void *>(element.data);
      print(ptr, element.same_line);
    }

    queue.pop();
  }
}

f3::f3(Font font, float font_size, Color text_color, Color background_color)
    : font(font), font_size(font_size), text(text_color),
      background(background_color), margin_h(1), margin_v(1),
      cursor(Vector2{0, 0}) {}
f3::~f3() {}
}; // namespace mr::debug
