#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef WIN32
#include <windows.h>
#endif
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <algorithm>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define FILE_LOG_MODE 0
#if FILE_LOG_MODE
#include <spdlog/sinks/basic_file_sink.h>
#endif

#define CLASS_PTR(klassName) \
class klassName; \
using klassName ## UPtr = std::unique_ptr<klassName>; \
using klassName ## Ptr = std::shared_ptr<klassName>; \
using klassName ## WPtr = std::weak_ptr<klassName>;

std::optional<std::string> LoadTextFile(const std::string& filename);
glm::vec3 GetAttenuationCoeff(float distance);
std::string getRelativePath(const char* path);
const float getRandomFloat(const int min, const int max);

enum class View
{
    VIEW_MODEL,
    VIEW_BROAD,
    VIEW_NARROW,
    VIEW_PENETRATION,
};

#endif // __COMMON_H__