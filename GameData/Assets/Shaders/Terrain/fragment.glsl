#version 430 core

out vec4 FragColor;

in vec3 FragPosition;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform int lightCount;
uniform vec3 cameraPosition;
uniform int material_id;
uniform int u_ShadowEnabled;
uniform sampler2D u_ShadowDepthTexture;

struct Light {
  vec3 position;
  vec3 color;
  float strength;
};

struct Material {
  vec3 albedo;
  float metallic;
  float roughness;
  vec3 emission_color;
  float emission_strength;
  float ambient_occlusion;
};

layout(std430, binding = 0) buffer LightsBuffer {
  Light lights[];
};

layout(std430, binding = 1) buffer MaterialsBuffer {
  Material material[];
};

const float PI = 3.14159265359;

float Calc_D(float NdotH, float roughness){
  float a = roughness * roughness;
  float a2 = a * a;
  float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
  return a2 / (PI * denom * denom + 0.0001);
}

float GGX(float NdotV, float NdotL, float roughness){
  float k = (roughness + 1.0); k = (k * k) / 8.0;
  float G1 = NdotL / (NdotL * (1.0 - k) + k);
  float G2 = NdotV / (NdotV * (1.0 - k) + k);
  return G1 * G2;
}

vec3 Specular(float NdotL, float NdotV, float D, float G, vec3 Fresnel){
  return (D * G * Fresnel) / max(4.0 * NdotL * NdotV, 0.001);
}

vec3 Diffuse(vec3 Fresnel, vec3 albedo, float metallic){
  vec3 kS = Fresnel; vec3 kD = (1.0 - kS) * (1.0 - metallic);
  return kD * albedo / PI;
}

vec3 FresnelSchlick(vec3 V, vec3 H, vec3 albedo, float metallic){
  float cosTheta = max(dot(V, H), 0.0); vec3 F0 = mix(vec3(0.04), albedo, metallic);
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
  projCoords = projCoords * 0.5 + 0.5;

  if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
    return 0.0;
  }

  float currentDepth = projCoords.z;
    
  float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(u_ShadowDepthTexture, 0);

  int radius = 1; 
  float samples = 0.0;

  for(int x = -radius; x <= radius; ++x) {
    for(int y = -radius; y <= radius; ++y) {
      float pcfDepth = texture(u_ShadowDepthTexture, projCoords.xy + vec2(x, y) * texelSize).r;
      shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
      samples += 1.0;
    }
  }

  return shadow / samples;
}

vec3 BRDF(
  vec3 Normal, vec3 FragPos, vec3 cameraPos, vec3 lightPos, vec3 lightColor,
  vec3 albedo, float metallic, float roughness, bool isFirstLight
)
{
  vec3 N = normalize(Normal);
  vec3 V = normalize(cameraPos - FragPos);
  vec3 L = normalize(lightPos - FragPos);
  vec3 H = normalize(V + L);

  float NdotL = max(dot(N, L), 0.0);
  float NdotV = max(dot(N, V), 0.0);
  float NdotH = max(dot(N, H), 0.0);

  vec3 Fresnel = FresnelSchlick(V, H, albedo, metallic);
  float D = Calc_D(NdotH, roughness);
  float G = GGX(NdotV, NdotL, roughness);

  vec3 specular = Specular(NdotL, NdotV, D, G, Fresnel);
  vec3 diffuse = Diffuse(Fresnel, albedo, metallic); 

  float shadow = 0.0;
  if (isFirstLight && u_ShadowEnabled == 1) {
    shadow = CalculateShadow(FragPosLightSpace, N, L);
  }

  return (1.0 - shadow) * (diffuse + specular) * lightColor * NdotL;
}

void main(){
  vec3 finalColor = vec3(1.0f);

  if(u_ShadowEnabled == 1){

    Material mat = material[material_id];
    vec3 N = normalize(Normal);
    vec3 directLighting = vec3(0.0f);
  
    for(int i = 0; i < lightCount; i++){
      bool isFirstLight = (i == 0);
      directLighting += BRDF(
        N, FragPosition, cameraPosition, lights[i].position, lights[i].color,
        mat.albedo, mat.metallic, mat.roughness, isFirstLight
      ) * lights[i].strength;
    }

    vec3 ambient = 0.03 * mat.albedo * mat.ambient_occlusion;
    vec3 emissiveColor = mat.emission_color * mat.emission_strength;

    finalColor = directLighting + ambient + emissiveColor;
  }

  FragColor = vec4(finalColor, 1.0);
}


