add_executable (HW2
    src/main.cpp
)

# TODO: Add tests and install targets if needed.
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

target_link_libraries(HW2 PRIVATE 
    glfw
    glad::glad
    glm::glm
    imgui::imgui
)

