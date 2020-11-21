#pragma once

#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>
#include <filesystem>
#include <regex>
#include <fstream>
#include <functional>
#include <array>
#include <utility>
#include <map>
#include <set>
#include <unordered_map>
#include <string>
#include <iterator>
#include <algorithm>
#include <chrono>
#include "injector/hooking.hpp"
#include "injector/calling.hpp"
#include "injector/utility.hpp"
#include "utf8cpp/utf8.h"
#include "BinaryFile.hpp"

#define FMT_HEADER_ONLY
#include "fmt/printf.h"

struct CharacterPos
{
    std::uint32_t row, column;
};

struct CharacterData
{
    std::uint32_t code;
    CharacterPos pos;
};

#define VALIDATE_SIZE(expr, size) static_assert(sizeof(expr) == size, "Wrong size.");
