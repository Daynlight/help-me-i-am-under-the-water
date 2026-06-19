#version 430 core

in vec2 TexCoords;
out vec4 FragColor;

uniform mat4 transformation;
uniform int lightCount;
uniform vec3 cameraPosition;
uniform int material_id;
uniform mat4 model;

struct Light {
  vec3 position;
  vec3 color;
  float strength;
};

layout(std430, binding = 0) buffer LightsBuffer {
  Light lights[];
};

struct Material {
  vec3 albedo;
  float metallic;
  float roughness;
  vec3 emission_color;
  float emission_strength;
  float ambient_occlusion;
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
  return kD * albedo / PI;
}

vec3 BRDF(vec3 Normal, vec3 FragPos, vec3 cameraPos, vec3 lightPos, vec3 lightColor, vec3 albedo, float metallic, float roughness, vec3 emission_color, float emission_strength, float ambient_occlusion) {
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
}




float sdElipsolid(vec3 p, vec3 center, vec3 coff, float radius) {
  return length((p - center) / coff) - radius;
}

float sdCone(vec3 p, vec3 center, vec2 c, float h) {
  vec3 translatedP = p - center;
  float q = length(translatedP.xz);
  return max(dot(c.xy, vec2(q, translatedP.y)), -h - translatedP.y);
}

float sdConeUpsideDown(vec3 p, vec3 center, vec2 c, float h) {
  vec3 translatedP = p - center;
  
  translatedP.y = -translatedP.y; 
  
  float q = length(translatedP.xz);
  return max(dot(c.xy, vec2(q, translatedP.y)), -h - translatedP.y);
}



float map(vec3 p) {
  float minDist = 100000.0; 

  float elipsolidTop = sdElipsolid(p, vec3(0.0, 0.0, 0.0), vec3(1.5, 1, 1.5), 5.0);
  float elipsolidBottomCut = sdElipsolid(p, vec3(0.0, -5.0, 0.0), vec3(1.7, 1, 1.7), 5.0);

  minDist = max(elipsolidTop, -elipsolidBottomCut);

  float cone1 = sdConeUpsideDown(p, vec3(0.0, -8, 0), vec2(0.9, 0.5), 9);

  minDist = min(minDist, cone1);

  return minDist;
}




vec3 calculateNormal(vec3 p) {
  vec2 eps = vec2(0.005, 0.0);
  return normalize(vec3(
    map(p + eps.xyy) - map(p - eps.xyy),
    map(p + eps.yxy) - map(p - eps.yxy),
    map(p + eps.yyx) - map(p - eps.yyx)
  ));
}

void main() {
  vec2 uv = TexCoords * 2.0 - 1.0;
  mat4 invTransformation = inverse(transformation);
  mat4 invModel = inverse(model);
  
  vec4 nearTarget = invTransformation * vec4(uv, -1.0, 1.0);
  vec4 farTarget  = invTransformation * vec4(uv,  1.0, 1.0);

  vec3 world_ro = nearTarget.xyz / nearTarget.w;
  vec3 world_rd = normalize((farTarget.xyz / farTarget.w) - world_ro);

  vec3 ro = (invModel * vec4(world_ro, 1.0)).xyz;
  vec3 rd = normalize((invModel * vec4(world_rd, 0.0)).xyz);

  float t = 0.0;
  float max_t = 4000.0; 
  bool hit = false;
  vec3 hitPosLocal = vec3(0.0);

  for(int i = 0; i < 200; i++) { 
    vec3 p = ro + rd * t;
    float d = map(p);
    if(d < 0.005) { 
      hit = true;
      hitPosLocal = p;
      break;
    }
    if(t > max_t) break;
    t += d;
  }

  if(!hit) {
    discard; 
  }



  float insideThickness = 0.0;
  float inside_t = t + 0.03; 
  float max_thickness_dist = t + 20.0; 
  
  for(int i = 0; i < 40; i++) {
    vec3 insideP = ro + rd * inside_t;
    float d = map(insideP);
    
    if(d < 0.0) {
      insideThickness += abs(d);
      inside_t += max(abs(d), 0.1);
    } else {
      inside_t += max(d, 0.1); 
    }
    
    if(inside_t > max_thickness_dist || inside_t > max_t) {
      break;
    }
  }

  float densityCoefficient = 0.15;
  float alpha = 1.0 - exp(-insideThickness * densityCoefficient);


  vec3 hitPosWorld = (model * vec4(hitPosLocal, 1.0)).xyz;
  
  vec4 clipPos = transformation * vec4(hitPosWorld, 1.0);
  float ndcDepth = clipPos.z / clipPos.w;
  gl_FragDepth = ndcDepth * 0.5 + 0.5;

  vec3 localNormal = calculateNormal(hitPosLocal);
  vec3 normal = normalize(mat3(transpose(invModel)) * localNormal);
  vec3 totalLighting = vec3(0.0);
  
  for(int i = 0; i < lightCount; i++) {
    totalLighting += BRDF(
      normal,
      hitPosWorld,
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
  }
  FragColor = vec4(totalLighting, alpha);
}



