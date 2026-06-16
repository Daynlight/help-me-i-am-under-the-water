#version 430 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D u_SceneColorTexture;
uniform sampler2D u_SceneDepthTexture;

uniform float u_FogDensity;
uniform vec3 u_FogColor;

uniform float u_water_height;
uniform vec3 u_CamPos;
uniform mat4 u_InvViewProj;

vec3 GetWorldPos(float depth) {
  float z = depth * 2.0 - 1.0;
  vec4 clipSpacePos = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
  vec4 worldSpacePos = u_InvViewProj * clipSpacePos;
  return worldSpacePos.xyz / worldSpacePos.w;
}

float WaterDepthFog(float camY) {
  float d = max(0.0, u_water_height - camY);
  return 1.0 - exp(-d * u_FogDensity * 2.0);
}

void main() {
  vec3 sceneColor = texture(u_SceneColorTexture, TexCoords).rgb;
  float depth = texture(u_SceneDepthTexture, TexCoords).r;

  vec3 worldPos = GetWorldPos(depth);

  vec3 rayVec = worldPos - u_CamPos;
  float rayLen = length(rayVec);

  vec3 rayDir = normalize(rayVec);

  if (depth >= 1.0) {
    rayLen = 2000.0;
    worldPos = u_CamPos + rayDir * rayLen;
  }

  if (rayLen < 0.0001) {
    FragColor = vec4(sceneColor, 1.0);
    return;
  }

  float camSide = u_CamPos.y - u_water_height;
  float hitSide = worldPos.y - u_water_height;

  float waterDistance = 0.0;

  if (camSide > 0.0 && hitSide > 0.0) {
    waterDistance = 0.0;
  } else if (camSide < 0.0 && hitSide < 0.0) {
    waterDistance = rayLen;
  } else if (camSide > 0.0 && hitSide < 0.0) {
    float tEnter = (u_water_height - u_CamPos.y) / rayDir.y;
    waterDistance = max(0.0, rayLen - tEnter);
  } else if (camSide < 0.0 && hitSide > 0.0) {
    float tExit = (u_water_height - u_CamPos.y) / rayDir.y;
    waterDistance = clamp(tExit, 0.0, rayLen);
  }

  float fogFactor = exp(-u_FogDensity * waterDistance);
  fogFactor = clamp(fogFactor, 0.0, 1.0);

  float depthFog = WaterDepthFog(u_CamPos.y);

  fogFactor *= mix(1.0, 0.4, depthFog);

  vec3 finalColor = mix(u_FogColor, sceneColor, fogFactor);
  FragColor = vec4(finalColor, 1.0);
}