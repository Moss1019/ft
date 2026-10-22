// fragment shader

#version 440 core

in vec3 vNormal;
in vec3 vFragPos;

uniform vec3 uLightPos;
uniform vec3 uLightColor;

out vec4 f_color;

void main()
{
    vec3 N = normalize(vNormal);
        vec3 L = normalize(uLightPos - vFragPos);

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = diff * uLightColor;

        vec3 ambient = 0.1 * uLightColor;

        vec3 result = (ambient + diffuse) * vec3(1.0);

        f_color = vec4(result, 1.0);
}