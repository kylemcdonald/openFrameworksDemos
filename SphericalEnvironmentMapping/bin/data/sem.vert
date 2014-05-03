#version 120

varying vec3 e;
varying vec3 n;

void main() {

	e = normalize( vec3( gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1.0 ) ) );
	n = normalize( gl_NormalMatrix * gl_Normal );

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;

}