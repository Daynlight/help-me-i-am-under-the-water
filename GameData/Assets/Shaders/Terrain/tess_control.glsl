#version 430 core

layout(vertices = 4) out;

uniform vec3 cameraPosition;
uniform vec2 tessBound;
uniform float distanceCoefficient; 

float computeTessLevel(vec3 p0, vec3 p1, vec3 p2, vec3 p3){
  vec3 center = (p0 + p1 + p2 + p3) * 0.25;

  vec3 diff = cameraPosition - center;
  float dist = dot(diff, diff);
  
  float maxTess = tessBound.y;
  float minTess = tessBound.x;

  float t = exp(-dist * distanceCoefficient);

  return mix(minTess, maxTess, t);
}

void main(){
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  if (gl_InvocationID == 0){
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    float tessLevel = computeTessLevel(p0, p1, p2, p3);

    gl_TessLevelOuter[0] = tessLevel;
    gl_TessLevelOuter[1] = tessLevel;
    gl_TessLevelOuter[2] = tessLevel;
    gl_TessLevelOuter[3] = tessLevel;

    gl_TessLevelInner[0] = tessLevel;
    gl_TessLevelInner[1] = tessLevel;
  }
}
