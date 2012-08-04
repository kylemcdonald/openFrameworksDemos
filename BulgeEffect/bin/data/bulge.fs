uniform sampler2DRect tex;
uniform vec2 center;
uniform float radius, shape;

const float PI = 3.14159;

void main() {
	vec2 offset = center - gl_TexCoord[0].xy;
	float mag = length(offset) / radius;
	//mag = 1. - pow(mag, shape); // simpler, c0, not c1 (better in the center)
	mag = (cos(PI * pow(min(mag, 1.), shape)) + 1.) / 2.; // complex , c0 and c1 (better near the edges)
	offset *= max(mag, 0.);
	//gl_FragColor = vec4(offset, 0., 1.);
	gl_FragColor = texture2DRect(tex, (gl_TexCoord[0].xy + offset));
}
