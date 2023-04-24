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

using COLLIDING_PAIR = std::pair<SpherePtr, SpherePtr>;

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
    void InitializeEnvParameter();
    void ComputeCollision();
    void SweepAndPrune();
    void FindCollidingSpheres(std::vector<CompareHelper> helpers, std::vector<COLLIDING_PAIR>& output);
    void Intersection(std::vector<COLLIDING_PAIR> v1, std::vector<COLLIDING_PAIR> v2, std::vector<COLLIDING_PAIR>& output);

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;

    int m_width{ 1200 };
    int m_height{ 900 };

    // clear color
    glm::vec4 m_clearColor { glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) };

    // light parameter
    glm::vec3 m_lightPos{ -5.f, 5.f, 5.f };
    glm::vec3 m_lightColor{ 0.f, 0.f, 0.f };

    // camera parameter
    bool m_cameraControl { false };
    const float m_cameraRotSpeed = 0.3f;
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch { 0.0f };
    float m_cameraYaw { 0.0f };
    glm::vec3 m_cameraFront { glm::vec3(0.0f, -1.0f, 0.0f) };
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 5.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };

    const float m_mouse_force_scale = 0.1f;
    glm::vec3 m_mouse_force{ glm::vec3(0.f, 0.f, 0.f) };


    // sphere
    std::vector<SpherePtr> m_spheres;
    std::vector<COLLIDING_PAIR> m_colliding_spheres;

    // UI
    int m_view_type = static_cast<int>(View::VIEW_MODEL);
};

#endif // __CONTEXT_H__