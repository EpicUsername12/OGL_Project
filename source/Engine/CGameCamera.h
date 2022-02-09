#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <numbers>

class CGameCamera {

  protected:
    float fovy = 70.0f;
    float near_dist = 0.01f;
    float far_dist = 1000.0f;
    float aspect_ratio = 16.0f / 9.0f;
    glm::vec3 up = glm::vec3(.0f, 1.0f, .0f);

  public:
    glm::vec3 pos = glm::vec3(.0f);
    glm::vec3 dir = glm::vec3(.0f);
    glm::vec2 view_angles = glm::vec2(.0f);

    void SetPosition(glm::vec3& pos) {
        pos = pos;
    }
    void SetPosition(float x, float y, float z) {
        pos = glm::vec3(x, y, z);
    }
    glm::vec3& GetPosition() {
        return pos;
    }

    void SetDirection(glm::vec3& pos) {
        dir = pos;
    }
    void SetDirection(float x, float y, float z) {
        dir = glm::vec3(x, y, z);
    }
    glm::vec3& GetDirection() {
        return dir;
    }

    void SetViewAngles(glm::vec2& angles) {
        view_angles = angles;
    }
    void SetViewAngles(float x, float y) {
        view_angles = glm::vec2(x, y);
    }
    glm::vec2& GetViewAngles() {
        return view_angles;
    }

    void SetFOV(float fov) {
        fovy = fov;
    }
    float& GetFOV() {
        return fovy;
    }

    void SetNear(float x) {
        near_dist = x;
    }
    float& GetNear() {
        return near_dist;
    }

    void SetFar(float x) {
        far_dist = x;
    }
    float& GetFar() {
        return far_dist;
    }

    void SetRatio(float ratio) {
        aspect_ratio = ratio;
    }
    float& GetRatio() {
        return aspect_ratio;
    }

  public:
    virtual void onCameraUpdate(float timespan) {
        this->ComputeNewDirection();
    }

    CGameCamera() {
    }
    CGameCamera(float fov, float ratio, float nearDist = 0.1f, float farDist = 1000.0f);

    void ComputeNewDirection();

    inline void ViewMatrix(glm::mat4& outputMatrix) {
        outputMatrix = glm::lookAt(this->pos, this->pos + this->dir, this->up);
    }

    inline glm::mat4 ViewMatrix() {
        return glm::lookAt(this->pos, this->pos + this->dir, this->up);
    }
};