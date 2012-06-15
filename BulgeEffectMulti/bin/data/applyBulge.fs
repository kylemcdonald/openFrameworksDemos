uniform sampler2DRect tex, bulge;

void main() {
	vec2 position = gl_TexCoord[0].xy;
	vec2 offset = 256. * (texture2DRect(bulge, position).xy - .5);
	gl_FragColor = texture2DRect(tex, position + offset);
}
