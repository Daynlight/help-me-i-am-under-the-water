#version 430 core

layout(quads, fractional_even_spacing, ccw) in;

out vec3 FragPosition;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform sampler2D uTexture;
uniform vec2 mapSize;
uniform float maxHeight;
uniform mat4 u_LightSpaceMatrix;

float terrainHeight(vec2 p){
  vec2 worldMin = vec2(-mapSize * 0.5);
  vec2 uv = (p - worldMin) / mapSize;

  vec2 texelSize = 1.0 / textureSize(uTexture, 0);
  
  float texelOffset = 1.5; 
  vec2 offset = texelSize * texelOffset;

  float center = texture(uTexture, uv).r;
  float left   = texture(uTexture, uv + vec2(-offset.x, 0.0)).r;
  float right  = texture(uTexture, uv + vec2(offset.x, 0.0)).r;
  float top    = texture(uTexture, uv + vec2(0.0, offset.y)).r;
  float bottom = texture(uTexture, uv + vec2(0.0, -offset.y)).r;

  float height = (center + left + right + top + bottom) / 5.0;

  return height * maxHeight;
}

vec3 calcNormal(vec2 p, float hCenter){
  float eps = 1.0;

  float hR = terrainHeight(p + vec2(eps, 0.0));
  float hU = terrainHeight(p + vec2(0.0, eps));

  float dHdX = hR - hCenter;
  float dHdZ = hU - hCenter;

  return normalize(vec3(-dHdX, eps, -dHdZ));
}

void main(){
  vec3 p0 = gl_in[0].gl_Position.xyz;
  vec3 p1 = gl_in[1].gl_Position.xyz;
  vec3 p2 = gl_in[2].gl_Position.xyz;
  vec3 p3 = gl_in[3].gl_Position.xyz;

  vec2 uv = gl_TessCoord.xy;

  vec3 a = mix(p0, p1, uv.x);
  vec3 b = mix(p3, p2, uv.x);
  vec3 pos = mix(a, b, uv.y);

  float hCenter = terrainHeight(pos.xz);
  pos.y = hCenter;

  FragPosition = pos;
  Normal  = calcNormal(pos.xz, hCenter);

  FragPosLightSpace = u_LightSpaceMatrix * vec4(pos, 1.0);

  gl_Position = projection * view * vec4(pos, 1.0);
}


