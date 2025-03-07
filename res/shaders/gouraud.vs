#version 330 core

in vec3 a_position;  
in vec3 a_normal;    
in vec2 a_uv;   // <-- Agregar coordenadas UV

out vec3 v_color;
out vec2 v_uv;  // <-- Pasar las UV al fragment shader

uniform mat4 u_model;
uniform mat4 u_viewprojection;
uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;
uniform vec3 u_Ka;  
uniform vec3 u_Kd;  
uniform vec3 u_Ks;  
uniform float u_shininess;

void main() {
    vec4 worldPosition = u_model * vec4(a_position, 1.0);
    vec3 normal = normalize((u_model * vec4(a_normal, 0.0)).xyz);
    vec3 lightDir = normalize(u_lightPosition - worldPosition.xyz);
    
    float d = length(u_lightPosition - worldPosition.xyz);
    float attenuation = 1.0 / (d * d);

    vec3 ambient = u_Ka * u_lightColor;
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = u_Kd * diff * u_lightColor;
    vec3 viewDir = normalize(-worldPosition.xyz);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = u_Ks * spec * u_lightColor;

    v_color = ambient + attenuation * (diffuse + specular);
    v_uv = a_uv;  // <-- Pasamos las coordenadas de textura

    gl_Position = u_viewprojection * worldPosition;
}
