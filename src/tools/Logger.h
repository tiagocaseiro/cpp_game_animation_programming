#pragma once

#include <cstdio>
#include <iostream>
#include <utility>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#define LOG_VAR(var) std::cout << #var << ": " << var << std::endl;

class Logger
{
public:
    template <typename... Args>
    static void log(const int logLevel, Args&&... args)
    {
        if(logLevel <= mLogLevel)
        {
            std::printf(std::forward<Args>(args)...);
            std::fflush(stdout);
        }
    }

    static void setLogLevel(int logLevel);

private:
    static int mLogLevel;
};

std::ostream& operator<<(std::ostream& os, glm::vec3 const& vec);
std::ostream& operator<<(std::ostream& os, glm::vec4 const& vec);
std::ostream& operator<<(std::ostream& os, glm::quat const& quat);
std::ostream& operator<<(std::ostream& os, glm::mat4 const& mat);
