#version 330

// input attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 mdvMat; // modelview matrix (constant for all the vertices)
uniform mat4 projMat; // projection matrix (constant for all the vertices)

out vec2 pos;

void main() {
  // no need for any particular transformation (Identity matrices)
  pos = position.xy*0.5+0.5;
  gl_Position = projMat*mdvMat*vec4(position,1.0);
}
