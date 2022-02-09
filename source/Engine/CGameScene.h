#pragma once

#include "../Utils/PhysXUtils.h"

#include "../Resource/CGameModel.h"
#include "../Engine/CGameCamera.h"

#include <vector>
#include <chrono>
#include <functional>
#include <algorithm>

typedef class CGameScene CGameScene;

typedef enum ESceneRenderType {
    RENDER_SHADOW, //< Set render mode to only vertices (for depth calculation)
    RENDER_NORMAL, //< Normal render, with textures etc.. from a standard camera to the screen
} ESceneRenderType;

class CGameActor {

  public:
    typedef enum class EActorType : uint16_t {
        ACTOR_INVALID,

        SIMPLE_MODEL, //< Simple model mesh, can possibly have physic collisions
        PLAYER_FPS,   //< FPS local player
        TEST_BOB_OMB, //< Test Bob Omb actor

        ACTOR_MAX,
    } EActorType;

  protected:
    CGameModel* model = nullptr;
    CGameScene* scene = nullptr;
    CGameCamera* camera = nullptr; //< For the rendering of each actor

    EActorType type = EActorType::ACTOR_INVALID;

  public:
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 rot = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    void SetPosition(glm::vec3 pos) {
        this->pos = pos;
    }
    void SetPosition(float x, float y, float z) {
        this->pos = glm::vec3(x, y, z);
    }

    glm::vec3& GetPosition() {
        return this->pos;
    }

    void SetRotation(glm::vec3 rot) {
        this->rot = pos;
    }
    void SetRotation(float x, float y, float z) {
        this->rot = glm::vec3(x, y, z);
    }

    glm::vec3& GetRotation() {
        return this->rot;
    }

    void SetScale(float s) {
        this->scale = glm::vec3(s);
    }

    void SetScale(float sx, float sy, float sz) {
        this->scale = glm::vec3(sx, sy, sz);
    }

    void SetScale(glm::vec3 s) {
        this->scale = s;
    }

    glm::vec3& GetScale() {
        return this->scale;
    }

    CGameModel* GetModel() {
        return this->model;
    }

    CGameActor() {
    }

    glm::mat4 GetModelMatrix() {
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), this->GetPosition());
        modelMatrix = glm::rotate(modelMatrix, glm::radians(this->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(this->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(this->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, this->GetScale());
        return modelMatrix;
    }

    virtual void onActorInit() = 0;
    virtual void onActorUpdate(float timespan) = 0;
    virtual void onActorRender() = 0;
    virtual void onActorDestroy() = 0;
};

struct SDirectionalLight {
    int width = 2048;
    int height = 2048;
    GLuint depth_fbo;
    GLuint depth_map;

    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    void initDepthMap() {

        glGenFramebuffers(1, &this->depth_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, this->depth_fbo);

        glGenTextures(1, &this->depth_map);
        glBindTexture(GL_TEXTURE_2D, this->depth_map);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, this->width, this->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                     NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depth_map, 0);
        glDrawBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            ABORT_EXIT("Shadow map framebuffer is incomplete!\n");
        }
    }

    void reInitDepthMap() {
        glDeleteTextures(1, &this->depth_map);
        glDeleteFramebuffers(1, &this->depth_fbo);
        this->initDepthMap();
    }

    glm::mat4 getMatrix() {
        glm::mat4 dirLightProj = glm::ortho<float>(-80, 80, -80, 80, -20, 200);
        glm::mat4 dirLightView =
            glm::lookAt(-this->direction * 20.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 dirLightMatrix = dirLightProj * dirLightView;
        return dirLightMatrix;
    }
};

struct SPointLight {
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class CGameScene {

  protected:
    int scene_id = 0;
    std::vector<CGameCamera*> cameras;
    std::vector<CGameActor*> actors;

  public:
    ESceneRenderType renderType = RENDER_NORMAL;
    bool isUserInterfaceMode = false;
    physx::PxScene* physScene = nullptr;

    SDirectionalLight mainDirLight;
    std::vector<SPointLight> pointLights;

    void setLightsToShader1(COpenGLShader* shader) {

        glUseProgram(shader->GetProgramID());

        /* Send directional light */
        shader->SetUniform("mainDirectionalLight.direction", this->mainDirLight.direction);
        shader->SetUniform("mainDirectionalLight.ambient", this->mainDirLight.ambient);
        shader->SetUniform("mainDirectionalLight.diffuse", this->mainDirLight.diffuse);
        shader->SetUniform("mainDirectionalLight.specular", this->mainDirLight.specular);

        /* Shadow map and light matrix */
        glm::mat4 lightMat = this->mainDirLight.getMatrix();
        shader->SetUniform("dirLightMatrix", lightMat);
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, this->mainDirLight.depth_map);
        shader->SetUniform("mainDirectionalLight.shadowMap", 7);

        /* Send each point lights */
        for (int i = 0; (i < 8) && (i < this->pointLights.size()); i++) {

            std::string number = std::to_string(i);
            SPointLight& plight = this->pointLights[i];
            shader->SetUniform(("pointLights[" + number + "].position").c_str(), plight.position);
            shader->SetUniform(("pointLights[" + number + "].ambient").c_str(), plight.ambient);
            shader->SetUniform(("pointLights[" + number + "].diffuse").c_str(), plight.diffuse);
            shader->SetUniform(("pointLights[" + number + "].specular").c_str(), plight.specular);
            shader->SetUniform(("pointLights[" + number + "].constant").c_str(), plight.constant);
            shader->SetUniform(("pointLights[" + number + "].linear").c_str(), plight.linear);
            shader->SetUniform(("pointLights[" + number + "].quadratic").c_str(), plight.quadratic);
        }

        shader->SetUniform("pointLightCount", std::min(8, (int)this->pointLights.size()));
    }

    CGameScene(int id) {
        this->scene_id = id;

        physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
        sceneDesc.cpuDispatcher = gDispatcher;
        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        this->physScene = gPhysics->createScene(sceneDesc);

        physx::PxPvdSceneClient* pvdClient = this->physScene->getScenePvdClient();
        if (pvdClient) {
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }
    }

    ~CGameScene() {
        for (auto& actor : this->actors) {
            actor->onActorDestroy();
        }
        for (auto& camera : this->cameras) {
            delete camera;
        }
    }

    void registerActor(CGameActor* actor);
    void registerCamera(CGameCamera* camera);

    virtual void onSceneUpdate(float timespan);
    virtual void onSceneRender(float timespan);
};

class CLevel {
  protected:
    std::string name = "UNKNOWN_LVL_MAP";
    CGameScene* gameScene = nullptr;

#ifdef _DEBUG
    glm::vec3 script_SpawnPointPos = glm::vec3(0.0f);
    glm::vec3 script_SpawnPointRot = glm::vec3(0.0f);
#endif

  public:
    virtual void updateAndRender(float timespan) = 0;

#ifdef _DEBUG
    virtual void updateImGui(float timespan) = 0;
#endif
};