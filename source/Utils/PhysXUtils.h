#pragma once

#include "DebugUtils.h"

#include <PxPhysicsAPI.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern physx::PxFoundation* gFoundation;
extern physx::PxPhysics* gPhysics;
extern physx::PxPvd* gPvd;
extern physx::PxCooking* gCooking;
extern physx::PxCpuDispatcher* gDispatcher;

inline physx::PxMat44 mat4_to_physx(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {

    physx::PxMat44 retMat;
    glm::mat4 mat(1.0f);
    mat = glm::translate(mat, pos);
    mat = glm::rotate(mat, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
    mat = glm::rotate(mat, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
    mat = glm::rotate(mat, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
    mat = glm::scale(mat, scale);

    retMat[0] = physx::PxVec4(mat[0].x, mat[0].y, mat[0].z, mat[0].w);
    retMat[1] = physx::PxVec4(mat[1].x, mat[1].y, mat[1].z, mat[1].w);
    retMat[2] = physx::PxVec4(mat[2].x, mat[2].y, mat[2].z, mat[2].w);
    retMat[3] = physx::PxVec4(mat[3].x, mat[3].y, mat[3].z, mat[3].w);
    return retMat;
}

inline physx::PxMat44 mat4_to_physx(glm::mat4 mat) {
    physx::PxMat44 retMat;
    retMat[0] = physx::PxVec4(mat[0].x, mat[0].y, mat[0].z, mat[0].w);
    retMat[1] = physx::PxVec4(mat[1].x, mat[1].y, mat[1].z, mat[1].w);
    retMat[2] = physx::PxVec4(mat[2].x, mat[2].y, mat[2].z, mat[2].w);
    retMat[3] = physx::PxVec4(mat[3].x, mat[3].y, mat[3].z, mat[3].w);
    return retMat;
}

inline physx::PxTransform transf_to_physx(glm::mat4 mat) {
    return physx::PxTransform(mat4_to_physx(mat));
}

inline physx::PxVec3 vec3_to_physx(glm::vec3 vec) {
    return physx::PxVec3(vec.x, vec.y, vec.z);
}

inline glm::vec3 vec3_from_physx(physx::PxVec3 vec) {
    return glm::vec3(vec.x, vec.y, vec.z);
}