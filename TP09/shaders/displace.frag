#version 330

out vec4 outBuffer;

uniform sampler2D heightmap;
uniform sampler2D normalmap;
uniform sampler2D snowTextu;
uniform sampler2D montagneTextu;
uniform sampler2D grassTextu;

in vec2 texCoord;

void main(){

	vec4 selelctedTexture;

	float height = texture(heightmap, texCoord).z;

	if(height > 0.75){

		selelctedTexture = texture(snowTextu, texCoord);

	} else if(height <= 0.75 && height > 0.15){

		selelctedTexture = texture(montagneTextu, texCoord);

	} else {
		selelctedTexture = texture(grassTextu, texCoord);

}

	outBuffer = selelctedTexture;
	
}