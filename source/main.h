#pragma once

#include "Utils/PhysXUtils.h"

#include <GL/glew.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <ShlObj_core.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Level/Test1.h"

void CreateDocumentFolder();
void InitializeWindowAndGraphics();
void InitializePhysX();

int main(int argc, char** argv);