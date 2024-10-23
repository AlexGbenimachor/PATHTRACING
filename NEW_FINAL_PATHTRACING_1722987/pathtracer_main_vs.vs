#version 430 core

layout(location  = 0) in vec3 aPos;
layout(location  = 1) in vec3 aNormal;
layout(location  = 2) in vec2 aTexCoord;


out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;
out vec4 FragPosLightSpace;
out vec4 Position;
out float CameraDist;



uniform mat4 lightSpaceMatrix;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 MV;

void main(){

   TexCoord = aTexCoord;
   //Position = MV * aPos;
   //CameraDist = -Position.z;
   FragPos = vec3(model * vec4(aPos, 1.0));
   Normal =  transpose(inverse(mat3(model))) * aNormal; 
   FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
   gl_Position = projection * view * vec4(FragPos, 1.0);
   



}

