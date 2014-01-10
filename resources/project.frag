#version 120


varying vec3 normal;
varying vec3 ecPosition3;
varying vec4 ecPosition;
varying vec4 ambientGlobal;
varying vec4 projCoord;

uniform sampler2D proj;
uniform float scale = 3.f;

void main (void)
{
  vec3 N = normal;
  vec3 v = ecPosition3;

  vec3 L = normalize(gl_LightSource[0].position.xyz - v);
  vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)
  vec3 R = normalize(-reflect(L,N));
  //calculate Ambient Term:
  vec4 Iamb = gl_FrontLightProduct[0].ambient;
  
  //calculate Diffuse Term:
  vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);
  Idiff = clamp(Idiff, 0.0, 1.0);
  
  vec4 Ispec = gl_FrontLightProduct[0].specular  * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
  Ispec = clamp(Ispec, 0.0, 1.0);

  vec3 coord = 0.5 * (projCoord.xyz / projCoord.w + 1.0);
  
  vec3 eee = v;
  vec3 nnn = R;
  float angle = dot(eee, nnn);
  
  if (coord.x >= 0.f && coord.x <= 1.f && coord.y >= 0.f && coord.y <= 1.f && angle < 0.0) {
    gl_FragColor = texture2DProj(proj, coord/scale);
  }
  else {
    gl_FragColor = vec4(vec3(0.f), 1.f);
  }

  /*
   else if (coord.x >= 0.f && coord.x <= 1.f && coord.y >= 0.f && coord.y <= 1.f) {
    gl_FragColor = max(dot(N, L) , 0.0) * texture2DProj(proj, coord/scale);
    gl_FragColor.a = 1.f;
  }
  else if (coord.x >= 0.f && coord.x <= 1.f && coord.y >= 0.f && coord.y <= 1.f) {
    gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;
    gl_FragColor = max(dot(N,L), 0.0) * texture2DProj(proj, coord/scale);
    gl_FragColor.rgb = vec3(0.f);
    gl_FragColor.a = 1.f;
  }
  else
    gl_FragColor = Iamb + Idiff;
   */

}

/*
varying vec3 normal;
varying vec3 ecPosition3;
varying vec4 ecPosition;
varying vec4 ambientGlobal;
varying vec4 projCoord;

uniform sampler2D proj;
uniform float scale = 3.f;
float distCoord (vec3, vec3, vec3);

void main () {
  // Eye-coordinates
	vec3 eye = -(ecPosition3);
  vec3 neye = -normalize(eye);
	
	// Clear the light intensity accumulators
	vec4 amb  = vec4(0.f);
	vec4 diff = vec4(0.f);
	vec4 spec = vec4(0.f);
  
  // Compute ambient color
  amb = ambientGlobal + gl_LightSource[0].ambient;
  
  // Compute diffuse
  vec3 VP = -vec3(gl_LightSource[0].position);
  vec3 halfVector = normalize(VP + neye);
  float d = length(VP);
  VP = normalize(VP);
  
  float nDotVP  = max(dot(normal, VP), 0.f);
  float nDotHV  = max(dot(normal, gl_LightSource[0].halfVector.xyz), 0.f);
  float ndotl   = max(dot(normal, VP + neye), 0.f);
  
  if (dot(gl_LightSource[0].position.xyz, normal) > 0.0) {
    spec += diff * ndotl;
    halfVector = normalize(halfVector);
    nDotHV = max(dot(normal, halfVector), 0.0);
    spec += gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(nDotHV, 1.f/gl_FrontMaterial.shininess);
  }
  
	diff = nDotVP * gl_LightSource[0].diffuse;
  
  gl_FragColor = amb + diff + spec;
  
  float dist = distCoord(ecPosition.xyz, eye, gl_LightSource[0].position.xyz);
  gl_FragColor.a = 0.f;
  
  vec3 e, n;
  e = gl_LightSource[0].position.xyz;
  n = normal;
 vec3 coord = 0.5 * (projCoord.xyz / projCoord.w + 1.0);
 
  if (coord.x >= 0.f && coord.x <= 1.f && coord.y >= 0.f && coord.y <= 1.f && dot(e, n) > 0.3) {
    gl_FragColor = texture2DProj(proj, coord/scale);
  }
  else
    gl_FragColor = amb + diff;
  
}

float distCoord (vec3 pos, vec3 eye, vec3 light) {
  eye   = eye - pos;
  light = light - pos;
  float d = length(light);
  if (dot (eye, light) > 0.f)
    return d/9.f;
  return 0.f;
}





















*/