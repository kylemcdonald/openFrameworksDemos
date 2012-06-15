uniform float scale, shape;
const float PI = 3.14159;

void main() {
	vec2 offset = vec2(scale, scale) - gl_FragCoord.xy;
	float mag = length(offset) / scale;
	//mag = 1. - pow(mag, shape); // simpler, c0, not c1 (better in the center)
	mag = (cos(PI * pow(min(mag, 1.), shape)) + 1.) / 2.; // complex , c0 and c1 (better near the edges)
	offset *= max(mag, 0.);
	gl_FragColor = vec4(offset / (2. * scale) + .5, 0., 1.);
}
