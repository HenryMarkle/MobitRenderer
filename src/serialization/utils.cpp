#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <exception>

#include <raylib.h>

#include <MobitParser/nodes.h>
#include <MobitParser/tokens.h>

#include <MobitRenderer/exceptions.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/serialization.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/dex.h>

namespace mr::serde {


bool deser_bool(const mp::Node *node) {
  const auto *int_node = dynamic_cast<const mp::Int*>(node);

  if (int_node != nullptr) {
    return int_node->number != 0;
  }

  const auto *float_node = dynamic_cast<const mp::Float*>(node);

  if (float_node == nullptr)
    throw deserialization_failure("node is not Int or Float");

  return float_node->number != 0;
}
int deser_int(const mp::Node *node) {
  const mp::Int *int_node = dynamic_cast<const mp::Int*>(node);
  if (int_node == nullptr) {
    
    const mp::Float *float_node = dynamic_cast<const mp::Float*>(node);
    if (float_node == nullptr) throw deserialization_failure("node is not an Int or a Float");

    return (int)float_node->number;
  }
  return int_node->number;
}
float deser_float(const mp::Node *node) {
  const mp::Float *float_node = dynamic_cast<const mp::Float*>(node);
  if (float_node == nullptr) {
    const mp::Int *int_node = dynamic_cast<const mp::Int*>(node);

    if (int_node == nullptr) throw deserialization_failure("node is not a Float or an Int");

    return (float)int_node->number;
  }
  return float_node->number;
}
int8_t deser_int8(const mp::Node *node) {
  const mp::Int *int_node = dynamic_cast<const mp::Int*>(node);
  if (int_node == nullptr) throw deserialization_failure("node is not an uint8");
  return (int8_t)int_node->number;
}
uint8_t deser_uint8(const mp::Node *node) {
  const mp::Int *int_node = dynamic_cast<const mp::Int*>(node);
  if (int_node == nullptr) throw deserialization_failure("node is not an uint8");
  return (uint8_t)int_node->number;
}
uint16_t deser_uint16(const mp::Node *node) {
  const mp::Int *int_node = dynamic_cast<const mp::Int*>(node);
  if (int_node == nullptr) throw deserialization_failure("node is not an uint16");
  return (uint16_t)int_node->number;
}
std::string deser_string(const mp::Node *node) {
  const mp::String *str_node = dynamic_cast<const mp::String*>(node);
  if (str_node == nullptr) throw deserialization_failure("node is not a String");
  return str_node->str;
}
Color deser_color(const mp::Node *node) {
  const mp::GCall *color_gcall_node = dynamic_cast<const mp::GCall*>(node);
  if (color_gcall_node == nullptr)
    throw deserialization_failure("color is not a global call");
  if (color_gcall_node->name != "color")
    throw deserialization_failure("color is not a global call named 'color'");
  if (color_gcall_node->args.size() < 3)
    throw deserialization_failure("color has insufficient arguments (expected at least 3)");

  uint8_t r, g, b;

  try {
    r = deser_uint8(color_gcall_node->args[0].get());
  } catch (deserialization_failure &e) {
    std::string msg("invalid color argument 1: ");
    msg.append(e.what());

    throw deserialization_failure(msg);
  }

  try {
    g = deser_uint8(color_gcall_node->args[1].get());
  } catch (deserialization_failure &e) {
    std::string msg("invalid color argument 2: ");
    msg.append(e.what());
  
    throw deserialization_failure(msg);
  }

  try {
    b = deser_uint8(color_gcall_node->args[2].get());
  } catch (deserialization_failure &e) {
    std::string msg("invalid color argument 2: ");
    msg.append(e.what());
    
    throw deserialization_failure(msg);
  }

  return Color{r, g, b, 255};
}
std::vector<std::string> deser_string_vec(const mp::Node *node) {
  const mp::List *list = dynamic_cast<const mp::List*>(node);
  if (list == nullptr) throw deserialization_failure("node is not a linear list");

  std::vector<std::string> strings;
  strings.reserve(list->elements.size());

  try {
    for (auto &element : list->elements) {
      mp::Node *element_node = element.get();
      mp::String *element_string = dynamic_cast<mp::String*>(element_node);
      if (element_string == nullptr) throw deserialization_failure("failed to deserialize list element: node is not a string");
      strings.push_back(element_string->str);
    }
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize list element: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  return strings;
}
std::unordered_set<std::string> deser_string_set(const mp::Node *node) {
  const mp::List *list = dynamic_cast<const mp::List*>(node);
  if (list == nullptr) throw deserialization_failure("node is not a linear list");

  std::unordered_set<std::string> strings;
  strings.reserve(list->elements.size());

  try {
    for (auto &element : list->elements) {
      mp::Node *element_node = element.get();
      mp::String *element_string = dynamic_cast<mp::String*>(element_node);
      if (element_string == nullptr) throw deserialization_failure("failed to deserialize list element: node is not a string");
      strings.insert(element_string->str);
    }
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize list element: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  return strings;
}
std::vector<int8_t> deser_int8_vec (const mp::Node *node) {
  const mp::List *list = dynamic_cast<const mp::List*>(node);
  if (list == nullptr) throw deserialization_failure("node is not a linear list");

  std::vector<int8_t> numbers;
  numbers.reserve(list->elements.size());

  try {
    for (auto &element : list->elements) {
      int8_t number = deser_int8(element.get());
      numbers.push_back(number);
    }
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize list element: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  return numbers;
}
std::vector<uint8_t> deser_uint8_vec (const mp::Node *node) {
  const mp::List *list = dynamic_cast<const mp::List*>(node);
  if (list == nullptr) throw deserialization_failure("node is not a linear list");

  std::vector<uint8_t> numbers;
  numbers.reserve(list->elements.size());

  try {
    for (auto &element : list->elements) {
      uint8_t number = deser_uint8(element.get());
      numbers.push_back(number);
    }
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize list element: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  return numbers;
}
void deser_point(const mp::Node *node, int &x, int &y) {
  const mp::GCall *gcall_node = dynamic_cast<const mp::GCall*>(node);
  
  if (gcall_node == nullptr) throw deserialization_failure("node is not a Global Call");
  if (gcall_node->name != "point") throw deserialization_failure("global call is not a point");
  if (gcall_node->args.size() < 2) throw deserialization_failure("point global call has insufficient arguments (expected at least 2)");

  int value_x, value_y;

  try {
    value_x = deser_int(gcall_node->args[0].get());
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize point argument 1: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  try {
    value_y = deser_int(gcall_node->args[1].get());
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize point argument 2: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  x = value_x;
  y = value_y;
}
void deser_point(const mp::Node *node, float &x, float &y) {
  const mp::GCall *gcall_node = dynamic_cast<const mp::GCall*>(node);
  
  if (gcall_node == nullptr) throw deserialization_failure("node is not a Global Call");
  if (gcall_node->name != "point") throw deserialization_failure("global call is not a point");
  if (gcall_node->args.size() < 2) throw deserialization_failure("point global call has insufficient arguments (expected at least 2)");

  float value_x, value_y;

  try {
    value_x = deser_float(gcall_node->args[0].get());
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize point argument 1: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  try {
    value_y = deser_float(gcall_node->args[1].get());
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize point argument 2: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  x = value_x;
  y = value_y;
}

void deser_point(const mp::Node *node, Vector2 &vector) {
  const mp::GCall *gcall_node = dynamic_cast<const mp::GCall*>(node);
  
  if (gcall_node == nullptr) throw deserialization_failure("node is not a Global Call");
  if (gcall_node->name != "point") throw deserialization_failure("global call is not a point");
  if (gcall_node->args.size() < 2) throw deserialization_failure("point global call has insufficient arguments (expected at least 2)");

  float value_x, value_y;

  try {
    value_x = deser_float(gcall_node->args[0].get());
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize point argument 1: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  try {
    value_y = deser_float(gcall_node->args[1].get());
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize point argument 2: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  vector.x = value_x;
  vector.y = value_y;
}

};