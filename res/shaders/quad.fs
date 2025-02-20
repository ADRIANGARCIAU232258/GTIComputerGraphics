varying vec2 v_uv;


void main()
{
	float dist = distance(v_uv, vec2(0.5));
	gl_FragColor = vec4(vec3(1.0-dist), 1.0);
}