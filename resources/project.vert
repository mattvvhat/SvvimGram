#version 120

uniform int   count = 4;

varying vec4  ecPosition;
varying vec3  ecPosition3;
varying vec3  normal;
varying vec4  ambientGlobal;
varying vec4  projCoord;
const int xxx = 3;
uniform mat4 shadowMatrices[xxx];

varying vec4 projCoors[3];

void main()
{
	ecPosition		= gl_ModelViewMatrix * gl_Vertex;
	ecPosition3		= ecPosition.xyz/ecPosition.w;
  
  projCoord     = shadowMatrices[0] * ecPosition;

	normal        = gl_NormalMatrix * gl_Normal;
	gl_Position		= gl_ModelViewProjectionMatrix * gl_Vertex;
	
	ambientGlobal	= gl_LightModel.ambient * gl_FrontMaterial.ambient;
}