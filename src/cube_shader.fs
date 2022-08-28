#version 330 core
out vec4 FragColor;  

uniform vec3 objectColor; //the amount of light color being reflected
uniform vec3 lightColor;  //color of the light source
uniform vec3 lightPos; //position of the light
uniform vec3 cameraPos; //position of the camera
in vec3 FragPos;
in vec3 Normal;
  
void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor; //amount of available light

    vec3 normal = normalize(Normal); //translating to world space denormalizes the normal
    vec3 toLight = normalize(lightPos - FragPos); //vector from frag to light
    
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(cameraPos - FragPos); //vector from frag to camera
    vec3 reflectRay = reflect(-toLight,Normal); //the reflected light ray that hit the surface
    
    //naturally the strength of the highlight should be at its peak when the reflected ray is 
    //in the exact direction of the camera
    //such a case is captured when the dot product returns 1 hence both vectors are layered ontop of one another
    float spec = pow(max(dot(viewDir, reflectRay), 0.0), 32);
    

     // idealy want values between 0 and 1. the dot product can return a negative. The value will be negative when the 
     // light source is more that 90 degrees to the frag. In that case, there should be no light reaching
     // the fragment. max() accomplishes this intuition.
    float diffuse = max(dot(normal, toLight), 0.0);

    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(result, 1.0);

}
