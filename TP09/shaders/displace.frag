#version 330

out vec4 outBuffer;

uniform sampler2D heightmap;
uniform sampler2D normalmap;
uniform sampler2D montagneTextu;

in vec2 texCoord;

void main(){

	vec4 colorTextu;

	float height = texture(heightmap, texCoord).z;

	if(height > 0.75){

		colorTextu = vec4(0.91,0.89,0.95,1);

	} else if(height <= 0.75 && height > 0.15){

		colorTextu = texture(montagneTextu, texCoord);

	} else {
		colorTextu = vec4(0.05,0.5,0,1);

}

	outBuffer = colorTextu;
	
}