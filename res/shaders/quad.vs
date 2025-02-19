varying vec2 v_uv;

void main()
{
	// Set vertex uvs
	v_uv = gl_MultiTexCoord0.xy;
	// Output clip-space
	gl_Position = gl_Vertex;
}