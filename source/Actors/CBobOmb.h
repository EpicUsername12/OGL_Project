#pragma once

#include "../Engine/CGameScene.h"
#include "../Resource/COpenGLShader.h"

class CBobOmb : public CGameActor {
    COpenGLShader* modelShader = nullptr;
    COpenGLShader* shadowShader = nullptr;

  public:
    virtual void onActorInit() {
    }
    virtual void onActorUpdate(float timespan) {
    }
    virtual void onActorRender();
    virtual void onActorDestroy() {
    }

    CBobOmb(CGameScene* scene, CGameCamera* camera);
};