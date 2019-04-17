#version 330

// input attributes
layout(location = 0) in vec3 position;


uniform mat4 mdvMat;      
uniform mat4 projMat;     
uniform mat3 normalMat;  

uniform sampler2D heightmap; // texture de perlin
uniform sampler2D normalmap; // texture de la normale
uniform sampler2D montagneTextu; // texture de montagne


out vec3 normalView;
out vec3 eyeView;
out vec3 tangentView;
out vec2 texCoord;
out vec4 pos;


void main() {
	vec3 position_w = position; // car impossible de modifier position qui est readonly
	texCoord = position.xy *0.5 + 0.5;

	position_w.z = texture(heightmap,texCoord).z; // on crée les montagnes
	eyeView     = normalize((mdvMat*vec4(position_w,1.0)).xyz);
  	normalView  = normalize(normalMat*texture(normalmap,position.xy*0.5+0.5).xyz);
	gl_Position = projMat*mdvMat*vec4(position_w, 1.0);
	pos = gl_Position;

}