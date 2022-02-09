#include "DebugDraw.h"

void DrawLine(COpenGLShader* shader, CGameCamera* camera, glm::vec3 posStart, glm::vec3 posEnd) {

    std::vector<glm::vec3> vertices;
    vertices.push_back(posStart);
    vertices.push_back(posEnd);

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 2, vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 View = camera->ViewMatrix();
    glm::mat4 Proj =
        glm::perspective(glm::radians(camera->GetFOV()), camera->GetRatio(), camera->GetNear(), camera->GetFar());

    glm::vec4 vertexColor = glm::vec4(0.8f, 0.0f, 0.0f, 0.8f);

    glUseProgram(shader->GetProgramID());
    shader->SetUniform("vertexColor", vertexColor);
    shader->SetUniform("Model", Model);
    shader->SetUniform("View", View);
    shader->SetUniform("Projection", Proj);
    shader->SetUniform("viewPosition", camera->GetPosition());

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.5f);
    glDrawArrays(GL_LINES, 0, 2);
    glLineWidth(1.0f);
    glDisable(GL_LINE_SMOOTH);

    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}