uniform sampler2DRect tex;
uniform vec2 mouse;

const float maxVelocity = .01;

void main() {
	vec4 cur = texture2DRect(tex, gl_TexCoord[0].xy);
	vec2 curPos = cur.xy;
	vec2 curVel = (cur.zw - .5) * maxVelocity;
	vec2 diff = (mouse - curPos);
	vec2 nextVel = .1 * diff / length(diff);
	nextVel = mix(nextVel, curVel, .99);
	vec2 nextPos = curPos + nextVel;
	gl_FragColor = vec4(nextPos, nextVel / maxVelocity + .5);
}
