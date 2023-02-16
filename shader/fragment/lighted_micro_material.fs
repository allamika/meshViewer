#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct MaterialBRDF {
    vec3  albedo;
    float metallic;
    float roughness;
    float ao;
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


const float PI = 3.14159265359;


out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform Material material;
uniform MaterialBRDF materialBRDF;
uniform vec3 viewPos;

#define NB_POINT_LIGHTS 128
uniform PointLight pointLights[NB_POINT_LIGHTS];
uniform int nbPointLight;

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir);
vec3 CalcL0(vec3 L, vec3 V, vec3 H);
float D(vec3 N, vec3 H, float a);
float G(vec3 N, vec3 V, vec3 L, float k);
float Gsub(float NdotV, float k);
vec3 F(float cosTheta, vec3 F0);

void main()
{
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    vec3 Lo = vec3(0.0);
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    for(int i=0; i<NB_POINT_LIGHTS && i<nbPointLight; i ++){
        Lo += CalcPointLight(pointLights[i], Normal, FragPos, viewDir);
    }

    vec3 ambient = vec3(0.03) * materialBRDF.albedo * materialBRDF.ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    result += pow(color, vec3(1.0/2.2)); 

    //for(int i=0; i<NB_DIR_LIGHTS && i<nbDirLight; i ++){
    //    result += CalcDirectionLight(directionLights[i], Normal, viewDir);
    //}
    
    
    
    FragColor = vec4(result, 1.0f);
}

vec3 CalcPointLight(PointLight light, vec3 N, vec3 fragPos, vec3 V){

    vec3 L = normalize(light.position - FragPos);
    vec3 H = normalize(L + V);


    float distance = length(light.position - FragPos);
    float attenuation = 1/(distance * distance);
    vec3 radiance = light.specular;


    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, materialBRDF.albedo, materialBRDF.metallic);
    vec3 F = F(max(dot(H, V), 0.0), F0);

    float D = D(N, H, materialBRDF.roughness);       
    float G = G(N, V, L, materialBRDF.roughness);

    vec3 numerator = D * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator; 

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - materialBRDF.metallic;

    float NdotL = max(dot(N, L), 0.0);        
    vec3 Lo = (kD * materialBRDF.albedo / PI + specular) * radiance * NdotL;
    
    return Lo;
}

//microfacette fonctions

float D(vec3 N, vec3 H, float a){
    float NdotH = max(dot(N, H), 0.0);
	
    float nom = a*a;
    float denom  = (NdotH*NdotH * (a*a - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return nom/denom;
}
  
float G(vec3 N, vec3 V, vec3 L, float k){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float Gsub1 = Gsub(NdotV, k);
    float Gsub2 = Gsub(NdotL, k);
	
    return Gsub1 * Gsub2;
}

float Gsub(float NdotV, float k){
    float nom = NdotV;
    float denom = NdotV * (1.0-k) + k;
	
    return nom/denom;
}

vec3 F(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}