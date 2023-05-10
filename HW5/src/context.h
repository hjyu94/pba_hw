#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "sphere.h"
#include "line.h"
#include "plane.h"

class CompareHelper {
public:
    CompareHelper(SpherePtr spherePtr, float value, bool is_start)
        : m_spherePtr(spherePtr), m_value(value), m_is_start(is_start)
    {}

    float m_value;
    SpherePtr m_spherePtr;
    bool m_is_start;
    
    static bool compare(const CompareHelper& a, const CompareHelper& b)
    {
        return a.m_value < b.m_value;
    }

    friend bool operator == (const CompareHelper& a, const CompareHelper& b)
    {
        return a.m_spherePtr == b.m_spherePtr;
    }
};

using COLLIDING_SPHERE_PAIR = std::pair<SpherePtr, SpherePtr>;

CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create();
    void Render(GLFWwindow* window);
    void Update();

    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);
    void ResetCamera();

    void InitializeEnvParameter();
    void ComputeCollision();
    void SweepAndPrune();
    void FindActuallyOverlappedSpheres();
    void FindCollidingSpheres(std::vector<CompareHelper> helpers, std::vector<COLLIDING_SPHERE_PAIR>& output);
    std::vector<COLLIDING_SPHERE_PAIR> Intersection(const std::vector<COLLIDING_SPHERE_PAIR> v1, const std::vector<COLLIDING_SPHERE_PAIR> v2);

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_line_program;

    int m_width{ 1200 };
    int m_height{ 900 };

    // clear color
    glm::vec4 m_clearColor{ glm::vec4(0.8f, 0.8f, 0.8f, 1.f) };

    // light parameter
    glm::vec3 m_lightPos{ 0.f, 10.f, 0.f};
    glm::vec3 m_lightColor{ 0.f, 0.f, 0.f };

    // camera parameter
    bool m_cameraControl { false };
    const float m_cameraRotSpeed = 0.3f;
    const float m_cameraMovSpeed = 0.2f;

    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch { 0.0f };
    float m_cameraYaw { 0.0f };

    glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 3.0f, 20.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };

    const float m_mouse_force_scale = 0.1f;
    glm::vec3 m_mouse_force{ glm::vec3(0.f, 0.f, 0.f) };

    // collision detection
    std::vector<SpherePtr> m_spheres;
    std::vector<LinePtr> m_lines;
    PlanePtr m_plane;
    
    std::vector<COLLIDING_SPHERE_PAIR> m_broad_colliding_spheres;
    std::vector<COLLIDING_SPHERE_PAIR> m_narrow_colliding_spheres;

    // UI
    int m_view_type = static_cast<int>(View::VIEW_MODEL);

    // collision
    float m_timestep = 0.01f;
    float m_gravity = -9.8f;
    float m_colliding_threshold = 0.05f;

    float is_paused = false;
    float epsilon = 0.01f;
};

#endif // __CONTEXT_H__