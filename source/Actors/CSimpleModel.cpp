#include "CSimpleModel.h"

CSimpleModel::CSimpleModel(CGameScene* scene, CGameCamera* camera, CGameModel* model) {
    this->scene = scene;
    this->model = model;
    this->camera = camera;
    this->modelShader = gShaderResourceMgr.LoadResource("game_files/shader/ModelShader");
    this->shadowShader = gShaderResourceMgr.LoadResource("game_files/shader/RenderShadow");
    this->type = EActorType::SIMPLE_MODEL;
}

void CSimpleModel::onActorInit() {

    physx::PxTransform transf(mat4_to_physx(this->pos, this->rot, this->scale));
    physx::PxRigidStatic* rigidStatic = gPhysics->createRigidStatic(transf);
    for (auto& mesh : this->model->meshes) {
        physx::PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = (physx::PxU32)mesh->vertices.size();
        meshDesc.points.stride = sizeof(CGameModelVertex);
        meshDesc.points.data = mesh->vertices.data();

        meshDesc.triangles.count = (physx::PxU32)mesh->indices.size();
        meshDesc.triangles.stride = 3 * sizeof(unsigned int);
        meshDesc.triangles.data = mesh->indices.data();

        physx::PxDefaultMemoryOutputStream writeBuffer;
        physx::PxTriangleMeshCookingResult::Enum result;
        bool status = gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
        if (!status) {
            ABORT_EXIT("Error in gCooking->cookTriangleMesh() = %d\n", (int)result);
        }

        physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
        physx::PxTriangleMesh* triMesh = gPhysics->createTriangleMesh(readBuffer);
        physx::PxTriangleMeshGeometry triMeshGeo(triMesh);
        physx::PxMaterial* triMeshMat = gPhysics->createMaterial(0.5f, 0.5f, 0.5f);
        physx::PxShape* triMeshShape = gPhysics->createShape(triMeshGeo, *triMeshMat, true);
        triMeshShape->userData = (void*)mesh;
        rigidStatic->attachShape(*triMeshShape);
    }

    rigidStatic->userData = (void*)this;
    this->rigidStaticBody = rigidStatic;
    this->scene->physScene->addActor(*rigidStatic);
}

void CSimpleModel::onActorRender() {
    this->scene->setLightsToShader1(this->modelShader);
    this->model->renderNormal(this->modelShader, this->camera, this->GetModelMatrix());
}