uniform sampler2DRect base, tex;

void main() {
	vec3 a = texture2DRect(base, gl_FragCoord.xy).rgb;
	vec3 b = texture2DRect(tex, gl_TexCoord[0].xy).rgb;
	a -= .5, b -= .5;
	gl_FragColor = vec4((a + b) + .5, 1.);
}
