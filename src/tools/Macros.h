#pragma once

#include <memory>
#include <optional>

#define SHARED_ONLY(ClassName)                                                                                         \
    using ClassName##Shared = std::shared_ptr<class ClassName>;                                                        \
    using ClassName##Weak   = std::weak_ptr<class ClassName>;

#define OPTIONAL_ONLY(ClassName) using ClassName##Optional = std::optional<class ClassName>;\
