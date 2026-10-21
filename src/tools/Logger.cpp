#include "Logger.h"

#include <algorithm>

int Logger::mLogLevel = 1;

void Logger::setLogLevel(int logLevel)
{
    static constexpr auto MAX_LEVEL = 9;

    mLogLevel = std::min(MAX_LEVEL, logLevel);
}

std::ostream& operator<<(std::ostream& os, glm::vec3 const& vec)
{
    std::cout << vec[0] << " " << vec[1] << " " << vec[2];
    return os;
}

std::ostream& operator<<(std::ostream& os, glm::vec4 const& vec)
{
    std::cout << vec[0] << " " << vec[1] << " " << vec[2] << " " << vec[3];
    return os;
}

std::ostream& operator<<(std::ostream& os, glm::quat const& quat)
{
    std::cout << quat[0] << " " << quat[1] << " " << quat[2] << " " << quat[3];
    return os;
}

std::ostream& operator<<(std::ostream& os, glm::mat4 const& mat)
{
    std::cout << mat[0] << std::endl;
    std::cout << mat[1] << std::endl;
    std::cout << mat[2] << std::endl;
    std::cout << mat[3] << std::endl;
    return os;
}