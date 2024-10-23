#ifndef  MYCOMPUTESHADER_H
#define  MYCOMPUTESHADER_H

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <glm/glm.hpp>



using namespace std;
using namespace glm;

void checkCompileErrors(unsigned int shaderObject, string type){
         int result;
         char infoLogLength[1024];
         if(type != "PROGRAM"){
            glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);
            if (!result){
                glGetShaderInfoLog(shaderObject, 1024, NULL, infoLogLength);
               cout<<"ERROR::SHADER_COMPILATION_ERROR of type:"<<type<<endl<<infoLogLength<<endl<<"-----------------"<<endl;

            }

         }else{
            glGetShaderiv(shaderObject, GL_LINK_STATUS, &result);
            if(!result){
                glGetShaderInfoLog(shaderObject, 1024, NULL, infoLogLength);
                cout<<"ERROR::SHADER_LINKING_ERROR of type:"<<type<<endl<<infoLogLength<<endl<<"-----------------"<<endl;
            }

         }

      }





      //create function for reading the file...
      string openComputeShaderSource(const char* shaderFilePath){
             string constr;
             ifstream fsStream(shaderFilePath, ios::in);
             string line="";
            while(!fsStream.eof()){
               getline(fsStream, line);
               constr.append(line+"\n");
            }
         fsStream.close();
         return constr;

       
      }
      unsigned int MyComputeShader(const char* computePath){
             string computerfileStr = openComputeShaderSource(computePath);//read computer shader source file
             const char* computeShaderSource = computerfileStr.c_str();// convert shader file source to constant
             //COMPILE COMPUTE SHADER
             GLuint computeShader;//, fragmentShader;
             computeShader =  glCreateShader(GL_COMPUTE_SHADER);
             glShaderSource(computeShader, 1, &computeShaderSource, NULL);
             glCompileShader(computeShader);
             checkCompileErrors(computeShader, "COMPUTE");
             
             //LINKING THE VERTEX AND FRAGMENT SHADER PROGRAM
             //make shader programme
               
             unsigned int PROGRAMID = glCreateProgram();
             glAttachShader(PROGRAMID,computeShader);
             glLinkProgram(PROGRAMID);
             checkCompileErrors(PROGRAMID, "PROGRAM");

             //detach and delete COMPUTE SHADER PROGRAM
             glDetachShader(PROGRAMID, computeShader);
             glDeleteShader(computeShader);
             
      
             return PROGRAMID;
      }

      #endif

