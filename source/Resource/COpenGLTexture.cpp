#include "COpenGLTexture.h"

CResourceManager<COpenGLTexture> gTextureResourceMgr;

void COpenGLTexture::Load(std::string filepath) {

    int comp;
    stbi_set_flip_vertically_on_load(true);
    uint8_t* imgFile = stbi_load(filepath.c_str(), &this->width, &this->height, &comp, STBI_rgb_alpha);
    if (!imgFile) {
        ABORT_EXIT("Texture resource loading failed, please check your game files.\n");
    }

    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgFile);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(imgFile);
    return;
}