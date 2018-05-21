#pragma once

#include <cstdint>
#include <memory>

using int64 = std::int64_t;

template <typename T>
using Handle = std::shared_ptr<T>;
