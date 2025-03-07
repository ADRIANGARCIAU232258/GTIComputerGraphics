#version 330 core

in vec3 v_color;
in vec2 v_uv;  // <-- Recibir coordenadas de textura

uniform sampler2D u_texture; // <-- Agregar textura

out vec4 FragColor;

void main() {
    vec4 texColor = texture(u_texture, v_uv); // <-- Muestrear la textura
    FragColor = vec4(v_color, 1.0) * texColor; // <-- Multiplicar el color de Gouraud por la textura
}
