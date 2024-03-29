// clang-format off
#version 330 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 lightMatrix;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = lightMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}  