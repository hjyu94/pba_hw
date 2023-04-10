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

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;

    int m_width{ 1200 };
    int m_height{ 900 };

    // clear color
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

    // light parameter
    glm::vec3 m_lightPos{ -5.f, -5.f, 5.f };
    glm::vec3 m_lightColor{ 0.f, 0.f, 0.f };

    // camera parameter
    bool m_cameraControl { false };
    const float m_cameraRotSpeed = 0.3f;
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch { 0.0f };
    float m_cameraYaw { 0.0f };
    glm::vec3 m_cameraFront { glm::vec3(0.0f, -1.0f, 0.0f) };
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 5.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, -1.0f, 0.0f) };

    ModelUPtr m_wire;
    ModelUPtr m_bead;

    // environment
    float m_gravity = 9.8f;
    /*
    float m_ks = 15.f;
    float m_kd = 1.f;
    
    float m_mass = 5.f;
    float m_current_velocity = 0.f;

    float m_elapsed_time = 0.f;
    float m_timestep = 0.01f;
    
    const char* m_current_method{"Explicit Euler"};
    
    float m_start_length = 5.f;
    float m_rest_length = 5.f;*/
};

#endif // __CONTEXT_H__