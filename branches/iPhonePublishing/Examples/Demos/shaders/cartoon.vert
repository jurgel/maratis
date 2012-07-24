#ifdef GL_ES
precision highp float;
#endif

attribute vec3 Vertex;
attribute vec3 Normal;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjModelViewMatrix;
uniform mat4 NormalMatrix;

varying vec3 position, normal;

void main(void)
{
	normal = (NormalMatrix * vec4(Normal, 1.0)).xyz;
	position = (ModelViewMatrix * vec4(Vertex, 1.0)).xyz;
	gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);
}