uniform sampler2DRect base, tex;

void main() {
	vec3 a = texture2DRect(base, gl_FragCoord.xy).rgb - .5;
	vec3 b = texture2DRect(tex, gl_TexCoord[0].xy).rgb - .5;
	gl_FragColor = vec4(clamp((a + b) + .5, 0., 1.), 1.);
}
