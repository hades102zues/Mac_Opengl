#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal; // the model space normal vector of a fragment
layout (location = 2) in vec2 aTexCoord; //texture coordinates tells which part of the texture shall be mapped to the current frag

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal; //world space normal vectors
out vec3 FragPos; //world space frag position
out vec2 TextureCoord; 

void main()
{
    	gl_Position =   projection * view * model * vec4(aPos, 1.0);
    	Normal = mat3(transpose(inverse(model))) * aNormal;
    	FragPos = vec3(model * vec4(aPos, 1.0));
    	TextureCoord = aTexCoord;    
}  
