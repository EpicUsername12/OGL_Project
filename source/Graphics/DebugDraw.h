#pragma once

#include "../Utils/DebugUtils.h"
#include "../Engine/CGameCamera.h"
#include "../Resource/COpenGLShader.h"

#define _USE_MATH_DEFINES
#include <vector>
#include <math.h>

#include <gl/glew.h>

void DrawLine(COpenGLShader* shader, CGameCamera* camera, glm::vec3 posStart, glm::vec3 posEnd);