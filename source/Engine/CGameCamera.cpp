#include "CGameCamera.h"

CGameCamera::CGameCamera(float fov, float ratio, float nearDist, float farDist) {
    this->fovy = fov;
    this->near_dist = nearDist;
    this->far_dist = farDist;
    this->aspect_ratio = ratio;
}

void CGameCamera::ComputeNewDirection() {

    // Clamp horizontally
    if (this->view_angles.x >= std::numbers::pi) {
        this->view_angles.x -= glm::radians(360.0f);
    } else if (this->view_angles.x <= -std::numbers::pi) {
        this->view_angles.x += glm::radians(360.0f);
    }

    // Clamp vertically
    if (this->view_angles.y >= std::numbers::pi / 2) {
        this->view_angles.y = glm::radians(89.99f);
    } else if (this->view_angles.y <= -std::numbers::pi / 2) {
        this->view_angles.y = glm::radians(-89.99f);
    }

    this->dir.x = cos(this->view_angles.y) * sin(this->view_angles.x);
    this->dir.y = sin(this->view_angles.y);
    this->dir.z = cos(this->view_angles.y) * cos(this->view_angles.x);
}