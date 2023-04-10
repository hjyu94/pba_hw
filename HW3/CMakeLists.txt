add_executable(HW3
    src/main.cpp
    src/common.cpp src/common.h
    src/shader.cpp src/shader.h
    src/program.cpp src/program.h
    src/context.cpp src/context.h
    src/buffer.cpp src/buffer.h
    src/vertex_layout.cpp src/vertex_layout.h
    src/image.cpp src/image.h
    src/texture.cpp src/texture.h
    src/mesh.cpp src/mesh.h
    src/model.cpp src/model.h
)

# TODO: Add tests and install targets if needed.
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

target_link_libraries(HW3 PRIVATE 
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    spdlog::spdlog spdlog::spdlog_header_only
    assimp::assimp
)

target_compile_definitions(HW3 PUBLIC
    CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

