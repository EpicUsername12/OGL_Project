// clang-format off

#version 330 core

struct Material {

	vec3 m_DiffuseComp;
	vec3 m_AmbientComp;
	vec3 m_EmissiveComp;
	vec3 m_SpecularComp;
	float m_SpecularPower;

	sampler2D m_DiffuseTexture;
	sampler2D m_EmissiveTexture;
	sampler2D m_SpecularTexture;
	sampler2D m_NormalTexture;

	int hasDiffuseTexture;
	int hasEmissiveTexture;
	int hasSpecularTexture;

};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    sampler2D shadowMap;
};  

uniform DirectionalLight mainDirectionalLight;

struct PointLight {   
 
    vec3 position;

    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform PointLight pointLights[8];
uniform int pointLightCount;

in vec3 fragmentPosition;
in vec3 fragmentNormal;
in vec2 fragmentTextureUV;
in vec3 fragmentTangent;
in vec3 fragmentBitangent;
in vec4 fragmentDirLightSpace;

uniform vec3 viewPosition;
uniform Material meshMaterial;

out vec4 outPixelColor;

vec2 poissonDisk[4] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760)
);

// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float ShadowCalculation1(DirectionalLight light, vec4 fragPosLightSpace) {

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(light.shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fragmentNormal);
    vec3 lightDir = normalize(-light.direction);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(light.shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(light.shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return 1 - shadow;
}

vec4 CalculateDirLightAttenuation(DirectionalLight light, vec4 MaterialAmbient, vec4 MaterialDiffuse, vec4 MaterialSpecular);
vec4 CalculatePointLightAttenuation(PointLight light, vec4 MaterialAmbient, vec4 MaterialDiffuse, vec4 MaterialSpecular);

void main()
{

	vec4 MaterialSpecularColor;
	vec4 MaterialAmbientColor;
	vec4 MaterialDiffuseColor;
	vec4 MaterialEmissiveColor;

	if(meshMaterial.hasDiffuseTexture > 0) {
		MaterialDiffuseColor = texture(meshMaterial.m_DiffuseTexture, fragmentTextureUV);
		MaterialAmbientColor = texture(meshMaterial.m_DiffuseTexture, fragmentTextureUV) * vec4(meshMaterial.m_AmbientComp, MaterialDiffuseColor.a);
		if(MaterialDiffuseColor.a < 0.1) discard;
	} else {
		MaterialDiffuseColor = vec4(meshMaterial.m_DiffuseComp, 1.0);
		MaterialAmbientColor = vec4(meshMaterial.m_DiffuseComp, 1.0) * 0.1;
	}

	if(meshMaterial.hasSpecularTexture > 0) {
		MaterialSpecularColor = texture(meshMaterial.m_SpecularTexture, fragmentTextureUV);
	} else {
		MaterialSpecularColor = vec4(meshMaterial.m_SpecularComp, 1.0);
	}

	if(meshMaterial.hasEmissiveTexture > 0) {
		MaterialEmissiveColor = texture(meshMaterial.m_EmissiveTexture, fragmentTextureUV);
	} else {
		MaterialEmissiveColor = vec4(meshMaterial.m_EmissiveComp, 1.0);
	}

    vec4 totalAttenuation = vec4(MaterialAmbientColor);
	int count = 1;
	totalAttenuation += CalculateDirLightAttenuation(mainDirectionalLight, MaterialAmbientColor, MaterialDiffuseColor, MaterialSpecularColor);
	for(int i = 0; i < pointLightCount; i++) {
		totalAttenuation += CalculatePointLightAttenuation(pointLights[i], MaterialAmbientColor, MaterialDiffuseColor, MaterialSpecularColor);
		count++;
	}

	outPixelColor = totalAttenuation/count;

}

vec4 CalculateDirLightAttenuation(DirectionalLight light, vec4 MaterialAmbient, vec4 MaterialDiffuse, vec4 MaterialSpecular) {

    float shadow = ShadowCalculation1(light, fragmentDirLightSpace);

	vec3 lightDir = normalize(-light.direction);

	// diffuse shading
	float diff = max(dot(fragmentNormal, lightDir), 0.0);
	// specular shading
	vec3 viewDir = normalize(viewPosition - fragmentPosition);
	vec3 reflectDir = reflect(-lightDir, fragmentNormal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), meshMaterial.m_SpecularPower);

	// combine results
	vec4 ambient  = vec4(light.ambient, 1)  * MaterialDiffuse;
	vec4 diffuse  = vec4(light.diffuse, 1)  * diff * MaterialDiffuse  * shadow;
	vec4 specular = vec4(light.specular, 1) * spec * MaterialSpecular * shadow;

	return (ambient + diffuse + specular);

}

vec4 CalculatePointLightAttenuation(PointLight light, vec4 MaterialAmbient, vec4 MaterialDiffuse, vec4 MaterialSpecular) {


	vec3 lightDir = normalize(light.position - fragmentPosition);
	// diffuse shading
	float diff = max(dot(fragmentNormal, lightDir), 0.0);
	// specular shading
	vec3 viewDir = normalize(viewPosition - fragmentPosition);
	vec3 reflectDir = reflect(-lightDir, fragmentNormal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), meshMaterial.m_SpecularPower);

	// attenuation
	float dist        = length(light.position - fragmentPosition);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));  
	// combine results

	vec4 ambient  = vec4(light.ambient, 1)  * MaterialDiffuse;
	vec4 diffuse  = vec4(light.diffuse, 1)  * diff * MaterialDiffuse;
	vec4 specular = vec4(light.specular, 1) * spec * MaterialSpecular;
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);

}