#pragma once

#include "COpenGLTexture.h"
#include "COpenGLShader.h"
#include "../Engine/CGameCamera.h"

#include <vector>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <optional>

struct CGameModelMaterial {
    glm::vec3 diffuse_color = glm::vec3(0.0f);
    glm::vec3 ambient_color = glm::vec3(0.0f);
    glm::vec3 emissive_color = glm::vec3(0.0f);
    glm::vec3 specular_color = glm::vec3(0.0f);
    float specular_power = 2.0f;

    std::optional<COpenGLTexture*> diffuse_texture;
    std::optional<COpenGLTexture*> emissive_texture;
    std::optional<COpenGLTexture*> specular_texture;
    std::optional<COpenGLTexture*> normal_texture;

    void Apply(COpenGLShader* shader) {

        // Diffuse texture/color
        {
            if (this->diffuse_texture.has_value()) {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, this->diffuse_texture.value()->id);
                shader->SetUniform("meshMaterial.m_DiffuseTexture", 1);
                shader->SetUniform("meshMaterial.hasDiffuseTexture", true);
            } else {
                shader->SetUniform("meshMaterial.m_DiffuseComp", this->diffuse_color);
                shader->SetUniform("meshMaterial.hasDiffuseTexture", false);
            }
        }

        // Emissive texture/color
        {
            if (this->emissive_texture.has_value()) {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, this->emissive_texture.value()->id);
                shader->SetUniform("meshMaterial.m_EmissiveTexture", 2);
                shader->SetUniform("meshMaterial.hasEmissiveTexture", true);
            } else {
                shader->SetUniform("meshMaterial.m_EmissiveComp", this->emissive_color);
                shader->SetUniform("meshMaterial.hasEmissiveTexture", false);
            }
        }

        // Specular texture/color
        {
            if (this->specular_texture.has_value()) {
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, this->specular_texture.value()->id);
                shader->SetUniform("meshMaterial.m_SpecularTexture", 3);
                shader->SetUniform("meshMaterial.hasSpecularTexture", true);
            } else {
                shader->SetUniform("meshMaterial.m_SpecularComp", this->specular_color);
                shader->SetUniform("meshMaterial.hasSpecularTexture", false);
            }
        }

        shader->SetUniform("meshMaterial.m_SpecularPower", this->specular_power);
        shader->SetUniform("meshMaterial.m_AmbientComp", this->ambient_color);
    }
};

struct CGameModelVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct CGameModelMesh {
    std::string name;
    std::vector<CGameModelVertex> vertices;
    std::vector<unsigned int> indices;
    CGameModelMaterial* material;

    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    ~CGameModelMesh() {

        this->vertices.clear();
        this->vertices.shrink_to_fit();

        this->indices.clear();
        this->indices.shrink_to_fit();
    }

    void SetupOpenGLBuffers();
};

class CGameModel : CResourceBase {

    void ProcessNode(const aiScene* scene, aiNode* node);
    void ProcessMesh(const aiScene* scene, aiMesh* mesh);
    void ProcessMaterials(std::string basepath, const aiScene* scene);

  public:
    std::vector<CGameModelMesh*> meshes;
    std::vector<CGameModelMaterial*> materials;

    CGameModel() {
    }

    virtual void Load(std::string filepath);

    void renderNormal(COpenGLShader* shader, CGameCamera* camera, glm::mat4 model);
    void renderShadow(COpenGLShader* shader, glm::mat4 model);

    ~CGameModel() {

        for (auto mesh : this->meshes) {
            delete mesh;
        }

        for (auto mat : this->materials) {
            delete mat;
        }
    }
};

extern CResourceManager<CGameModel> gModelResourceMgr;
