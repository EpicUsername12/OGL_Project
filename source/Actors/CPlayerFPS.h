#pragma once

#include "../Engine/CGameCamera.h"
#include "../Engine/CGameScene.h"

#include <numbers>
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <GLFW/glfw3.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/normal.hpp>

enum EInputType {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    JUMP,
    WALK,
    FLY_UP,
    FLY_DOWN,
};

struct SInputData {
    int key;
    bool is_pressed;
};

class CPlayerFPS : public CGameActor {
    GLFWwindow* window = nullptr;
    GLFWvidmode* video_mode = nullptr;
    CGameScene* scene = nullptr;

  public:
    glm::vec3 real_pos = glm::vec3(.0f);

    float sens_x = 1.f;
    float sens_y = 1.f;

    int forward_key = GLFW_KEY_W;
    int backward_key = GLFW_KEY_S;
    int left_key = GLFW_KEY_A;
    int right_key = GLFW_KEY_D;
    int walk_key = GLFW_KEY_LEFT_ALT;
    int jump_key = GLFW_KEY_SPACE;

    bool is_walking = false;
    bool is_jumping = false;

    float move_speed = 16.0f;
    float run_accel = 14.0f;
    float run_deaccel = 10.0f;
    float air_accel = 2.0f;
    float air_deaccel = 2.0f;
    float air_control = 0.3f;
    float side_accel = 50.0f;
    float side_speed = 1.0f;
    float jump_speed = 20.0f;
    float friction_ratio = 8.0f;
    float gravity = 70.0f;

    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 velocity_norm = glm::vec3(0.0f);
    glm::vec3 player_velocity = glm::vec3(0.0f);
    // float m_PlayerTopVelocity = 0.0f;

    glm::vec3 forward_dir = glm::vec3(0.0f);
    glm::vec3 right_dir = glm::vec3(0.0f);

    float forward_amount = 0.0f; //< Forward amount [-1; 1] (-1 being backwards)
    float right_amount = 0.0f;   //< Right amount [-1; 1] (-1 being left)

    bool is_grounded = false;
    bool is_jump_queued = false;

    float lastModeSwap = 0.0f;

  public:
    CPlayerFPS(GLFWwindow* window, GLFWvidmode* vidmode, CGameScene* scene, CGameCamera* camera);

    virtual void onActorInit();
    virtual void onActorUpdate(float timespan);
    virtual void onActorRender() {
    }
    virtual void onActorDestroy() {
    }

    void QueueJump();
    void GroundMove(float timespan);
    void AirMove(float timespan);

    void Accelerate(glm::vec3 dir, float speed, float accel, float timespan);
    void ApplyFriction(float rate, float timespan);
    void AirControl(glm::vec3 dir, float speed, float timespan);

    inline glm::vec3 CalculateWishDirection();

    bool IsKeyPressed(int key) {
        return glfwGetKey(this->window, key) == GLFW_PRESS;
    }

    bool IsMousePressed(int key) {
        return glfwGetMouseButton(this->window, key) == GLFW_PRESS;
    }
};