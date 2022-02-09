#pragma once

#include "../Engine/CGameScene.h"
#include "../Resource/COpenGLShader.h"

class CSimpleModel : public CGameActor {
    COpenGLShader* modelShader = nullptr;
    COpenGLShader* shadowShader = nullptr;
    physx::PxRigidStatic* rigidStaticBody = nullptr;

  public:
    virtual void onActorInit();
    virtual void onActorUpdate(float timespan) {
    }
    virtual void onActorRender();
    virtual void onActorDestroy() {
    }
    CSimpleModel(CGameScene* scene, CGameCamera* camera, CGameModel* model = nullptr);
};