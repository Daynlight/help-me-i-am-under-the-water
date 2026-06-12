#version 430 core

out vec4 FragColor;

in vec3 FragPosition;
in vec3 Normal;

uniform int lightCount;
uniform vec3 cameraPosition;
uniform int material_id;


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

float DistributionGGX(vec3 N, vec3 H, float roughness){
  float a = roughness * roughness;
  float a2 = a * a;

  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return a2 / max(denom, 0.0001);
}

float GeometrySchlickGGX(float NdotV, float roughness){
  float r = roughness + 1.0;
  float k = (r * r) / 8.0;

  return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);

  return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 FresnelSchlick(vec3 V, vec3 H, vec3 albedo, float metallic){
  float cosTheta = max(dot(V, H), 0.0);  
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float Calc_D(float NdotH, float roughness){
  float a = roughness * roughness;
  float a2 = a * a;

  float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
  float D = a2 / (PI * denom * denom + 0.0001);
  return D;
}

float GGX(float NdotV, float NdotL, float roughness){
  float k = (roughness + 1.0);
  k = (k * k) / 8.0;

  float G1 = NdotL / (NdotL * (1.0 - k) + k);
  float G2 = NdotV / (NdotV * (1.0 - k) + k);
  float G = G1 * G2;
  return G;
}

vec3 Specular(float NdotL, float NdotV, float D, float G, vec3 Fresnel){
  return (D * G * Fresnel) / max(4.0 * NdotL * NdotV, 0.001);
}

vec3 Diffuse(vec3 Fresnel, vec3 albedo, float metallic){
  vec3 kS = Fresnel;
  vec3 kD = (1.0 - kS) * (1.0 - metallic);

  vec3 diffuse = kD * albedo / PI;
  return diffuse;
}



vec3 BRDF(
  vec3 Normal,
  vec3 FragPos,
  vec3 cameraPos,
  vec3 lightPos,
  vec3 lightColor,
  vec3 albedo,
  float metallic,
  float roughness,
  vec3 emission_color,
  float emission_strength,
  float ambient_occlusion
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

  vec3 lighting = (diffuse + specular) * lightColor * NdotL;
  vec3 ambient = 0.03 * albedo * ambient_occlusion;
  vec3 emissiveColor = emission_color * emission_strength;

  return lighting + ambient + emissiveColor;
};

void main(){
  vec3 N = normalize(Normal);
  vec3 V = normalize(cameraPosition - FragPosition);

  float fresnel = pow(1.0 - max(dot(N, V), 0.0), 5.0);
  fresnel = mix(0.05, 1.0, fresnel);

  vec3 color = material[material_id].albedo * 0.1f;

  for(int i = 0; i < lightCount; i++){
    vec3 L = normalize(lights[i].position - FragPosition);
    
    color += BRDF(
      N,
      FragPosition,
      cameraPosition,
      lights[i].position,
      lights[i].color,
      material[material_id].albedo,
      material[material_id].metallic,
      material[material_id].roughness,
      material[material_id].emission_color,
      material[material_id].emission_strength,
      material[material_id].ambient_occlusion
    ) * lights[i].strength;
  };

  float alpha = clamp(0.2 + fresnel, 0.0, 0.2);
  
  FragColor = vec4(color, alpha);
}