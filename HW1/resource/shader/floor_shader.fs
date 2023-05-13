#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;  

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;

void main()
{   
    vec3 Normal = vec3(0, 1, 0);
    vec3 fColor = texture(texture_diffuse1, TexCoords).rgb;

    // ambient
    const float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    const float diffuseStrength = 0.7;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;
    
    // specular
    const float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 128); 
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * fColor;
    FragColor = vec4(result, 1.0);
}