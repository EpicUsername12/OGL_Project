#pragma once

#include "../Engine/CGameScene.h"
#include "../Actors/CPlayerFPS.h"
#include "../Actors/CSimpleModel.h"
#include "../Actors/CBobOmb.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Level_Test1 : public CLevel {

    CGameCamera* camera = nullptr;
    CPlayerFPS* firstPersonControl = nullptr;
    CGameModel* model = nullptr;
    CSimpleModel* modelActor = nullptr;

    CBobOmb* bomb1 = nullptr;

  public:
    Level_Test1(GLFWwindow* window, GLFWvidmode* videoMode);
    ~Level_Test1();

    virtual void updateAndRender(float timespan) {
        this->gameScene->onSceneUpdate(timespan);
        this->gameScene->onSceneRender(timespan);
    }

#ifdef _DEBUG
    virtual void updateImGui(float timespan);
#endif
};