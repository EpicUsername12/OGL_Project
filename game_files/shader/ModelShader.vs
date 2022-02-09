// clang-format off

#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTextureUV;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

out vec3 fragmentPosition;
out vec3 fragmentNormal;
out vec2 fragmentTextureUV;
out vec3 fragmentTangent;
out vec3 fragmentBitangent;
out vec4 fragmentDirLightSpace;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 dirLightMatrix;

void main()
{

    fragmentPosition = vec3(Model * vec4(vertexPosition, 1.0));
    fragmentNormal = mat3(transpose(inverse(Model))) * vertexNormal; 
    fragmentTextureUV = vertexTextureUV;
    fragmentDirLightSpace = dirLightMatrix * vec4(fragmentPosition, 1.0);

    /*
        mat3 normalMatrix = transpose(inverse(mat3(model)));
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 N = normalize(normalMatrix * aNormal);
        T = normalize(T - dot(T, N) * N);
        vec3 B = cross(N, T);
    
        mat3 TBN = transpose(mat3(T, B, N));    
        vs_out.TangentLightPos = TBN * lightPos;
        vs_out.TangentViewPos  = TBN * viewPos;
        vs_out.TangentFragPos  = TBN * vs_out.FragPos;
    */

    gl_Position = Projection * View * vec4(fragmentPosition, 1.0);

}