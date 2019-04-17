#version 330

// input attributes
layout(location = 0) in vec3 position;


uniform mat4 mdvMat;
uniform mat4 projMat;

uniform sampler2D heightmap;

out vec2 texCoord;

out vec3 pos;

void main() {
    pos = position;

	texCoord = position.xy *0.5 + 0.5;

	pos.z = texture(heightmap,texCoord).z;

	gl_Position = projMat*mdvMat*vec4(pos, 1);

}