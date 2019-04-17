#version 330

out vec4 outBuffer;

uniform sampler2D heightmap;
uniform sampler2D normalmap;
uniform sampler2D montagneTextu;

in vec2 texCoord;

void main(){
	outBuffer = texture(montagneTextu, texCoord);
}