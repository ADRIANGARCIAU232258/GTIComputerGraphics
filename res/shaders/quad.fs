varying vec2 v_uvs;
uniform sampler2D u_texture;
uniform float threshold; // Ajustable (ejemplo: 0.5 para un corte medio)

void main()
{
    vec4 texture_color = texture2D(u_texture, v_uvs);
    float gray = dot(texture_color.rgb, vec3(0.299, 0.587, 0.114));
    float threshold = 0.5;
    // Umbral aplicado directamente a cada canal de color
    vec3 bw = step(vec3(threshold), vec3(gray)); 
    
    gl_FragColor = vec4(bw, texture_color.a);
}
