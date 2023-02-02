#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  


out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform Material material;
uniform vec3 viewPos;

#define NB_POINT_LIGHTS 128
uniform PointLight pointLights[NB_POINT_LIGHTS];
uniform int nbPointLight;

#define NB_DIR_LIGHTS 128
uniform DirLight directionLights[NB_DIR_LIGHTS];
uniform int nbDirLight;

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir);
vec3 CalcDirectionLight(DirLight light, vec3 norm, vec3 viewDir);

void main()
{
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    for(int i=0; i<NB_POINT_LIGHTS && i<nbPointLight; i ++){
        result += CalcPointLight(pointLights[i], Normal, FragPos, viewDir);
    }

    for(int i=0; i<NB_DIR_LIGHTS && i<nbDirLight; i ++){
        result += CalcDirectionLight(directionLights[i], Normal, viewDir);
    }
    
    
    
    FragColor = vec4(result, 1.0f);
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir){
    vec3 ambient = light.ambient * material.ambient;

    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);;

    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular); 

    float distance = length(light.position - FragPos);
    float attenuation = 1/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcDirectionLight(DirLight light, vec3 norm, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient = light.ambient * material.ambient;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);;

    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular); 

    return (ambient + diffuse + specular);
}
