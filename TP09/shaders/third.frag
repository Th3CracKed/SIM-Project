#version 330

out vec4 outBuffer;

uniform sampler2D heightmap;
uniform sampler2D normalmap;

in vec2 texCoord;

void main(){
	outBuffer = texture(normalmap, texCoord);
}