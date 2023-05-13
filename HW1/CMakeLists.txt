set(PROJECT_NAME HW1)

add_executable (${PROJECT_NAME}
    src/main.cpp
 )

# TODO: Add tests and install targets if needed.
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(imgui CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    spdlog::spdlog spdlog::spdlog_header_only
    assimp::assimp
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    spdlog::spdlog spdlog::spdlog_header_only
    assimp::assimp
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)