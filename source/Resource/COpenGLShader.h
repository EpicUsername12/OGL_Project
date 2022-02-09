#pragma once

#include "../Utils/FileUtils.h"

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CResourceManager.h"

#include <unordered_map>
#include <string>

class COpenGLShader : CResourceBase {
    GLuint program_id = 0;

  public:
    COpenGLShader() {
    }

    virtual void Load(std::string filepath) {
        this->LoadShaders(filepath + ".vs", filepath + ".fs", filepath + ".gs");
    }

    void LoadShaders(std::string vsFilepath, std::string fsFilepath, std::string gsFilepath);

    std::unordered_map<std::string, int> locations;

    inline GLuint GetProgramID() {
        return this->program_id;
    }

    inline void SetUniform(const char* name, float x) {
        glUniform1f(this->locations[name], x);
    }

    inline void SetUniform(const char* name, int x) {
        glUniform1i(this->locations[name], x);
    }

    inline void SetUniform(const char* name, glm::vec3& x) {
        glUniform3fv(this->locations[name], 1, glm::value_ptr(x));
    }

    inline void SetUniform(const char* name, float x, float y, float z) {
        const GLfloat data[3] = { x, y, z };
        glUniform3fv(this->locations[name], 1, data);
    }

    inline void SetUniform(const char* name, glm::vec4& x) {
        glUniform4fv(this->locations[name], 1, glm::value_ptr(x));
    }

    inline void SetUniform(const char* name, float x, float y, float z, float w) {
        const GLfloat data[4] = { x, y, z, w };
        glUniform4fv(this->locations[name], 1, data);
    }

    inline void SetUniform(const char* name, glm::mat4& x) {
        glUniformMatrix4fv(this->locations[name], 1, GL_FALSE, glm::value_ptr(x));
    }
};

extern CResourceManager<COpenGLShader> gShaderResourceMgr;