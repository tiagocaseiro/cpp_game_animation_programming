#pragma once

#define SHARED_ONLY(ClassName)                                                                                         \
    using ClassName##Shared = std::shared_ptr<class ClassName>;                                                        \
    using ClassName##Weak   = std::weak_ptr<class ClassName>;