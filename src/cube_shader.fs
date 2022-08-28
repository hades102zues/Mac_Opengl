#version 330 core
out vec4 FragColor;  

uniform vec3 objectColor; //the amount of light color being reflected
uniform vec3 lightColor;  //color of the light source
uniform vec3 lightPos; //position of the light
uniform vec3 cameraPos; //position of the camera
in vec3 FragPos;
in vec3 Normal;
 
struct Material {
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shinny;
};

uniform Material material;

void main()
{
    vec3 ambient = material.ambient * lightColor; //amount of available light

    vec3 normal = normalize(Normal); //translating to world space denormalizes the normal
    vec3 toLight = normalize(lightPos - FragPos); //vector from frag to light 
    float diff = max(dot(normal, toLight), 0.0);
    vec3 diffuse = lightColor * (diff * material.diffuse);

    vec3 viewDir = normalize(cameraPos - FragPos); //vector from frag to camera
    vec3 reflectRay = reflect(-toLight,Normal); //the reflected light ray that hit the surface
    float shine = pow(max(dot(viewDir, reflectRay), 0.0), material.shinny);
    vec3 specular = lightColor * (shine * material.specular);
    

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);

}
