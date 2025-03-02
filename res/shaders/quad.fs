varying vec2 v_uvs;
uniform sampler2D u_texture;
uniform float angle; // Ángulo en radianes

void main()
{
    float angle = radians(45.0);
    vec2 center = vec2(0.5);
    vec2 uv = v_uvs - center; // Mover origen al centro

    // Matriz de rotación
    float cosA = cos(angle);
    float sinA = sin(angle);
    mat2 rotation = mat2(cosA, -sinA, sinA, cosA);

    uv = rotation * uv;
    uv += center; // Devolver al espacio original
    uv = fract(uv); // Repite la textura en los bordes

    gl_FragColor = texture2D(u_texture, uv);
}
