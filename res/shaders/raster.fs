// Recibimos las UV desde el vertex shader
varying vec2 v_uv;

// Textura enviada desde la CPU
uniform sampler2D u_texture;

void main()
{
	gl_FragColor = texture2D(u_texture, v_uv); // Color de la textura en la UV
}