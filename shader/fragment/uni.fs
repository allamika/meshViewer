#version 330 core
out vec4 FragColor;

in vec3 Normal;

uniform sampler2D texture_diffuse1;

void main()
{    
    float ambientStrength = 0.5;
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.5f, 0.95f, 0.65f));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor *0.75;



    vec3 objectColor = vec3(1.0f, 0.5f, 0.2f);
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0f);
}