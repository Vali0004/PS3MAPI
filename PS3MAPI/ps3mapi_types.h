#pragma once
#include <cstdint>
#define CUSTOM_T(size) using u##size = uint##size##_t; using s##size = int##size##_t;

CUSTOM_T(8);
CUSTOM_T(16);
CUSTOM_T(32);
CUSTOM_T(64);
using f32 = float;
using f64 = double;