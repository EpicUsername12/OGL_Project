// clang-format off

#version 330 core

in vec3 fragmentPosition;
uniform vec3 viewPosition;
out vec4 outPixelColor;

uniform vec4 vertexColor;

void main()
{
	outPixelColor = vertexColor;
}