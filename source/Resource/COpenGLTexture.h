#pragma once

#include "CResourceManager.h"

class COpenGLTexture : CResourceBase {

  public:
    GLuint id = 0;
    int width = 0;
    int height = 0;

  public:
    COpenGLTexture() {
    }
    virtual void Load(std::string filepath);
};

extern CResourceManager<COpenGLTexture> gTextureResourceMgr;