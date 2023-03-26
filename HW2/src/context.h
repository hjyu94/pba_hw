#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "ceiling.h"
#include "mass.h"

CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;

    int m_width {640};
    int m_height {480};

    // ceiliing
    unsigned int m_ceiling_vao;
    unsigned int m_ceiling_vbo;
    unsigned int m_ceiling_ebo;

    CeilingUPtr m_ceiling;
    MassUPtr m_mass;

    // spring
    float restLength = 3.f;
    glm::vec3 m_spring_com{ glm::vec3(0.0f, -1.0f, 0.0f) };
    unsigned int m_spring_vao;
    unsigned int m_spring_vbo;
    unsigned int m_spring_ebo;


    // clear color
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

    // camera parameter
    bool m_cameraControl { false };
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch { 0.0f };
    float m_cameraYaw { 0.0f };
    glm::vec3 m_cameraFront { glm::vec3(0.0f, -1.0f, 0.0f) };
    glm::vec3 m_cameraPos { glm::vec3(0.0f, -1.0f, 5.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
};

#endif // __CONTEXT_H__