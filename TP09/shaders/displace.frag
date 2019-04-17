#version 330

out vec4 outBuffer;

uniform sampler2D heightmap; // texture de perlin
uniform sampler2D normalmap; // texture de la normale
uniform sampler2D snowTextu;
uniform sampler2D montagneTextu;
uniform sampler2D grassTextu;


uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
uniform vec3 light; // lumière 
in vec2 texCoord;
in  vec3  normalView;
in  vec3  eyeView;

void main(){
	
	 //Lumiere phong

    float et = 20.0;
    vec3 n = normalize(normalView);
    vec3 e = normalize(eyeView);
    vec3 l = normalize(light);


    float diff = max(dot(l,n),0.);
    float spec = pow(max(dot(reflect(l,n),e),0),et);

    // CHOIX DE LA TEXTURE A AFFICHER EN FONCTION DE LA HAUTEUR
   	vec4 selelctedTexture;

	float height = texture(heightmap, texCoord).z;

	if(height > 0.75){

		selelctedTexture = texture(snowTextu, texCoord);

	} else if(height <= 0.75 && height > 0.15){

		selelctedTexture = texture(montagneTextu, texCoord);

	} else {
		selelctedTexture = texture(grassTextu, texCoord);

}

    outBuffer = selelctedTexture*(diff + spec)*2.0;
}