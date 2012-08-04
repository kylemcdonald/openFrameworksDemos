uniform sampler2DRect remap, source;

// which components you use (rg, gb) depend on how you stored the texture
// and it looks like there's an rb bug with float loading atm.

void main(void) {
	vec2 st = gl_TexCoord[0].st;
	
	// offset style
	vec2 offset = texture2DRect(remap, st).gb - .5; // values from 0 to 1
	gl_FragColor = texture2DRect(source, st + offset * 128.);
	
	// absolute style
	//vec2 target = texture2DRect(remap, st).rg * vec2(1280., 720.); // values from 0 to 1
	//gl_FragColor = texture2DRect(source, target);
}