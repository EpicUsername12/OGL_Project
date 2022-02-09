// clang-format off

#version 330 core

layout(location = 0) in vec3 vertexPosition;

out vec3 fragmentPosition;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    fragmentPosition = vec3(Model * vec4(vertexPosition, 1.0));
    gl_Position = Projection * View * vec4(fragmentPosition, 1.0);
}