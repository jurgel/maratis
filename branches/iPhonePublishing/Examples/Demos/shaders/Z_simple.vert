#ifdef GL_ES
precision highp float;
#endif

attribute vec3 Vertex;
uniform mat4 ProjModelViewMatrix;

void main(void)
{
	gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);
}