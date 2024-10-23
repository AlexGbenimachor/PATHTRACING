#ifndef MYSHADER_H
#define MYSHADER_H

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




void checkCompilationErrors(unsigned int shaderObject, string type){
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
      string openShaderSource(const char* shaderFilePath){
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
      unsigned int MyShader(const char* vertexfilepath, const char* fragmentfilepath){

         string vertexfileStr = openShaderSource(vertexfilepath);//read vertex source file
         string fragmentfileStr = openShaderSource(fragmentfilepath); //read fragment source file...
         const char* vertexShaderSource = vertexfileStr.c_str();// convert shader file source to constant
         const char* fragmentShaderSource = fragmentfileStr.c_str();//convert shader file to char constant
      
          //create Vertex and Fragment Shader...
         
         //compile vertex shader...
         GLuint vertexShader, fragmentShader;
         vertexShader = glCreateShader(GL_VERTEX_SHADER);
         glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
         glCompileShader(vertexShader);
         checkCompilationErrors(vertexShader, "VERTEX");
         
         //compile fragment shader
         fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
         glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
         glCompileShader(fragmentShader);
         checkCompilationErrors(fragmentShader, "FRAGMENT");



         //LINKING THE VERTEX AND FRAGMENT SHADER PROGRAM
         //make shader programme
       
         unsigned int PROGRAMID = glCreateProgram();
         glAttachShader(PROGRAMID, vertexShader);
         glAttachShader(PROGRAMID, fragmentShader);
         glLinkProgram(PROGRAMID);
         checkCompilationErrors(PROGRAMID, "PROGRAM");

         //detach and delete VERTEX AND FRAGMENT PROGRAM
         glDetachShader(PROGRAMID, vertexShader);
         glDetachShader(PROGRAMID, fragmentShader);
         glDeleteShader(vertexShader);
         glDeleteShader(fragmentShader);

         return PROGRAMID;
      }

      #endif


     




