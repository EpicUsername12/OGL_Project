#include "CPlayerFPS.h"

CPlayerFPS::CPlayerFPS(GLFWwindow* window, GLFWvidmode* vidmode, CGameScene* scene, CGameCamera* camera) {
    this->window = window;
    this->video_mode = vidmode;

    this->scene = scene;
    this->camera = camera;
    this->type = EActorType::PLAYER_FPS;
}

void CPlayerFPS::onActorInit() {
    this->camera->pos = this->pos;
    this->camera->pos.y += 3.0f;
    glfwSetCursorPos(this->window, this->video_mode->width / 2.0, this->video_mode->height / 2.0);
}

void CPlayerFPS::onActorUpdate(float timespan) {

    /* Don't fetch inputs if we don't have the input focus */
    if (!glfwGetWindowAttrib(this->window, GLFW_FOCUSED)) {
        return;
    }

    /* Check if player wants to go in user interface selection mode, with a 0.10s cooldown between swaps */
    if (glfwGetTime() - this->lastModeSwap > 0.075f) {
        if (this->IsKeyPressed(GLFW_KEY_ESCAPE)) {
            this->scene->isUserInterfaceMode = !this->scene->isUserInterfaceMode;
            this->lastModeSwap = (float)glfwGetTime();
        }
    }

    /* If we're not in user selection mode, then read mouse inputs */
    if (!this->scene->isUserInterfaceMode) {

        double xpos, ypos;
        glfwGetCursorPos(this->window, &xpos, &ypos);
        glfwSetCursorPos(this->window, this->video_mode->width / 2.0, this->video_mode->height / 2.0);

        float angleX = (float)((this->video_mode->width / 2.0) - xpos);
        float angleY = (float)((this->video_mode->height / 2.0) - ypos);

        this->camera->view_angles.x += angleX * 0.001f;
        this->camera->view_angles.y += angleY * 0.001f;
        this->camera->ComputeNewDirection();

        this->forward_dir = this->camera->dir;
        this->forward_dir.y = 0;
        this->forward_dir = glm::normalize(this->forward_dir);

        this->right_dir.x = (float)sin(this->camera->view_angles.x - std::numbers::pi / 2);
        this->right_dir.y = 0.0f;
        this->right_dir.z = (float)cos(this->camera->view_angles.x - std::numbers::pi / 2);
        this->right_dir = glm::normalize(this->right_dir);

        if (this->IsKeyPressed(this->forward_key)) {
            this->forward_amount = 1.0f;
        }
        if (this->IsKeyPressed(this->backward_key)) {
            this->forward_amount = -1.0f;
        }
        if (this->IsKeyPressed(this->right_key)) {
            this->right_amount = 1.0f;
        }
        if (this->IsKeyPressed(this->left_key)) {
            this->right_amount = -1.0f;
        }

        if (!this->IsKeyPressed(this->forward_key) && !this->IsKeyPressed(this->backward_key)) {
            this->forward_amount = 0.0f;
        }

        if (!this->IsKeyPressed(this->right_key) && !this->IsKeyPressed(this->left_key)) {
            this->right_amount = 0.0f;
        }
    } else {
        this->right_amount = 0.0f;
        this->forward_amount = 0.0f;
    }

    this->QueueJump();
    if (this->is_grounded) {
        this->GroundMove(timespan);
    } else {
        this->AirMove(timespan);
    }

    glm::vec3 frameMovement = this->player_velocity * timespan;
    if (this->IsKeyPressed(this->walk_key)) {
        frameMovement *= 0.4f;
    }

    if (glm::length(frameMovement) != 0.0f) {
        const physx::PxU32 num_ents = 32;
        physx::PxSweepHit buffer[num_ents];
        physx::PxSweepBuffer sweepBuffer(buffer, num_ents);

        /* Vertical capsule, and unit direction */
        physx::PxCapsuleGeometry playerShape(0.7f, 1.15f);                                    //< Radius, half-height
        glm::mat4 transf = glm::translate(glm::mat4(1.0f), this->pos + glm::vec3(0, 1.5, 0)); //< Origin
        transf = glm::rotate(transf, physx::PxHalfPi, glm::vec3(0, 0, 1));                    //< Vert capsule
        glm::vec3 unitDir = glm::normalize(frameMovement);
        float dist = glm::length(frameMovement);

        /* Sweep a capsule on the physical scene that represent the invisible player capsule */
        bool res = this->scene->physScene->sweep(playerShape, transf_to_physx(transf), vec3_to_physx(unitDir), dist,
                                                 sweepBuffer, physx::PxHitFlag::eDEFAULT | physx::PxHitFlag::eMTD);

        if (res) {
            /* For each hit, apply displacement */
            for (physx::PxU32 i = 0; i < sweepBuffer.nbTouches; i++) {
                physx::PxSweepHit* hit = &sweepBuffer.touches[i];
                frameMovement -= vec3_from_physx(hit->normal * hit->distance);
                /* If hit normal y > 0, it means we're hitting ground while going down = grounded */
                if (hit->normal.y > 0) {
                    this->is_grounded = true;
                }
            }
        } else {
            this->is_grounded = false;
        }
    } else {
        this->is_grounded = true;
    }

    this->camera->GetPosition() += frameMovement;
    this->pos = this->camera->GetPosition();
    this->pos.y -= 3.0f;

    return;
}

void CPlayerFPS::QueueJump() {
    if (this->IsKeyPressed(this->jump_key) && !this->is_jump_queued) {
        this->is_jump_queued = true;
    }

    if (!this->IsKeyPressed(this->jump_key)) {
        this->is_jump_queued = false;
    }
}

void CPlayerFPS::GroundMove(float timespan) {
    glm::vec3 wishdir;

    // Do not apply friction if the player is queueing up the next jump
    if (!this->is_jump_queued) {
        this->ApplyFriction(1.0f, timespan);
    } else {
        this->ApplyFriction(0.0f, timespan);
    }

    wishdir = this->CalculateWishDirection();
    this->velocity_norm = wishdir;

    float wishspeed = glm::length(wishdir);
    wishspeed *= this->move_speed;

    // Accelerate player in the inputed direction
    this->Accelerate(wishdir, wishspeed, this->run_accel, timespan);
    this->player_velocity.y = 0; // Apply gravity only when the player is in the air

    // If player should jump, increase vertical velocity
    if (this->is_jump_queued) {
        this->player_velocity.y = this->jump_speed;
        this->is_jump_queued = false;
    }
}

void CPlayerFPS::Accelerate(glm::vec3 dir, float speed, float accel, float timespan) {
    float addspeed;
    float accelspeed;
    float currentspeed;

    currentspeed = glm::dot(this->player_velocity, dir);
    addspeed = speed - currentspeed;
    if (addspeed <= 0)
        return;

    accelspeed = accel * timespan * speed;
    if (accelspeed > addspeed)
        accelspeed = addspeed;

    this->player_velocity.x += accelspeed * dir.x;
    this->player_velocity.z += accelspeed * dir.z;
}

void CPlayerFPS::ApplyFriction(float rate, float timespan) {
    glm::vec3 vec = this->player_velocity;
    float speed, newspeed, control, drop;

    vec.y = 0.0;
    speed = glm::length(vec);
    drop = 0.0;

    /* Only if the player is on the ground then apply friction */
    if (this->is_grounded) {
        control = speed < this->run_deaccel ? this->run_deaccel : speed;
        drop = control * this->friction_ratio * timespan * rate;
    }

    newspeed = speed - drop;
    if (newspeed < 0)
        newspeed = 0;
    if (speed > 0)
        newspeed /= speed;

    this->player_velocity.x *= newspeed;
    this->player_velocity.z *= newspeed;
}

void CPlayerFPS::AirMove(float timespan) {

    glm::vec3 wishdir;
    float accel;

    wishdir = this->CalculateWishDirection();
    float wishspeed = glm::length(wishdir);
    wishspeed *= this->move_speed;
    this->velocity_norm = wishdir;

    float wishspeed2 = wishspeed;
    if (glm::dot(this->player_velocity, wishdir) < 0)
        accel = this->air_deaccel;
    else
        accel = this->air_accel;
    // If the player is ONLY strafing left or right
    if (this->forward_amount == 0.0f && this->right_amount != 0.0f) {
        if (wishspeed > this->side_speed)
            wishspeed = this->side_speed;
        accel = this->side_accel;
    }

    this->Accelerate(wishdir, wishspeed, accel, timespan);
    if (this->air_control)
        this->AirControl(wishdir, wishspeed2, timespan);

    // Apply gravity
    this->player_velocity.y -= this->gravity * timespan;
}

void CPlayerFPS::AirControl(glm::vec3 dir, float wishspeed, float timespan) {
    float yspeed, speed, dot, k;

    // Can't control movement if not moving forward or backward
    if (this->forward_amount == 0.0f || wishspeed == 0.0f)
        return;

    yspeed = this->player_velocity.y;
    this->player_velocity.y = 0;

    speed = glm::length(this->player_velocity);
    this->player_velocity = glm::normalize(this->player_velocity);

    dot = glm::dot(this->player_velocity, dir);
    k = 32.0f;
    k *= this->air_control * dot * dot * timespan;

    // cap movement speed in air, it can get really fast
    if (speed > (1.4f * this->move_speed)) {
        speed = 1.4f * this->move_speed;
    }

    // Change direction while slowing down
    if (dot > 0) {
        this->player_velocity.x = this->player_velocity.x * speed + dir.x * k;
        this->player_velocity.y = this->player_velocity.y * speed + dir.y * k;
        this->player_velocity.z = this->player_velocity.z * speed + dir.z * k;

        this->player_velocity = glm::normalize(this->player_velocity);
        this->velocity_norm = this->player_velocity;
    }

    this->player_velocity.x *= speed;
    this->player_velocity.y = yspeed;
    this->player_velocity.z *= speed;
}

glm::vec3 CPlayerFPS::CalculateWishDirection() {
    glm::vec3 dir(0.0f);
    dir += this->forward_dir * this->forward_amount;
    dir += this->right_dir * this->right_amount;
    return dir;
}