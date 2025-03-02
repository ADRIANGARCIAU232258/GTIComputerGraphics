varying vec2 v_uvs; 
void main()
{
// Set vertex uvs
	v_uvs = gl_MultiTexCoord0.xy;
// Output clip-space
	gl_Position = gl_Vertex;
}