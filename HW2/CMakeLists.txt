add_executable(HW2
    src/main.cpp
    src/common.cpp src/common.h
    src/shader.cpp src/shader.h
    src/program.cpp src/program.h
    src/context.cpp src/context.h
    src/object.cpp src/object.h
)

# TODO: Add tests and install targets if needed.
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(HW2 PRIVATE 
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    spdlog::spdlog spdlog::spdlog_header_only
)

target_compile_definitions(HW2 PUBLIC
    CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
