#include "CGameScene.h"

void CGameScene::registerActor(CGameActor* actor) {
    this->actors.push_back(actor);
    actor->onActorInit();
}

void CGameScene::registerCamera(CGameCamera* cam) {
    this->cameras.push_back(cam);
}

void CGameScene::onSceneUpdate(float timespan) {

    this->physScene->simulate(timespan);
    this->physScene->fetchResults(true);

    for (CGameActor*& actor : this->actors) {
        actor->onActorUpdate(timespan);
    }

    for (auto& cam : this->cameras) {
        cam->onCameraUpdate(timespan);
    }
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void CGameScene::onSceneRender(float timespan) {

    /* Render actors normaly */
    for (CGameActor*& actor : this->actors) {
        actor->onActorRender();
    }
}
