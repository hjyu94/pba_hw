add_executable (HW2
    src/main.cpp
    src/buffer.cpp src/buffer.h
    src/common.cpp src/common.h
    src/context.cpp src/context.h
    src/program.cpp src/program.h
    src/shader.cpp src/shader.h
    src/vertex_layout.cpp src/vertex_layout.h
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

