uniform sampler2DRect tex;
varying vec4 cur;

void main() {
	gl_FragColor = vec4(vec3(mod(length(cur.zw), 1.)), 0.);
}
