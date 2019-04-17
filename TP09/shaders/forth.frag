#version 330

out vec4 bufferColor;

uniform vec3      light;
uniform sampler2D normalmap;
uniform sampler2D colormap;

in vec2 texcoord;

vec4 shade(in vec2 coord) {
  vec4  nd = texture(normalmap,coord,0);
  vec4  c  = texture(colormap ,coord,0);
  vec4  F = vec4(vec3(0.7),1.);

  vec3  n = nd.xyz;
  float d = nd.w;

  vec3 e = vec3(0,0,-1);
  vec3 l = normalize(light);

  float diff = max(dot(l,n),0.0);
  float spec = pow(max(dot(reflect(l,n),e),0.0),d*10.0);
  vec4 S = vec4(c.xyz*(diff + spec),1);

  
  return mix(S,F,clamp(0.,1.,d));
}

void main() {
  vec4 color = shade(texcoord);
  bufferColor = vec4(color);
}
