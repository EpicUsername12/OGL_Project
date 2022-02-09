#pragma once

#include "../Utils/DebugUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

class CResourceBase {
    virtual void Load(std::string filepath) = 0;
};

template <typename T> class CResourceManager {
    static_assert(std::is_base_of<CResourceBase, T>::value, "Type T is not derived from CResourceBase");

    std::unordered_map<std::string, T*> resources;

  public:
    T* LoadResource(std::string path) {

        if (this->resources.contains(path)) {
            return this->resources.at(path);
        }

        // Allocate resource and return allocated ptr
        T* resource = new T();
        resource->Load(path);

        this->resources.insert(std::make_pair(path, resource));
        return resource;
    }
};
