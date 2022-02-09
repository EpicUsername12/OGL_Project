#include "CBobOmb.h"

#include "CSimpleModel.h"

CBobOmb::CBobOmb(CGameScene* scene, CGameCamera* camera) {
    this->scene = scene;
    this->camera = camera;
    this->type = EActorType::TEST_BOB_OMB;
    this->modelShader = gShaderResourceMgr.LoadResource("game_files/shader/ModelShader");
    this->shadowShader = gShaderResourceMgr.LoadResource("game_files/shader/RenderShadow");
    this->model = gModelResourceMgr.LoadResource("game_files/model/bomb.obj");
}

void CBobOmb::onActorRender() {
    this->scene->setLightsToShader1(this->modelShader);
    this->model->renderNormal(this->modelShader, this->camera, this->GetModelMatrix());
}