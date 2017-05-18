#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNorm;
layout(location = 2) in vec2 vertexUV;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 fragmentUV;
out vec3 fragmentNormal;
out vec3 fragmentPos;

void main() {
  fragmentUV = vertexUV;
  fragmentNormal = (M * vec4(vertexNorm, 0)).xyz;
  fragmentPos = (M * vec4(vertexPos, 1)).xyz;

  gl_Position = P * V * M * vec4(vertexPos, 1);
}
