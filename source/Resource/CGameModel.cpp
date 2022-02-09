#include "CGameModel.h"

CResourceManager<CGameModel> gModelResourceMgr;

void CGameModel::Load(std::string filepath) {

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcessPreset_TargetRealtime_Fast);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        ABORT_EXIT("Failed loading model %s\n\nError: %s\n", filepath.c_str(), importer.GetErrorString());
    }

    this->materials.reserve(scene->mNumMaterials);
    this->ProcessMaterials(filepath, scene);

    // https://learnopengl.com/img/model_loading/assimp_structure.png
    this->ProcessNode(scene, scene->mRootNode);
}

void CGameModel::ProcessNode(const aiScene* scene, aiNode* node) {
    // Process each mesh
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        this->ProcessMesh(scene, scene->mMeshes[node->mMeshes[i]]);
    }

    // Process all children meshes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        this->ProcessNode(scene, node->mChildren[i]);
    }
}

void CGameModel::ProcessMesh(const aiScene* scene, aiMesh* mesh) {

    CGameModelMesh* newMesh = new CGameModelMesh();
    newMesh->name = mesh->mName.C_Str();
    newMesh->vertices.reserve(mesh->mNumVertices);
    newMesh->indices.reserve(mesh->mNumFaces * 3);

    // Process each vertex of the mesh
    // Vertex pos, normal, UV, tangent, bitangent
    for (unsigned int vId = 0; vId < mesh->mNumVertices; vId++) {

        CGameModelVertex vertex;
        vertex.position.x = mesh->mVertices[vId].x;
        vertex.position.y = mesh->mVertices[vId].y;
        vertex.position.z = mesh->mVertices[vId].z;

        if (mesh->HasNormals()) {
            vertex.normal.x = mesh->mNormals[vId].x;
            vertex.normal.y = mesh->mNormals[vId].y;
            vertex.normal.z = mesh->mNormals[vId].z;
        }

        if (mesh->mTextureCoords[0] != nullptr) {
            vertex.texture_uv.x = mesh->mTextureCoords[0][vId].x;
            vertex.texture_uv.y = mesh->mTextureCoords[0][vId].y;

            vertex.tangent.x = mesh->mTangents[vId].x;
            vertex.tangent.y = mesh->mTangents[vId].y;
            vertex.tangent.z = mesh->mTangents[vId].z;

            vertex.bitangent.x = mesh->mBitangents[vId].x;
            vertex.bitangent.y = mesh->mBitangents[vId].y;
            vertex.bitangent.z = mesh->mBitangents[vId].z;
        } else {
            vertex.texture_uv = glm::vec2(.0f);
        }

        newMesh->vertices.push_back(vertex);
    }

    // Process faces (for element buffer )
    for (unsigned fId = 0; fId < mesh->mNumFaces; fId++) {
        aiFace* face = &mesh->mFaces[fId];
        for (unsigned int iId = 0; iId < face->mNumIndices; iId++) {
            newMesh->indices.push_back(face->mIndices[iId]);
        }
    }

    newMesh->material = this->materials[mesh->mMaterialIndex];
    newMesh->SetupOpenGLBuffers();

    this->meshes.push_back(newMesh);
}

void CGameModel::ProcessMaterials(std::string basepath, const aiScene* scene) {
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {

        aiMaterial* material = scene->mMaterials[i];
        CGameModelMaterial* gameMat = new CGameModelMaterial();

        aiColor3D color(0.0f, 0.0f, 0.0f);
        float temp;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        gameMat->diffuse_color = glm::vec3(color.r, color.g, color.b);
        material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        gameMat->ambient_color = glm::vec3(color.r, color.g, color.b);
        material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
        gameMat->emissive_color = glm::vec3(color.r, color.g, color.b);
        material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        gameMat->specular_color = glm::vec3(color.r, color.g, color.b);
        material->Get(AI_MATKEY_SHININESS, temp);
        gameMat->specular_power = temp;

        aiString texturePath;
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
            std::string actualPath = texturePath.C_Str();
            if (actualPath.find("\\/") == std::string::npos) {
                actualPath = GetBaseDirectory(basepath) + texturePath.C_Str();
            }
            gameMat->diffuse_texture = gTextureResourceMgr.LoadResource(actualPath);
        }

        if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
            material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath);
            std::string actualPath = texturePath.C_Str();
            if (actualPath.find("\\/") == std::string::npos) {
                actualPath = GetBaseDirectory(basepath) + texturePath.C_Str();
            }
            gameMat->emissive_texture = gTextureResourceMgr.LoadResource(actualPath);
        }

        if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
            material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
            std::string actualPath = texturePath.C_Str();
            if (actualPath.find("\\/") == std::string::npos) {
                actualPath = GetBaseDirectory(basepath) + texturePath.C_Str();
            }
            gameMat->specular_texture = gTextureResourceMgr.LoadResource(actualPath);
        }

        if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
            material->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
            std::string actualPath = texturePath.C_Str();
            if (actualPath.find("\\/") == std::string::npos) {
                actualPath = GetBaseDirectory(basepath) + texturePath.C_Str();
            }
            gameMat->normal_texture = gTextureResourceMgr.LoadResource(actualPath);
        }

        this->materials.push_back(gameMat);
    }
}

void CGameModelMesh::SetupOpenGLBuffers() {
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CGameModelVertex) * this->vertices.size(), this->vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), this->indices.data(),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CGameModelVertex),
                          (void*)offsetof(CGameModelVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CGameModelVertex),
                          (void*)offsetof(CGameModelVertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CGameModelVertex),
                          (void*)offsetof(CGameModelVertex, texture_uv));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(CGameModelVertex),
                          (void*)offsetof(CGameModelVertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(CGameModelVertex),
                          (void*)offsetof(CGameModelVertex, bitangent));
}

void CGameModel::renderNormal(COpenGLShader* shader, CGameCamera* camera, glm::mat4 model) {

    glm::mat4 View = camera->ViewMatrix();
    glm::mat4 Proj =
        glm::perspective(glm::radians(camera->GetFOV()), camera->GetRatio(), camera->GetNear(), camera->GetFar());

    glUseProgram(shader->GetProgramID());
    shader->SetUniform("Model", model);
    shader->SetUniform("View", View);
    shader->SetUniform("Projection", Proj);
    shader->SetUniform("viewPosition", camera->GetPosition());

    glEnable(GL_TEXTURE_2D);

    for (CGameModelMesh* mesh : this->meshes) {
        glBindVertexArray(mesh->vao);
        mesh->material->Apply(shader);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
    }
}

void CGameModel::renderShadow(COpenGLShader* shader, glm::mat4 model) {
    shader->SetUniform("modelMatrix", model);
    for (CGameModelMesh* mesh : this->meshes) {
        glBindVertexArray(mesh->vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
    }
}