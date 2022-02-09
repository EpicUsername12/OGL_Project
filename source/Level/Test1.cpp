#include "Test1.h"

Level_Test1::Level_Test1(GLFWwindow* window, GLFWvidmode* videoMode) {

    this->name = "Test1";
    this->gameScene = new CGameScene(1);

    this->camera = new CGameCamera(90.0f, 16.0f / 9.0f);
    this->gameScene->registerCamera(this->camera);

    this->model = gModelResourceMgr.LoadResource("game_files/model/untitled.obj");
    this->modelActor = new CSimpleModel(this->gameScene, this->camera, this->model);

    this->firstPersonControl = new CPlayerFPS(window, videoMode, this->gameScene, this->camera);
    this->firstPersonControl->pos = glm::vec3(3, 15, -3);

    this->bomb1 = new CBobOmb(this->gameScene, this->camera);
    this->bomb1->pos = glm::vec3(-5, 5, -18);

    this->gameScene->registerActor(this->modelActor);
    this->gameScene->registerActor(this->firstPersonControl);
    this->gameScene->registerActor(this->bomb1);

    this->gameScene->mainDirLight.direction = glm::vec3(1, -1, 1);
    this->gameScene->mainDirLight.diffuse = glm::vec3(0.8f);
    this->gameScene->mainDirLight.ambient = glm::vec3(0.1f);
    this->gameScene->mainDirLight.specular = glm::vec3(0.4f);
}

#ifdef _DEBUG
void Level_Test1::updateImGui(float timespan) {

    if (!this->gameScene->isUserInterfaceMode) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
    }

    ImGui::Begin("Selection: Current level globals");
    if (ImGui::TreeNode("Spawn point")) {
        ImGui::InputFloat3("Position", glm::value_ptr(this->script_SpawnPointPos));
        ImGui::InputFloat3("Rotation", glm::value_ptr(this->script_SpawnPointRot));
        if (ImGui::Button("Select from current position")) {
            this->script_SpawnPointPos = this->firstPersonControl->pos;
            this->script_SpawnPointRot = this->camera->dir;
        }
        ImGui::TreePop();
    }
    ImGui::End();
}
#endif

Level_Test1::~Level_Test1() {
    delete this->model;
    delete this->gameScene; //< Delete scene after everything, it will also delete all registered actors/cameras
}