#version 330

layout(location = 0) in vec3 position;

out vec2 texcoord;


void main() {

gl_Position = vec4(position,1);

texcoord = position.xy*0.5+0.5;

}
