uniform sampler2DRect tex;
varying vec4 cur;

void main() {
	gl_FrontColor = gl_Color;
	cur = texture2DRect(tex, gl_Vertex.xy);
	vec2 pos = cur.xy;
	gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 0., 1.);
}
