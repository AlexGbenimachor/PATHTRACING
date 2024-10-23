// STD LIBRARY
#include <math.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <stack>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// OPENGL LIBRARY

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "loadShaderfolder/loadShader.hpp"
#include "loadShaderfolder/myCompute.hpp"


using namespace std;
using namespace glm;


//################################################ALL YOUR CLASSES GOES HERE######################################################

//build Ray class
#define RAYTRACE_RENDER_WIDTH   1000 // also set window width & height to these values
#define RAYTRACE_RENDER_HEIGHT  1000




//float t = INFINITY;
class Ray{
      public:
            Ray(){}
            Ray(const vec3& origin, const vec3& direction){
                this->On = origin;
                this->dir = direction;
            }
            vec3 origin(){return On;}
            vec3 direction(){return dir;}
            
            vec3 P(const float& t){return On + t * dir;}
            
       private:
            vec3 On;
            vec3 dir;

};

class u_object{
      public:
            u_object(){}
            u_object(vector<vec3> position, const vec3& obSize, vector<vec3> Objcolor, const int& objtype){
                   this->Pos = position;//position
                   this->Sz = obSize;//obj size
                   this->type  = objtype;//type
                   this->obColor = Objcolor;//object color
           
           };
           vector<vec3> position(){return Pos;}
           vec3 obSize(){return Sz;}
           int objtype(){return type;}
           vector<vec3> objColors(){return obColor;}
      private:
             vector<vec3> Pos;
             vec3 Sz;
             int type;
             vector<vec3> obColor;







};

//create sphere class here...
class Sphere{

    private:
     int numVertices;
     int numIndices;
     vector<int> indices;
     vector<vec2> texCoords;
     vector<vec3> vertices;
     vector<vec3> normals;
     //void init(int);
     //float toRadians(float degrees);

    public:
      

      Sphere(){//declare a default value Sphere precision
        init(48);
      }
      Sphere(int prec){
        init(prec);
      };
      
      int getNumVertices();
      int getNumIndices();
      vector<int> getIndices();
      vector<vec3> getVertices();
      vector<vec2> getTexCoords();
      vector<vec3> getNormals();

      float toRadians(int degrees){return (degrees * 2.0f * 3.14159f) / 360.0f;}

      void init(int prec){
           numVertices = (prec+1)*(prec+1);//initialize the number of vertices required to render Sphere
           numIndices  = prec * prec * 6;  //initialization of the number of indices to render the triangle needed to render sphere..
           for(int i=0; i<numVertices; i++){vertices.push_back(vec3());};
           for(int i=0; i<numVertices; i++){texCoords.push_back(vec2());};
           for(int i=0; i<numVertices; i++){normals.push_back(vec3());};
           for(int i=0; i<numIndices; i++){indices.push_back(0);};

           //calculate the number triangles per vertices...
           for(int i=0; i<=prec; i++){
            for(int j=0; j<=prec; j++){
               float y = (float)cos(toRadians(180.0f - i * 180.0f / prec));
               float x =  -(float)cos(toRadians(j*360.0f / prec)) * (float)abs(cos(asin(y)));
               float z = (float)sin(toRadians(j*360.0f / prec)) * (float)abs(cos(asin(y)));
               vertices[i*(prec + 1) + j] = glm::vec3(x, y, z);
			   texCoords[i*(prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / prec));
			   normals[i*(prec + 1) + j] = glm::vec3(x,y,z);

            }
           }
           //compute the indicies of the each triangle
           for (int i = 0; i<prec; i++) {
               for (int j = 0; j<prec; j++) {
                indices[6 * (i*prec + j) + 0] = i*(prec + 1) + j;
                indices[6 * (i*prec + j) + 1] = i*(prec + 1) + j + 1;
                indices[6 * (i*prec + j) + 2] = (i + 1)*(prec + 1) + j;
                indices[6 * (i*prec + j) + 3] = i*(prec + 1) + j + 1;
                indices[6 * (i*prec + j) + 4] = (i + 1)*(prec + 1) + j + 1;
                indices[6 * (i*prec + j) + 5] = (i + 1)*(prec + 1) + j;
             }
            }


           

        }//void()

        


};

//define the accessors of the Sphere...
int Sphere::getNumVertices() { return numVertices; }
int Sphere::getNumIndices() { return numIndices; }
vector<int> Sphere::getIndices() { return indices; }
vector<vec3> Sphere::getVertices() { return vertices; }
vector<vec2> Sphere::getTexCoords() { return texCoords; }
vector<vec3> Sphere::getNormals() { return normals; }


// declare your classes here
class triangleFaces
{

public:
    vector<vec3> triangleVertex; // triangle vertices
    vector<vec3> colorVertex;    // color vertices
    vector<vec2> textCoord;
    vec3 objPos; // triangle position

    triangleFaces(){}; // empty constructor...

    triangleFaces(vector<vec3> triangleVertex, vector<vec3> colorVertex, vec3 objPos, vector<vec2> textCoord);
};

triangleFaces::triangleFaces(vector<vec3> triangleVertexs, vector<vec3> colors, vec3 objpos, vector<vec2> texTCoord)
{
    this->triangleVertex = triangleVertexs;
    this->colorVertex = colors;
    this->objPos = objpos;
    this->textCoord = texTCoord;
}
class rectangle
{

public:
    vector<vec3> vertexPos;
    vector<vec3> colorPos;
    vector<vec2> texturePos;
    rectangle(){}; // empty constructor
    rectangle(vector<vec3> vertexPos, vector<vec3> colorPos, vector<vec2> texturePos);
};

rectangle::rectangle(vector<vec3> vertexpos, vector<vec3> colorpos, vector<vec2> texturePos)
{
    this->vertexPos = vertexpos;
    this->colorPos = colorpos;
    this->texturePos=texturePos;
}



//############################################################ALL WINDOWS SETTING GOES HERE#####################################################
int width = 1366;//800;////1366;//1366;//
int height= 768;//600;;//768; //768;//

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

// TIMING

float deltaTime = 0.0f;
float lastFrame = 0.0f;
// lighting properties...
//1. lightPos
glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);//(1.2f, 1.0f, 2.0f);//(-0.2f, -1.0f, -0.3f);
vec3 lightPos(1.2f, 1.0f, 2.0f);//
vec3 lightPos2(-1.5f, 1.0f, 2.0f);
//2. lightPos
vec3 lightPos3(-8.0, 10.0, 0.0);
vec3 objColor(0.62f, 0.31f, 0.1f);
vec3 objColor2(0.71f, 0.40f, 0.11f);
vec3 lightColor( 1.0f, 1.0f, 1.0f);

vec3 viewPos(-1.5f, 1.5f, 4.0);


//camera properties

vec3 cameraPosition = camera.Position;
vec3 cameraDirection = camera.Front;

//defined vertex array object and buffer object...
int flect_FBO, ReflectTexture, depth_fbo, DepthMap;
unsigned int BaseVAO, BaseVBO, cubeVAO, cubeVBO, PlaneVAO, PlaneVBO, sphereVAO, sphereVBO, depthRenderBuffer,CubeMapShadowMap;
unsigned int QuadVAO, QuadVBO, SkyBoxVAO, SkyBoxVBO, depthMapFBO,  depthMap, reflectMap, reflectVBO,FBOCubeMapShadow, PYRVAO, PYRVBO, ConeVAO, ConeVBO,ConeEBO;//, rbo, framebuffer;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
GLuint tileTexture, cubeTexture, sphereTexture, tennisTexture,  tennisTexture2, textureColorbuffer, specularTexture;
//#################################################CALLBACK METHODS GOES HERE######################################################
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLuint loadcubemapimages(vector<string> faces);
GLuint loadTextures2Object(char const *imPath);

string sphereColorStr;
void ObjInput(int ProgramID, u_object obj);
void renderScene(int ProgramID);
void renderSphere(int ProgramID);
void renderCube(int ProgramID);
void renderQuad(int ProgramID);
void renderQuad2(int ProgramID);
void PlaneObject(int programID);
void renderCone(int ProgramID);
void renderSkyBox();
GLuint  depthFBO();
void renderPathtracerScene(int ProgramID);
vec3 computeNormal(vec3 a, vec3 b, vec3 c);
void renderCone(float ConHeight, int programID);
void renderPyramind(int programID);

GLuint renderRenderCubeFBO();
unsigned int DynamicCubemap();
void renderObject(int programID);
double random_double(double min, double max);
//void ObjectProperty(int ProgramID, u_object Xobject);
//void renderScene(int ProgramID, u_object BxObject, u_object spObject);//render Scene
void renderScene(int ProgramID);//render Scene
//void pointLightMaterial(int ProgramID, vector<vec3> pointLightVec, u_object pointObject);//render point lights
//void MatProperty(int ProgramID,vector<vec3> objectAmb, vector<vec3> objectDiff, vector<vec3> objSpec, vector<float> shininess);
void renderFramebufferTextureID(int ProgramID, int FBO, int TextureID, u_object bxObject, u_object spObject);
float intersectSphere(glm::vec3 position, float radius, Ray r);
float intersectBox(glm::vec3 position, glm::vec3 boxSize, Ray r);
//#####################################################NOTHING GOES HERE##########################################################
vector<vec3> pointlights,  cubePosition,  spherePosition;
u_object SphereObject, BoxObject, lightObject;
GLuint TexCubeMapID, FBOID, RBOID;
const int CUBEMAP_SIZE = 1024;

//define the workgroup size...
int workGroupsX = RAYTRACE_RENDER_WIDTH;
int workGroupsY = RAYTRACE_RENDER_HEIGHT;
int workGroupsZ = 1;

const unsigned int TEXTURE_WIDTH = 1000, TEXTURE_HEIGHT = 1000;

float twirl_amount = 0;

int main(){
     cout<<"+PARSING MONTE CARLO PATH-TRACING BY PROPERTIES FROM C++ TO SHADER+"<<endl;
    
    if (!glfwInit()){
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create windows
    GLFWwindow *window; // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(width, height, "+PARSING MONTE CARLO PATH-TRACING BY PROPERTIES FROM C++ TO SHADER+", NULL, NULL);

    if (window == NULL){
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }

    // create context window
    glfwMakeContextCurrent(window);
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    

    }

     /*  Callback function  */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwGetFramebufferSize(window, &width, &height);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    

    glEnable(GL_DEPTH_TEST);
    
    
    // query limitations
	// -----------------
    int max_compute_work_group_count[3];
    int max_compute_work_group_size[3];
    int max_compute_work_group_invocations;

    for (int idx = 0; idx < 3; idx++) {
	      glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
	      glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
        }	
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);








    std::cout << "OpenGL Limitations: " << std::endl;
    std::cout << "maximum number of work groups in X dimension " << max_compute_work_group_count[0] << std::endl;
    std::cout << "maximum number of work groups in Y dimension " << max_compute_work_group_count[1] << std::endl;
    std::cout << "maximum number of work groups in Z dimension " << max_compute_work_group_count[2] << std::endl;

    std::cout << "maximum size of a work group in X dimension " << max_compute_work_group_size[0] << std::endl;
    std::cout << "maximum size of a work group in Y dimension " << max_compute_work_group_size[1] << std::endl;
    std::cout << "maximum size of a work group in Z dimension " << max_compute_work_group_size[2] << std::endl;

    std::cout << "Number of invocations in a single local work group that may be dispatched to a compute shader " <<       max_compute_work_group_invocations << std::endl;
    
    //this part of code we call our supplementary for rendering vertex and fragments(shader files)
    //Myshader files...
     unsigned int MainProgramID    = MyShader("pathtracer_main_vs.vs","pathtracer_main_fs.fs");
     unsigned int LightProgramID    = MyShader("pathtracer_light_vs.vs","pathtracer_light_fs.fs");
    
    
    
    
    vector<string> faces;
    faces.push_back("./Skybox/1.jpg");//right
    faces.push_back("./Skybox/2.jpg");//left
    faces.push_back("./Skybox/3.jpg");//top
    faces.push_back("./Skybox/4.jpg");//bottom
    faces.push_back("./Skybox/5.jpg");//front
    faces.push_back("./Skybox/6.jpg");//back

   
   

    cout<<"size of faces vector:"<<endl<< faces.size()<<endl;
    
    
//SQUARE OBJECT    
rectangle halfTriangle(vector<vec3>{//coordinates
                                      vec3(0.5, 0.5, 0.0), 
                                      vec3(0.5, -0.5, 0.0), 
                                      vec3(-0.5,  0.5, 0.0), 
                                      vec3(0.5,  -0.5, 0.0), 
                                      vec3(-0.5,  -0.5, 0.0), 
                                      vec3(-0.5,  0.5, 0.0)}, 
                                      //Normal
                         vector<vec3>{vec3(0.0, 1.0, 0.0), 
                                      vec3(1.0, 1.0, -1.0), 
                                      vec3(1.0, 1.0, 1.0),
                                      vec3(0.0, 1.0, 0.0), 
                                      vec3(-1.0, 1.0, 1.0), 
                                      vec3(-1.0, 1.0, -1.0) }, 
                                    //Textcoord
                        vector<vec2>{vec2( 1.0f, 1.0f), //1
                                    vec2( 1.0f, 0.0f), //2
                                    vec2(0.0f, 1.0f),//3
                                    vec2( 1.0f, 0.0f),
                                    vec2( 0.0f, 0.0f),
                                    vec2(0.0f, 1.0f)});                             

   vector<vec3> recVertices;
   vector<vec3> recColorVert;
   vector<vec2> recTexCoord;
   for (unsigned int ix=0; ix<halfTriangle.vertexPos.size(); ix++){
         recVertices.push_back(halfTriangle.vertexPos[ix]);
         recColorVert.push_back(halfTriangle.colorPos[ix]);
         recTexCoord.push_back(halfTriangle.texturePos[ix]);

   }
    //base buffer VAO, VBO;
    glGenBuffers(1, &BaseVBO);
    glGenVertexArrays(1, &BaseVAO);
    //light buffer VAO, VBO
    
   //bind Base vertex to BUFFER
   glBindVertexArray(BaseVAO);
   glGenBuffers(1, &BaseVBO);
   glBindBuffer(GL_ARRAY_BUFFER, BaseVBO);
   glBufferData(GL_ARRAY_BUFFER, recVertices.size() * sizeof(vec3), &recVertices[0], GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(0);

   //bind Base vertex to Normal
  
   glBindVertexArray(BaseVAO);
   glGenBuffers(1, &BaseVBO);
   glBindBuffer(GL_ARRAY_BUFFER, BaseVBO);
   glBufferData(GL_ARRAY_BUFFER,   recColorVert.size()*sizeof(vec3), &recColorVert[0], GL_STATIC_DRAW);   
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(1);

   //Base TexCoord
   glBindVertexArray(BaseVAO);
   glGenBuffers(1, &BaseVBO);
   glBindBuffer(GL_ARRAY_BUFFER, BaseVBO);
   glBufferData(GL_ARRAY_BUFFER,   recTexCoord.size()*sizeof(vec2), &recTexCoord[0], GL_STATIC_DRAW);   
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glEnableVertexAttribArray(2);
   
   
    

    
    
    
     //cube map loader...
    GLuint skyboxMap =  loadcubemapimages(faces);

    //2d texture map loader...
    string LennaPath ="Lenna.png";
    string cubePath ="1000_F_199149981_RG8gciij11WKAQ5nKi35Xx0ovesLCRaU.jpg";//"container2.png";//
    string ballPath="TennisBall.jpg";
    string ballPath2="ball2.jpg";
    string specularPath ="container2_specular.png";
    GLuint lennaTexture = loadTextures2Object(LennaPath.c_str());
    tileTexture= loadTextures2Object(faces[0].c_str());//FLOOR 
    cubeTexture = loadTextures2Object(cubePath.c_str()); //cube texture...
    tennisTexture = loadTextures2Object(ballPath.c_str());
    tennisTexture2 = loadTextures2Object(ballPath2.c_str());
    specularTexture =  loadTextures2Object(specularPath.c_str());
    
    
    //Initialization framebuffer object
    flect_FBO, ReflectTexture =  renderRenderCubeFBO();
    FBOID, TexCubeMapID = DynamicCubemap();
    depth_fbo, DepthMap = depthFBO();

   
    
    
    
    //define point light position...
    
     for(int i = 0; i<6; i++){

        
        //vec3 posT = vec3( -2.5*i, 4.0*i , -4.0*i );
        vec3 posT = vec3( 3.0*i, 2.5+i , 4.5*i );
        pointlights.push_back(posT);

     }
     cout<<"pointlight position length: "<< pointlights.size()<<endl;
     
     
     
    //cubes position ...
   
    for(int i = 0; i<10; i++){
    
    
        vec3 cubePos = vec3((i*-2.0f ), 4.0*i , 0.3*i);
        cubePosition.push_back(cubePos);
    
    
    }
    cout<<"cube position length: "<<cubePosition.size()<<endl;
    
    
    
    //sphere position 
    
     
    for(int i = 0; i<10; i++){
    
    
        vec3 spherePos = vec3((i*2.0), 0.5*i, 0.3*i);
        spherePosition.push_back(spherePos);
    
    
    }
    cout<<"sphere position length: "<<spherePosition.size()<<endl;
    
    
    vector<vec3> objColor ={vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0), vec3(0.0, 1.0, 1.0),    
    vec3(1.0, 0.0, 1.0), vec3(1.0), 
    vec3(0.4, 0.4, 0.4), vec3(0.5, 1.0, 0.5), 
    vec3(1.0, 0.5, 0.5)};    
    
    cout<<"object colors: "<<objColor.size()<<endl;
    
    //Ambient...
    vector<vec3> ambientVec = {vec3(0.0215,0.1745,0.0215), vec3(0.135,0.2225, 0.1575), vec3(0.05375,0.05, 0.06625), vec3(0.1745,0.01175,0.01175), vec3(0.1,0.18725,0.1745),vec3(0.329412,0.223529,0.027451), vec3(0.2125,0.1275,0.054), vec3(0.25,0.25,0.25), vec3(0.19125,0.0735,0.0225), vec3(0.24725,0.1995,0.0745)
    };
    cout<<"Ambient light properties: "<<ambientVec.size()<<endl;
     //Diffuse...
     vector<vec3> DiffuseVec = {vec3(0.07568,0.61424,0.07568), vec3(0.54,0.89,0.63), vec3(0.18275,0.17,0.22525), vec3(1,	0.829,	0.829), vec3(0.61424,0.04136,0.04136),vec3(0.396,0.74151, 0.69102), vec3(0.780392,0.568627,0.113725), vec3(0.714,	0.4284,0.18144), vec3(0.4,0.4,0.4), vec3(0.7038,0.27048,0.0828)
    };
    cout<<"Diffuse light properties: "<<DiffuseVec.size()<<"\ncheck length: 0 "<<DiffuseVec[0].z<<endl;
    //Specular...
    vector<vec3> SpecuLarVec = {vec3(0.633,0.727811,0.633), vec3(0.316228,0.316228,0.316228), vec3(0.332741,0.328634,0.346435), vec3(0.296648,0.296648,0.296648), vec3(0.727811,0.626959,0.626959),vec3(0.297254,0.30829,0.306678), vec3(0.297254,0.30829,	0.306678), vec3(0.393548,0.271906, 0.166721), vec3(0.774597,0.774597,0.774597), vec3(0.256777, 0.137622,0.086014)
    };
    cout<<"Specular light properties: "<<SpecuLarVec.size()<<endl;
    //------------------------------------------------------------
    vector<float> objectShininess = {0.6, 0.1, 0.3, 0.088, 0.6, 0.1, 0.21794872,0.2, 0.6, 256.0 };
    cout<<"length of object shininess: "<<objectShininess.size()<<endl;
   

    
    
   //ads properties...
   vec3  ambientLight  = vec3( 0.05f, 0.05f, 0.05f);
   vec3  specularLight = vec3(1.0f, 1.0f, 1.0f);
   vec3  DiffuseLight  = vec3(0.8f, 0.8f, 0.8f);
   float constant  = 1.0f;
   float linear = 0.09f;
   float quadratic = 0.032f;
   
   //directlight properties...
    vec3 dirLightDirection =vec3(-0.2f, -1.0f, -0.3f);
    vec3 dirLightAmnbient = vec3(0.05f, 0.05f, 0.05f);
    vec3 dirLightDiffuse = vec3( 0.4f, 0.4f, 0.4f);
    vec3 dirLightSpecular = vec3( 0.5f, 0.5f, 0.5f);
   
   
   
      int fCounter = 0;
      
    
      
      
      vec3 sphereScale = vec3(0.5);
      mat4 modelSphere = mat4(1.0);
      int SphObjType = 1;
      //==========================
      vec3 BoxColor = vec3(1.0, 1.0, 0.0);
      vec3 BoxScale = vec3(0.5);
      mat4 modelBox = mat4(1.0);
      int  BoxObjType = 2;
      //============================
      vec3 lightScale = vec3(0.4);
      int lightType = 3;
      
      SphereObject   = u_object(spherePosition ,sphereScale, objColor, SphObjType );
      BoxObject   = u_object(cubePosition ,BoxScale,  objColor, BoxObjType );
      lightObject   = u_object(pointlights ,lightScale, objColor, lightType);
     
      cout<<"("<<SphereObject.position()[5].x<<","<<SphereObject.position()[5].y<<","<<SphereObject.position()[5].z<<")"<<endl;
      cout<<"("<<BoxObject.position()[9].x<<","<<BoxObject.position()[9].y<<","<<BoxObject.position()[9].z<<")"<<endl;
      cout<<"("<<SphereObject.objColors()[5].x<<","<<SphereObject.objColors()[5].y<<","<<SphereObject.objColors()[5].z<<")"<<endl;//color...
      
       do{
              float currentFrame = static_cast<float>(glfwGetTime());
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;
                
                
                
                 processInput(window);
                
                // Render to our framebuffer
                //unsigned int framebuffer;
		float globalTime = static_cast<float>(glfwGetTime() * 0.001);

                // render
                // ------
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                double random =  random_double(-1.0, 1.0);
                vec3 cameraPosition = camera.Position;
                vec3 cameraDirection = camera.Front;
                float aspectRatio =(float) width/height;
                float fov = (float)radians(camera.Zoom);
                vec2 Resolution = vec2((float)width, (float)height);
                glm::mat4 model = glm::mat4(1.0f);
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
                glm::mat4 view = camera.GetViewMatrix();
                
                //===========SHADOW FRAMEBUFFER=====//
              
               //============SCENE ============//
                glUseProgram(MainProgramID);
                
                vec3 lightColor =  vec3(1.0);
                lightPos =  vec3(0.0f, 19.5f, 0.0);
                vec3 lightAmbient =  vec3(0.2f, 0.2f, 0.2f);
                vec3 lightdiffuse =  vec3( 0.5f, 0.5f, 0.5f);
                vec3 lightspecular = vec3(1.0f, 1.0f, 1.0f);
                
                float Light_linear = 0.09f;
                float lightConstant =  1.0f;
                float lightQuadratic = 0.032f; 
                
                glUniform1f(glGetUniformLocation(MainProgramID, "random"), random);
                glUniform1f(glGetUniformLocation(MainProgramID, "fov"), fov);
                glUniform2f(glGetUniformLocation(MainProgramID, "Iresolution"), Resolution.x, Resolution.y);
                //light properties
                glUniform3f(glGetUniformLocation(MainProgramID,  "light.ambient"),  lightAmbient.x, lightAmbient.y,lightAmbient.z);
                glUniform3f(glGetUniformLocation(MainProgramID,  "light.specular"),  lightspecular.x, lightspecular.y, lightspecular.z);
                glUniform3f(glGetUniformLocation(MainProgramID,  "light.diffuse"),  lightdiffuse.x,lightdiffuse.y, lightdiffuse.z);
                
                //light constant, light linear, light quadratic...
                 glUniform1f(glGetUniformLocation(MainProgramID, "light.linear"), Light_linear);
                 glUniform1f(glGetUniformLocation(MainProgramID, "light.constant"),  lightConstant);
                 glUniform1f(glGetUniformLocation(MainProgramID, "light.quadratic"), lightQuadratic);
                
                glUniform3f(glGetUniformLocation(MainProgramID,  "cameraPos"),  cameraPosition.x,  cameraPosition.y, cameraPosition.z);
                glUniform3f(glGetUniformLocation(MainProgramID,  "lightColor"),  lightColor.x, lightColor.y, lightColor.z);
                glUniform3f(glGetUniformLocation(MainProgramID,  "lightPos"),  lightPos.x,lightPos.y, lightPos.z);
                glUniform3f(glGetUniformLocation(MainProgramID,  "viewPos"),  cameraPosition.x,cameraPosition.y, cameraPosition.z);
                renderPathtracerScene(MainProgramID);
                
                
                //==============TOP LIGHT=========================================//
                
                glUseProgram(LightProgramID);
                //top light
                mat4 topmodelLight = glm::mat4(1.0f);
                //topmodelLight = scale( topmodelLight, vec3(1.0f));
                topmodelLight = glm::translate( topmodelLight, glm::vec3(0.0f, 19.5f, 0.0));
                topmodelLight = rotate( topmodelLight, radians(90.0f), vec3(0.0, 0.0, 1.0));
                topmodelLight = rotate( topmodelLight, radians(90.0f), vec3(0.0, 1.0, 0.0));
                topmodelLight = scale( topmodelLight, vec3(1.0f));
                vec3  topSideColor = vec3(1.0f);
                
                //glUseProgram(ProgramID);
                glUniform3f(glGetUniformLocation(LightProgramID,  "color"),  topSideColor.x,topSideColor.y, topSideColor.z);
                glUniformMatrix4fv(glGetUniformLocation(LightProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                glUniformMatrix4fv(glGetUniformLocation(LightProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(LightProgramID, "model"), 1, GL_FALSE, glm::value_ptr(topmodelLight));
                glBindVertexArray(BaseVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);


                
                
                
                
                
                
                
                glBindVertexArray(0);
                glfwSwapBuffers(window);
                glfwPollEvents();


                }while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
               //clear Array Buffers
                 glDeleteBuffers(1, &BaseVBO);
                 glDeleteBuffers(1, &PlaneVBO);
                 glDeleteBuffers(1, &sphereVBO);
                 glDeleteBuffers(1,&SkyBoxVBO);
                 glDeleteBuffers(1, &depthMapFBO);
                 glDeleteBuffers(1, &QuadVBO);
                 //PYRVAO, PYRVBO 
                 glDeleteBuffers(1, &PYRVBO);
                 glDeleteBuffers(1, &ConeVBO);
                 glDeleteBuffers(1, &ConeEBO);
                 //clear vertex buffers
                 glDeleteVertexArrays(1, &BaseVAO);
                 glDeleteVertexArrays(1,&sphereVAO);
                 glDeleteVertexArrays(1, &PlaneVAO);
                 glDeleteVertexArrays(1, &SkyBoxVAO);
                 glDeleteVertexArrays(1, &depthMap);
                 glDeleteVertexArrays(1, &QuadVAO);
                 glDeleteVertexArrays(1, &PYRVAO);
                 glDeleteVertexArrays(1, &ConeVAO);
                 
                 
                 //glDeleteRenderbuffers(1, &rbo);
                 //glDeleteFramebuffers(1, &framebuffer);

                glfwTerminate();
                return 0;
          



    
    


}


//plane object
void PlaneObject(int programID){
     vector<vec3> PlaneVertices;
     vector<vec3> PlaneColorVertices;
     vector<vec2> PtextureCoord;
     rectangle planeFaces(vector<vec3>{vec3(25.0f, -0.5f,  25.0f),//plane vertex position...
                                       vec3(-25.0f, -0.5f,  25.0f),
                                       vec3(-25.0f, -0.5f, -25.0f),
                                       vec3( 25.0f, -0.5f,  25.0f),
                                       vec3(-25.0f, -0.5f, -25.0f),
                                       vec3( 25.0f, -0.5f, -25.0f)}, 
                          vector<vec3>{vec3(0.0f, 1.0f, 0.0f),//Normal...
                                       vec3(0.0f, 1.0f, 0.0f),
                                       vec3(0.0f, 1.0f, 0.0f),
                                       vec3(0.0f, 1.0f, 0.0f),
                                       vec3(0.0f, 1.0f, 0.0f), 
                                       vec3( 0.0f, 1.0f, 0.0f)}, 
                          vector<vec2>{vec2(25.0f,  0.0f),//Texture Coordinate..
                                       vec2(0.0f, 0.0f), 
                                       vec2( 0.0f, 25.0f),
                                       vec2(25.0f,  0.0f),
                                       vec2(0.0f,  25.0f),
                                       vec2(25.0f, 25.0f)}); 
                                       
     //plane properties
    for(int ix = 0; ix<planeFaces.vertexPos.size(); ix++){
        PlaneVertices.push_back(planeFaces.vertexPos[ix]);
        PlaneColorVertices.push_back(planeFaces.colorPos[ix]);
        PtextureCoord.push_back(planeFaces.texturePos[ix]);
    }
    
       //plane buffer VAO, VBO;
        glGenBuffers(1, &PlaneVBO);
        glGenVertexArrays(1, &PlaneVAO);
        //plane vertex
        //plane  vertex position to BUFFER
        glBindVertexArray(PlaneVAO);
        glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
        glBufferData(GL_ARRAY_BUFFER, PlaneVertices.size() * sizeof(vec3), & PlaneVertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        
        
        //bind plane vertex to Normal

        glBindVertexArray(PlaneVAO);
        glGenBuffers(1, &PlaneVBO);
        glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
        glBufferData(GL_ARRAY_BUFFER,  PlaneColorVertices.size()*sizeof(vec3), &PlaneColorVertices[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);

        
        
        
        
        //Plane texCoord
        glBindVertexArray(PlaneVAO);
        glGenBuffers(1, &PlaneVBO);
        glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
        glBufferData(GL_ARRAY_BUFFER,   PtextureCoord.size()*sizeof(vec2), &PtextureCoord[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
        
        
        
        
        glBindVertexArray(PlaneVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);





}

//CUBE OBJECT
void renderCube(int programID){

    
            //cube vertex
    vector<vec3> cubeVertices;
    vector<vec3> cubeColorVertices;
    vector<vec2> textureCoord;
    rectangle cubeFaces(vector<vec3>{//TOP SIDE 1
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(1.0f,  1.0f, -1.0f), 
                                        vec3(1.0f, -1.0f, -1.0f),
                                        vec3(1.0f,  1.0f, -1.0f),
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(-1.0f,  1.0f, -1.0f),

                                        //RIGHT SIDE 2
                                        vec3(-1.0f, -1.0f,  1.0f),
                                        vec3(1.0f, -1.0f,  1.0f), 
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f, -1.0f,  1.0f),

                                        //BOTTOM SIDE 3
                                        vec3(-1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f, -1.0f), 
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(-1.0f, -1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f,  1.0f),

                                        //LEFT SIDE 4
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(1.0f, -1.0f, -1.0f), 
                                        vec3(1.0f,  1.0f, -1.0f),
                                        vec3(1.0f, -1.0f, -1.0f),
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(1.0f, -1.0f,  1.0f),

                                        //FRONT SIDE 5
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(1.0f, -1.0f, -1.0f), 
                                        vec3(1.0f, -1.0f,  1.0f),
                                        vec3(1.0f, -1.0f,  1.0f),
                                        vec3(-1.0f, -1.0f,  1.0f),
                                        vec3(-1.0f, -1.0f, -1.0f),

                                        //BACK SIDE 6
                                        vec3(-1.0f,  1.0f, -1.0f),
                                        vec3(1.0f,  1.0f , 1.0f), 
                                        vec3(1.0f,  1.0f, -1.0f),
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f, -1.0f),
                                        vec3(-1.0f,  1.0f,  1.0f)
    }, vector<vec3>{
            //NORMAL 01 / COLOR 01

            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            //NORMAL 02 / COLOR 02
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            //NORMAL 03 / COLOR 03
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            //NORMAL 04 / COLOR 04
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            //NORMAL 05/COLOR 05

            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            //NORMAL 06/COLOR 06
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f)
    }, vector<vec2>{
                //texture 01
                vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    //texture 02

                    vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    //texture 03

                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    //texture 04

                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    //texture 05

                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    //texture 06

                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f)
                        
            });
    //cube properties
    for(int ix = 0; ix<cubeFaces.vertexPos.size(); ix++){
        cubeVertices.push_back(cubeFaces.vertexPos[ix]);
        cubeColorVertices.push_back(cubeFaces.colorPos[ix]);
        textureCoord.push_back(cubeFaces.texturePos[ix]);
    }

    //cube VAO, VBO
//cube buffer VAO, VBO;
glGenBuffers(1, &cubeVBO);
glGenVertexArrays(1, &cubeVAO);
//cube vertex
//bind cube vertex to BUFFER
glBindVertexArray(cubeVAO);
glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(vec3), & cubeVertices[0], GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(0);


//bind cube vertex to Normal

glBindVertexArray(cubeVAO);
glGenBuffers(1, &cubeVBO);
glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
glBufferData(GL_ARRAY_BUFFER,  cubeColorVertices.size()*sizeof(vec3), &cubeColorVertices[0], GL_STATIC_DRAW);   
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(1);

//cube texCoord
glBindVertexArray(cubeVAO);
glGenBuffers(1, &cubeVBO);
glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
glBufferData(GL_ARRAY_BUFFER,   textureCoord.size()*sizeof(vec2), &textureCoord[0], GL_STATIC_DRAW);   
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(2);




glBindVertexArray(cubeVAO);
glDrawArrays(GL_TRIANGLES, 0, 36);
glBindVertexArray(0);



}

//SPHERE OBJECT
Sphere mySphere(48);
void renderSphere(int programID){
     
     
    //sphere VAO and VBO 
   //let's create a sphere 
    
        vector<int> indx = mySphere.getIndices();
        vector<vec3> vert = mySphere.getVertices();
        vector<vec2> tex = mySphere.getTexCoords();
        vector<vec3> norm = mySphere.getNormals();

        vector<float> pvalues;// vertex positions		
        vector<float> tvalues;// texture coordinates		
        vector<float> nvalues;// normal vectors		

        int numIndices = mySphere.getNumIndices();
        for (int i = 0; i<numIndices; i++ ){
            //vertex position
            pvalues.push_back((vert[indx[i]]).x);
            pvalues.push_back((vert[indx[i]]).y);
            pvalues.push_back((vert[indx[i]]).z);

            //texture position
            tvalues.push_back((tex[indx[i]]).s);
            tvalues.push_back((tex[indx[i]]).t);

            //normal vectors
            nvalues.push_back((norm[indx[i]]).x);
            nvalues.push_back((norm[indx[i]]).y);
            nvalues.push_back((norm[indx[i]]).z);
        }


    
        glGenVertexArrays(1, &sphereVAO);
        glGenBuffers(1, &sphereVBO);

        //Sphere position...
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0,4, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        
        //Texture vertex position buffer
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER,  tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);


        //Normal vertex position buffer... based on the last failed codes, because the VBO(vertex buffer object) wasn't well initialized and binded to the array vertex
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, nvalues.size()*4, &nvalues[0], GL_STATIC_DRAW);   
        //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);


      



    glBindVertexArray(sphereVAO);
    glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
    glBindVertexArray(0);

    



}

 //Pyramid  vertices..
void renderPyramind(int programID){
     
   // pyramid vertex
    triangleFaces triangleFace1(vector<vec3>{
                                    // FRONT FACE
                                    vec3(-0.5, -0.5, -0.5), // left
                                    vec3(0.5, -0.5, -0.5),  // right
                                    vec3(0.0, 0.5, 0.0),    // top
                                    // RIGHT FACE
                                    vec3(-0.5, -0.5, 0.5), // LEFT
                                    vec3(0.5, -0.5, 0.5),  // RIGHT
                                    vec3(0.0, 0.5, 0.0),   // TOP
                                    // BACK FACE
                                    vec3(-0.5, -0.5, -0.5), // LEFT
                                    vec3(-0.5, -0.5, 0.5),  // RIGHT
                                    vec3(0.0, 0.5, 0.0),    // TOP
                                    // LEFT FACE
                                    vec3(0.5, -0.5, -0.5), // LEFT
                                    vec3(0.5, -0.5, 0.5),  // RIGHT
                                    vec3(0.0, 0.5, 0.0),
                                    // BASE LEFT
                                    vec3(-0.5f, -0.5f, -0.5f),
                                    vec3(0.5f, -0.5f, 0.5f),
                                    vec3(-0.5f, -0.5f, 0.5f),
                                    // BASE RIGHT
                                    vec3(0.5f, -0.5f, 0.5f),
                                    vec3(-0.5f, -0.5f, -0.5f),
                                    vec3(0.5f, -0.5f, -0.5f)

                                }, // top

                                // color
                                vector<vec3>{
                                    // FRONT FACE COLOR//NORMAL
                                    vec3(0.0, 0.0, -1.0), // blue
                                    vec3(0.0, 0.0, -1.0), // green
                                    vec3(0.0, 0.0, -1.0), // red
                                    // RIGHT FACE COLOR
                                    vec3(0.0, 0.0, 1.0), // blue
                                    vec3(0.0, 0.0, 1.0), // green
                                    vec3(0.0, 0.0, 1.0), // red
                                    // BACK FACE COLOR
                                    vec3(-1.0, 0.0, 0.0), // blue
                                    vec3(-1.0, 0.0, 0.0), // green
                                    vec3(-1.0, 0.0, 0.0), // red
                                    // LEFT FACE COLOR
                                    vec3(1.0, 0.0, 0.0), // blue
                                    vec3(1.0, 0.0, 0.0), // green
                                    vec3(1.0, 0.0, 0.0), // red

                                    vec3(0.0, -1.0, 0.0), // blue
                                    vec3(0.0, -1.0, 0.0), // green
                                    vec3(0.0, -1.0, 0.0), // red

                                    vec3(1.0, 0.0, 0.0), // blue
                                    vec3(1.0, 0.0, 0.0), // green
                                    vec3(1.0, 0.0, 0.0), // red

                                },
                                vec3(-0.5, 0.0, 0.0), // triangle default position
                                vector<vec2>{         // the front face texture
                                             vec2(0.0, 0.0), 
                                             vec2(1.0, 0.0f), 
                                             vec2(0.5f, 1.0),
                                             // right face...
                                             vec2(0.0f, 0.0f), 
                                             vec2(1.0f, 0.0f), 
                                             vec2(0.5f, 1.0f),
                                             // Back face texture
                                             vec2(0.0f, 1.0f),
                                             vec2(0.0f, 0.0f), 
                                             vec2(0.5f, 1.0f),

                                             vec2(0.0f, 1.0f), 
                                             vec2(0.0f, 0.0f), 
                                             vec2(0.5f, 1.0f),

                                             vec2(1.0f, 1.0f), 
                                             vec2(1.0f, 0.0f), 
                                             vec2(0.5f, 1.0f),

                                             vec2(0.0f, 0.0f), 
                                             vec2(0.0f, 1.0f), 
                                             vec2(0.5f, 1.0f)

                                });

    vector<vec3> pyravertices;      // vertex
    vector<vec3> pyraColorVertices; // colors
    vector<vec2> pyraTextureCoords; // textures

    // pyramid properties
    for (unsigned int i = 0; i < triangleFace1.triangleVertex.size(); i++)
    {
        pyravertices.push_back(triangleFace1.triangleVertex[i]);
        pyraColorVertices.push_back(triangleFace1.colorVertex[i]);
        pyraTextureCoords.push_back(triangleFace1.textCoord[i]);
    }
    //cout<<"pyramid vertices: "<<pyravertices.size()<<endl;
    //cout<<"pyramid texture coords: "<<pyraTextureCoords.size()<<endl;
    //cout<<"pyramid Normals Color: "<<pyraColorVertices.size()<<endl;
    //PYRVAO, PYRVBO 
    
     //base buffer VAO, VBO;
    glGenBuffers(1, &PYRVBO);
    glGenVertexArrays(1, &PYRVAO);
    //light buffer VAO, VBO
    
   //bind Base vertex to BUFFER
   glBindVertexArray(PYRVAO);
   glGenBuffers(1, &PYRVBO);
   glBindBuffer(GL_ARRAY_BUFFER, PYRVBO );
   glBufferData(GL_ARRAY_BUFFER, pyravertices.size() * sizeof(vec3), &pyravertices[0], GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(0);

   //bind Base vertex to Normal
  
   glBindVertexArray(PYRVAO);
   glGenBuffers(1, &PYRVBO);
   glBindBuffer(GL_ARRAY_BUFFER, PYRVBO);
   glBufferData(GL_ARRAY_BUFFER,   pyraColorVertices.size()*sizeof(vec3), &pyraColorVertices[0], GL_STATIC_DRAW);   
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(1);

   //Base TexCoord
   glBindVertexArray(PYRVAO);
   glGenBuffers(1, &PYRVBO);
   glBindBuffer(GL_ARRAY_BUFFER, PYRVBO);
   glBufferData(GL_ARRAY_BUFFER,   pyraTextureCoords.size()*sizeof(vec2), &pyraTextureCoords[0], GL_STATIC_DRAW);   
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glEnableVertexAttribArray(2);











}



//this compute the normal of the cone
vec3 computeNormal(vec3 a, vec3 b, vec3 c){
      
      vec3 x = vec3(b.x - a.x, b.y - a.y, b.z - a.z);
      vec3 y = vec3(c.x - a.x, c.y - a.x, c.z - a.z);
      vec3 result = vec3(((x.y*y.z)-(y.y*x.z)), ((x.x*y.z)-(y.x*x.z)),((x.x*y.y)-(y.x*x.y)));
      return result;
 };
//renders the cone object
void renderCone(float ConHeight,int ProgramID){
     vector<vec3> pts; //vertices
     vector<vec3> nts;// normals
     vector<vec2> tex;//text coordinates

     vec3 a = vec3(0.0, ConHeight, 0.0);
     vec3 b, c;
     b.y = c.y = 0.0;
     float t,s;
     float i = 0.05;
     int iSectorCount = 0;
     float fReciprocalPrecisition = (float)(10 / 2*360);
     for (double angle = 0; angle<=360; angle+=10){
          c.x = b.x;
          c.z = b.z;
          b.x = cos(radians(angle));
          b.z = sin(radians(angle));
             
             // relative texture coordinates
          float fTextureOffsetS1 = iSectorCount       * fReciprocalPrecisition;
          float fTextureOffsetS2 = (iSectorCount + 1) * fReciprocalPrecisition;
          float sa =  a.x/ (2*360);
          float ta =  a.z/ (2*360);
            
          float sb =  b.x/ (2*360);
          float tb =  b.z/ (2*360);
          //cout<<"fTextureOffsetS1:\t"<<fTextureOffsetS1/2*360<<"fTextureOffsetS2:\t"<<fTextureOffsetS2/2*360<<endl;
          float sc =  c.x/ (2*360);
          float tc =  c.z/ (2*360);

          if(angle!=0){
                //compute the Normal of the cone...
            vec3 normal =  computeNormal(a,  b, c);
            nts.push_back(normal);
                //compute the vertex of the cone...
                
            pts.push_back(a);
            tex.push_back(vec2(0.5*(fTextureOffsetS1/(2*360)), 1.0));
            pts.push_back(b);
            tex.push_back(vec2(0.5*(fTextureOffsetS2/(2*360)), 0.0));
            pts.push_back(c);
            tex.push_back(vec2(0.5*(fTextureOffsetS2/(2*360)), 0.0));
               
                
                //get texture...
                /*
                 tex.push_back(vec2(1-a.x/angle, 1-a.y/angle));
                 tex.push_back(vec2(1-b.x/angle, 1-b.y/angle));
                 
                 
                 tex.push_back(vec2(1-c.x/angle, 1-c.y/angle));
             */
                 iSectorCount++;
             }
             
            
            
           }
           //draw the circle below
           for (double angle = 0; angle<=360; angle+=10){
                a = vec3(0.0);
                tex.push_back(vec2(0.0));
                tex.push_back(vec2((b.x/cos(2*M_PI)), 1-b.z/1));
                //tex.push_back(vec2(1+(b.x/sin(angle)), b.z));
                c.x = b.x;
                c.z = b.z;
                b.x = cos(radians(angle));
                b.z = sin(radians(angle));
                
               double cs = cos(angle);
               double sn = sin(angle);

               float fTextureOffsetS = 0.5F + (float)(0.5F * cs);
               float fTextureOffsetT = 0.5F + (float)(0.5F * sn);
               float sa =  a.x/ (2*360);
               float ta =  a.z/ (2*360);
                
               float  sb =  b.x/ (2*360);
               float  tb =  b.z/ (2*360);
                
               float  sc =  c.x/ (2*360);
               float  tc =  c.z/ (2*360);
                
                vec3 normal =  computeNormal(a,  b, c);
                pts.push_back(a);
                tex.push_back(vec2(fTextureOffsetS/(2*M_PI), fTextureOffsetT/(2*M_PI)));
                pts.push_back(b);
                //tex.push_back(vec2(1*sb, 1.0));
                pts.push_back(c);
                //tex.push_back(vec2(1*sc,  1.0));
                nts.push_back(normal);
             
             }
             
             

        //cout<<"size of points:"<<pts.size()<<endl;

        //plane buffer VAO, VBO;
        glGenBuffers(1, &ConeVBO);
        glGenVertexArrays(1, &ConeVAO);
        //cone vertex
        //cone vertex position to BUFFER
        glBindVertexArray(ConeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, ConeVBO);
        glBufferData(GL_ARRAY_BUFFER, pts.size() * sizeof(vec3), &pts[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        //bind cone vertex to Normal

        glBindVertexArray(ConeVAO);
        glGenBuffers(1, &ConeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, ConeVBO);
        glBufferData(GL_ARRAY_BUFFER,  nts.size()*sizeof(vec3), &nts[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);


        //cone texCoord
        glBindVertexArray(ConeVAO);
        glGenBuffers(1, &ConeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, ConeVBO);
        glBufferData(GL_ARRAY_BUFFER,   tex.size()*sizeof(vec2), &tex[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
        
        glBindVertexArray(ConeVAO);
                //(sizeof(array)/sizeof(array[0])
                //int arrayObj = (sizeof(array)/sizeof(array[0]);
        glDrawArrays(GL_TRIANGLES, 0, 2*360);
           
}

void renderQuad(int ProgramID){
    vector<vec3> quadVertices;
    vector<vec3> quadNormal;
    vector<vec2> quadTexcoord;
    rectangle Quad(vector<vec3>{
              vec3(-1.0f,  1.0f, 0.0f),
              vec3(-1.0f, -1.0f, 0.0f), 
              vec3(1.0f,  1.0f, 0.0f),
              vec3(1.0f, -1.0f, 0.0f)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    }, vector<vec3>{
             



    }, vector<vec2>{
        vec2(0.0f, 1.0f), 
        vec2(0.0f, 0.0f), 
        vec2(1.0f, 1.0f), 
        vec2(1.0f, 0.0f)
    });



     for(int ix = 0; ix<Quad.vertexPos.size(); ix++){
        quadVertices.push_back(Quad.vertexPos[ix]);
        //quadNormal.push_back(Quad.colorPos[ix]);
        quadTexcoord.push_back(Quad.texturePos[ix]);
    }



    //cube buffer VAO, VBO;
glGenBuffers(1, &QuadVBO);
glGenVertexArrays(1, &QuadVAO);
//cube vertex
//bind cube vertex to BUFFER
glBindVertexArray(QuadVAO);
glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(vec3), &quadVertices[0], GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(0);




//cube texCoord
glBindVertexArray(QuadVAO);
glGenBuffers(1, &QuadVBO);
glBindBuffer(GL_ARRAY_BUFFER,QuadVBO);
glBufferData(GL_ARRAY_BUFFER,   quadTexcoord.size()*sizeof(vec2), &quadTexcoord[0], GL_STATIC_DRAW);   
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(2);


glBindVertexArray(QuadVAO);
glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
glBindVertexArray(0);



}


void renderQuad2(int ProgramID){
    vector<vec3> quadVertices;
    vector<vec3> quadNormal;
    vector<vec2> quadTexcoord;
    rectangle Quad(vector<vec3>{
        vec3(-1.0f,  1.0f, 0.3f),
        vec3(-1.0f, -1.0f, 0.3f), 
        vec3( 1.0f,  1.0f,  0.3f),
        vec3(1.0f,  -1.0f, 0.3f),
        vec3(1.0f, 1.0f, 0.3f), 
        vec3(-1.0, 1.0f,0.3f)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
            }, vector<vec3>{
             



    }, vector<vec2>{
        vec2(0.0f, 1.0f), 
        vec2(0.0f, 0.0f), 
        vec2(1.0f, 1.0f), 
        vec2(1.0f, 0.0f),
        vec2(1.0f, 0.0f),
        vec2(0.0f, 1.0f)
    });



     for(int ix = 0; ix<Quad.vertexPos.size(); ix++){
        quadVertices.push_back(Quad.vertexPos[ix]);
        //quadNormal.push_back(Quad.colorPos[ix]);
        quadTexcoord.push_back(Quad.texturePos[ix]);
    }



    //cube buffer VAO, VBO;
glGenBuffers(1, &QuadVBO);
glGenVertexArrays(1, &QuadVAO);
//cube vertex
//bind cube vertex to BUFFER
glBindVertexArray(QuadVAO);
glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(vec3), &quadVertices[0], GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(0);



//cube texCoord
glBindVertexArray(QuadVAO);
glGenBuffers(1, &QuadVBO);
glBindBuffer(GL_ARRAY_BUFFER,QuadVBO);
glBufferData(GL_ARRAY_BUFFER,   quadTexcoord.size()*sizeof(vec2), &quadTexcoord[0], GL_STATIC_DRAW);   
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(2);


glBindVertexArray(QuadVAO);
glDrawArrays(GL_TRIANGLES, 0, 6);
glBindVertexArray(0);



}

void renderSkyBox(){

     //SKYBOX
    vector<vec3>  SkyBoxVertices;
   vector<vec3>  SkyBoxColorVertices;
   rectangle SkyBoxFaces(vector<vec3>{//TOP SIDE 1
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f, -1.0f, -1.0f), 
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(1.0f, -1.0f, -1.0),
                                    vec3(1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),

                                    //RIGHT SIDE 2
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(-1.0f, -1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f,  1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),

                                     //BOTTOM SIDE 3
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(1.0f, -1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f, -1.0f),
                                    vec3(1.0f, -1.0f, -1.0f),


                                     //LEFT SIDE 4
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(-1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f, -1.0f,  1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),

                                    //FRONT SIDE 5
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(1.0f,  1.0f, -1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(-1.0f,  1.0f,  1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    //FRONT SIDE 6
                                    vec3(-1.0f, -1.0f, -1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(1.0f, -1.0f,  1.0f)

   }, vector<vec3>{}, vector<vec2>{});
   //SKYBOX properties
   for(int ix = 0; ix< SkyBoxFaces.vertexPos.size(); ix++){
      SkyBoxVertices.push_back( SkyBoxFaces.vertexPos[ix]);
   }

    //cube VAO, VBO
    //cube buffer VAO, VBO;
    glGenBuffers(1, &SkyBoxVBO);
    glGenVertexArrays(1, &SkyBoxVAO);
    //cube vertex
    //bind cube vertex to BUFFER
    glBindVertexArray(SkyBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, SkyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, SkyBoxVertices.size() * sizeof(vec3), &SkyBoxVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glEnableVertexAttribArray(0);




}

double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}


double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}



void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime); // move up
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime); // move DOWN
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

GLuint loadcubemapimages(vector<string> faces){
    /* 
    TextureID-> return textureID
    ImageVars-> include the width, height and image array itself
    
    */
    GLuint TextureID;
    glGenTextures(1, &TextureID);
    glActiveTexture(GL_TEXTURE0);
    
    int width, height, nrChannels;
    unsigned char* image;
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);
    
    for(int i =0; i<faces.size(); i++){
    
       image = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
       stbi_set_flip_vertically_on_load(true);
      
       
       if (image){
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
          stbi_image_free(image);
         
       }else{
          std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
          stbi_image_free(image);
         
       }
    
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    
    
    return TextureID;

}


//load texture 

GLuint loadTextures2Object(char const *imPath){

       GLuint textureID;
       unsigned char* image;
       glGenTextures(1, &textureID);
       int width,height, nrChannels;
       
       image = stbi_load(imPath , &width, &height, &nrChannels, 0);
       
       // Assign texture to ID
       glBindTexture(GL_TEXTURE_2D, textureID);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
       glGenerateMipmap(GL_TEXTURE_2D);	
 
       // Parameters
       
       if(image){
        GLenum imgFormat;
        if(nrChannels == 1)
          imgFormat = GL_RED;
        else if(nrChannels == 3)
          imgFormat = GL_RGB;
        else if(nrChannels == 4)
           imgFormat = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, imgFormat, width, height, 0, imgFormat, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(image);
        stbi_set_flip_vertically_on_load(true);

        }else{
            cout << "Texture failed to load at path: " << imPath << std::endl;
            stbi_image_free(image);
        };
       
       return textureID;




}


float intersectSphere(glm::vec3 position, float radius, Ray r){
      float a  = dot(r.direction(), r.direction());
      float b  = dot(2.0f * r.direction(), r.origin()-position);
      float c  = dot(r.origin() - position,  r.origin() - position) - (radius * radius);
      float d  = (b*b) - 4.0f * a * c;
      
       if (d < 0) return -1;
      //compute the t1 and t2
      float t1 = (-b + sqrt(d))/(2*a);
      float t2 = (-b - sqrt(d))/(2*a);
      
      
      // compute the closest t 
      float tNear = std::min(t1, t2);
      float tFar=   std::max(t1, t2);
      
      if(tFar < 0) return -1;
      
      if(tNear<0) return tFar;

};




float intersectBox(glm::vec3 position, glm::vec3 boxSize, Ray r){
      float t;
      vec3 boxMin = position - boxSize / 2.0f;
      vec3 boxMax = position + boxSize / 2.0f;
      
      vec3 t1 = (boxMin - r.origin()) / r.direction(); //compute the t1
      vec3 t2 = (boxMax - r.origin()) / r.direction(); //compute the t2
      
      vec3 tMin = glm::min(t1, t2);
      vec3 tMax = glm::max(t1, t2);
      
      float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
      float tFar =  glm::min(glm::min(tMax.x, tMax.y), tMax.z);
      
      
      if (tNear >= tFar || tFar <= 0.0)
          { 
            t = -1.0;
            cout<<"t Near: "<<t<<endl;
            return t;
            
          }
          
          
      if (tNear < 0.0)
          {   
                t = tFar;
                cout<<"t Far:"<<t<<endl;
		return t;
          }



};




//create shadow buffer...

GLuint depthFBO(){

       unsigned int fbo;
       glGenFramebuffers(1, &fbo);//generate framebuffer
       
       //bind framebuffer
       glBindFramebuffer(GL_FRAMEBUFFER, fbo);
       
       //Attach a texture framebuffer
       unsigned int textureID;
       glBindTexture(GL_TEXTURE_2D, textureID);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
       
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return fbo,  textureID;



};



GLuint renderRenderCubeFBO(){
    //Camera Xcamera(glm::vec3(0.0f, 0.0f, 3.0f));
    //Framebuffer...
    unsigned int cubeFBO;
    glGenFramebuffers(1, &cubeFBO);//generate Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO);  //bind buffer
    
    //bind Texture to Buffer...
    //1.define Texturebuffer
    //Attach a texture framebuffer
    GLuint TextureID;
    glGenTextures(1, &TextureID);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);
    
    for(int i = 0; i<6; i++){
     
       glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
       camera.switchFace(i);
    
    
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureID, 0);
    
    //Renderbuffer
    unsigned int cubeRBO;
    glGenRenderbuffers(1, &cubeRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, cubeRBO); 
    glRenderbufferStorage(GL_RENDERBUFFER,  GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cubeRBO);
    
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return cubeFBO, TextureID;


}

//new version of Dynamic
unsigned int DynamicCubemap(){
             //initialize Cubemap...
             glGenTextures(1, &TexCubeMapID);
             glActiveTexture(GL_TEXTURE1);	
             glBindTexture(GL_TEXTURE_CUBE_MAP, TexCubeMapID);
             //set texture parameters
             glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
             glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
             glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
             glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
             glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
             glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TexCubeMapID, 0);
             
             for (int face = 0; face < 6; face++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,GL_RGBA,CUBEMAP_SIZE, CUBEMAP_SIZE, 0, GL_RGBA, GL_FLOAT,NULL);
             }
             
             //GL_CHECK_ERRORS;
             
             glGenFramebuffers(1, &FBOID);
             glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBOID);
             //RENDERBUFFER...
             glGenRenderbuffers(1, &RBOID);
             glBindRenderbuffer(GL_RENDERBUFFER, RBOID);
             glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, CUBEMAP_SIZE, CUBEMAP_SIZE);
             glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FBOID);
             glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, TexCubeMapID, 0);
             
             GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
             if(status != GL_FRAMEBUFFER_COMPLETE) {
		cerr<<"Frame buffer object setup error."<<endl;
		exit(EXIT_FAILURE);
	     } else {
		 cerr<<"Framebuffer Object setup is successful."<<endl;
	     }
	      //unbind FBO
	      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	     //unbind renderbuffer
	     glBindRenderbuffer(GL_RENDERBUFFER, 0);
	     
	     //GL_CHECK_ERRORS;

	     //enable depth testing and back face culling
	     //glEnable(GL_DEPTH_TEST);
	     //glEnable(GL_CULL_FACE);

	     cout<<"successful Initialization of Framebuffer Object"<<endl;
	     
	     return FBOID, TexCubeMapID;
	     
	     
	     
}



//first Scene...

void renderScene(int ProgramID){
     
     //OBJECT COLOR PROPERTY...
    vector<vec3> objColor ={vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0), vec3(0.0, 1.0, 1.0),    
    vec3(1.0, 0.0, 1.0), vec3(1.0), 
    vec3(0.4, 0.4, 0.4), vec3(0.5, 1.0, 0.5), 
    vec3(1.0, 0.5, 0.5)};    
    
    
    
    //OBJECT MATERIAL PROPERTIES...
    //Ambient...
    vector<vec3> ambientVec = {vec3(0.0215,0.1745,0.0215), vec3(0.135,0.2225, 0.1575), vec3(0.05375,0.05, 0.06625), vec3(0.1745,0.01175,0.01175), vec3(0.1,0.18725,0.1745),vec3(0.329412,0.223529,0.027451), vec3(0.2125,0.1275,0.054), vec3(0.25,0.25,0.25), vec3(0.19125,0.0735,0.0225), vec3(0.24725,0.1995,0.0745)
    };
    //cout<<"Ambient light properties: "<<ambientVec.size()<<endl;
     //Diffuse...
     vector<vec3> DiffuseVec = {vec3(0.07568,0.61424,0.07568), vec3(0.54,0.89,0.63), vec3(0.18275,0.17,0.22525), vec3(1,	0.829,	0.829), vec3(0.61424,0.04136,0.04136),vec3(0.396,0.74151, 0.69102), vec3(0.780392,0.568627,0.113725), vec3(0.714,	0.4284,0.18144), vec3(0.4,0.4,0.4), vec3(0.7038,0.27048,0.0828)
    };
    //cout<<"Diffuse light properties: "<<DiffuseVec.size()<<"\ncheck length: 0 "<<DiffuseVec[0].z<<endl;
    //Specular...
    vector<vec3> SpecuLarVec = {vec3(0.633,0.727811,0.633), vec3(0.316228,0.316228,0.316228), vec3(0.332741,0.328634,0.346435), vec3(0.296648,0.296648,0.296648), vec3(0.727811,0.626959,0.626959),vec3(0.297254,0.30829,0.306678), vec3(0.297254,0.30829,	0.306678), vec3(0.393548,0.271906, 0.166721), vec3(0.774597,0.774597,0.774597), vec3(0.256777, 0.137622,0.086014)
    };
    //cout<<"Specular light properties: "<<SpecuLarVec.size()<<endl;
    //------------------------------------------------------------
    vector<float> objectShininess = {0.6, 0.1, 0.3, 0.088, 0.6, 0.1, 0.21794872,0.2, 0.6, 256.0 };
    //cout<<"length of object shininess: "<<objectShininess.size()<<endl;
   ///material constants...
    vector<float> matQuadratic = {0.032f,0.032f,0.032f,0.032f,0.032f,0.032f,0.032f,0.032f,0.032f,0.032f};
    vector<float> matConstant =  {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    vector<float> matLinear={0.09f,  0.09f,  0.09f,  0.09f,  0.09f,  0.09f,  0.09f,  0.09f,  0.09f,  0.09f};
    
    
    //OBJECT EMISSIVE PROPERTY...
     //float EMISSIVE = 0, METAL =1, DIELECTRIC =2, LAMBERTIAN=3;
        const int LAMBERTIAN = 0x00000001;
        const int METAL = 0x00000002;
        const int DIELECTRIC = 0x00000004;
        const int EMISSIVE = 0x00000008;
        
        //OBJECT MATERIAL PROPERTIES...
        vector<float> objfuzz = {0.0, 0.4, 0.5, 0.0, 0.8, 0.0, 0.0, 0.0, 0.4, 0.0};
        vector<float> objRefractIdx = {1.0, 0.0, 1.4, 1.9, 1.3, 1.4, 0.0, 1.0, 1.3,1.4};
        vector<int> objMaterial = {LAMBERTIAN, METAL, DIELECTRIC, EMISSIVE, METAL, DIELECTRIC, LAMBERTIAN, EMISSIVE, METAL, DIELECTRIC };
     
             

      //1. Sphere objects
                for(int i = 0; i<spherePosition.size(); i++){
                    vec3 SphereColor = vec3(1.0, 0.0, 1.0);
                    vec3 sphereScale = vec3(0.5);
                    mat4 modelSphere = mat4(1.0);
                    int SphObjType = 1;
                    modelSphere = rotate(modelSphere, (float)radians(90.0*i), vec3(0.0, 1.0, 0.0)); 
                    //modelSphere  = rotate(modelSphere, (float)radians(90.0)*i, vec3(1.0, 0.0, 0.0)); 
                    modelSphere  = scale(modelSphere,  sphereScale);
                    modelSphere  = translate(modelSphere, spherePosition[i] );
                    //SphereObject   = u_object(spherePosition ,sphereScale, objColor, SphObjType);
                    //ObjectProperty(ProgramID, spObject);// this is box object...
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D,  tennisTexture);
                     //glActiveTexture(GL_TEXTURE1);
                   // glBindTexture(GL_TEXTURE_2D,  tennisTexture2);
                    //cout<<" in objects("<<objColor[5].x<<","<<objColor[5].y<<","<<objColor[5].z<<")"<<endl;//color...
                    
                    
                    //OBJECT PROPERTIES...
                    //_________________________________________________
                    //=================================================
                    string objPos  = string("objects[").append(to_string(i)).append(string("].position")); 
                    string objSize = string("objects[").append(to_string(i)).append(string("].objSize")); 
                    string objType = string("objects[").append(to_string(i)).append(string("].objType")); 
                    string objColors = string("objects[").append(to_string(i)).append(string("].objcolor"));
                    
                    glUniform1i(glGetUniformLocation(ProgramID,  objType.c_str()), SphObjType);//object type
                    glUniform3f(glGetUniformLocation(ProgramID,  objSize.c_str()), sphereScale.x,  sphereScale.y,   sphereScale.z);//sphere Scale...
                    glUniform3f(glGetUniformLocation(ProgramID,  objPos.c_str()), spherePosition[i].x,  spherePosition[i].y,   spherePosition[i].z);//sphere object position...
                    glUniform3f(glGetUniformLocation(ProgramID,  objColors.c_str()),  objColor[i].x,  objColor[i].y,  objColor[i].z);//sphere object color...
                    
                    //OBJECT EMISSIVE PROPERTY
                    //_________________________________________________
                    //=================================================
                    string objMat = string("objects[").append(to_string(i)).append(string("].objMat"));
                    string objFuzz = string("objects[").append(to_string(i)).append(string("].objfuzz"));
                    string objrefractIdx = string("objects[").append(to_string(i)).append(string("].objRefractIdx"));
                    glUniform1i(glGetUniformLocation(ProgramID, objMat.c_str()),objMaterial[i]);//Material 
                    glUniform1i(glGetUniformLocation(ProgramID, objFuzz.c_str()),objfuzz[i]);//Fuzz
                    glUniform1f(glGetUniformLocation(ProgramID, objrefractIdx.c_str()),objRefractIdx[i]);//refractive Index

                    
                    //OBJECT MATERIAL PROPERTY
                    //_________________________________________________
                    //=================================================
                    string matAmbient     =   string("materials[").append(to_string(i)).append(string("].ambient"));
                    string matDiffuse     =   string("materials[").append(to_string(i)).append(string("].diffuse")); 
                    string matSpecular    =   string("materials[").append(to_string(i)).append(string("].specular"));    
                    string matShininess   =   string("materials[").append(to_string(i)).append(string("].shininess"));
                    string mattLinear     =   string("materials[").append(to_string(i)).append(string("].linear")); 
                    string matQuadratic   =   string("materials[").append(to_string(i)).append(string("].quadratic"));  
                    string matConstant    =   string("materials[").append(to_string(i)).append(string("].constant"));  
                    
                     glUniform3f(glGetUniformLocation(ProgramID,  matAmbient.c_str()),  ambientVec[i].x,  ambientVec[i].y,  ambientVec[i].z);//sphere ambient material property...
                     glUniform3f(glGetUniformLocation(ProgramID,  matDiffuse.c_str()),  DiffuseVec[i].x,  DiffuseVec[i].y,  DiffuseVec[i].z);//sphere diffuse material property...
                     glUniform3f(glGetUniformLocation(ProgramID,  matSpecular.c_str()),  SpecuLarVec[i].x,  SpecuLarVec[i].y,  SpecuLarVec[i].z);//sphere specular material property...
                    glUniform1f(glGetUniformLocation(ProgramID,  matShininess.c_str()),objectShininess[i]);//material linear
                    glUniform1f(glGetUniformLocation(ProgramID,  mattLinear.c_str()),matLinear[i]);//material linear
                    glUniform1f(glGetUniformLocation(ProgramID,  matQuadratic.c_str()),matQuadratic[i]);//material quadratic
                    glUniform1f(glGetUniformLocation(ProgramID,  matConstant.c_str()),matConstant[i]);//material constant
                     
                                        
                    
                    glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(modelSphere)); 
                    renderSphere(ProgramID); 

              
                }
                

                
                
                
                //2. create cube objects...
                for(int i = 0; i < cubePosition.size(); i++){
                    vec3 BoxColor = vec3(1.0, 1.0, 0.0);
                    vec3 BoxScale = vec3(0.5);
                    mat4 modelBox = mat4(1.0);
                   
                    int  BoxObjType = 2;
                    modelBox  = rotate(modelBox, (float)radians(90.0*i), vec3(0.0, 1.0, 0.0)); 
                    // modelBox  = rotate(modelBox, (float)radians(-90.0), vec3(0.0, -1.0, 0.0)); 
                    modelBox  = rotate(modelBox, (float)radians(90.0), vec3(-1.0, 0.0, 0.0)); 
                    modelBox  = scale(modelBox, BoxScale);
                    modelBox  = translate(modelBox, cubePosition[i]);
                    //BoxObject   = u_object(cubePosition ,BoxScale,  objColor, BoxObjType );
                    //ObjectProperty(ProgramID, BxObject);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D,  cubeTexture);
                    //glActiveTexture(GL_TEXTURE1);
                    //glBindTexture(GL_TEXTURE_2D,  specularTexture);
                    //OBJECT PROPERTIES...
                    //_________________________________________________
                    //=================================================
                    string objPos  = string("objects[").append(to_string(i)).append(string("].position")); 
                    string objSize = string("objects[").append(to_string(i)).append(string("].objSize")); 
                    string objType = string("objects[").append(to_string(i)).append(string("].objType")); 
                    string objColors = string("objects[").append(to_string(i)).append(string("].objcolor"));
                    glUniform1i(glGetUniformLocation(ProgramID, objType.c_str()), BoxObjType);//box object type
                    glUniform3f(glGetUniformLocation(ProgramID, objSize.c_str()), BoxScale.x,  BoxScale.y, BoxScale.z);//box Scale...
                    glUniform3f(glGetUniformLocation(ProgramID, objPos.c_str()), cubePosition[i].x,  cubePosition[i].y,   cubePosition[i].z);//sphere object position...
                    glUniform3f(glGetUniformLocation(ProgramID,  objColors.c_str()),  objColor[i].x,  objColor[i].y,  objColor[i].z);//sphere object color...
                    
                    //OBJECT EMISSIVE PROPERTY
                    //_________________________________________________
                    //=================================================
                    string objMat = string("objects[").append(to_string(i)).append(string("].objMat"));
                    string objFuzz = string("objects[").append(to_string(i)).append(string("].objfuzz"));
                    string objrefractIdx = string("objects[").append(to_string(i)).append(string("].objRefractIdx"));
                    glUniform1i(glGetUniformLocation(ProgramID, objMat.c_str()),objMaterial[i]);//Material 
                    glUniform1i(glGetUniformLocation(ProgramID, objFuzz.c_str()),objfuzz[i]);//Fuzz
                    glUniform1f(glGetUniformLocation(ProgramID, objrefractIdx.c_str()),objRefractIdx[i]);//refractive Index

                    
                    //OBJECT MATERIAL PROPERTY
                    //_________________________________________________
                    //=================================================
                     string matAmbient     =   string("materials[").append(to_string(i)).append(string("].ambient"));
                     string matDiffuse     =   string("materials[").append(to_string(i)).append(string("].diffuse")); 
                     string matSpecular    =   string("materials[").append(to_string(i)).append(string("].specular"));    
                     string matShininess   =   string("materials[").append(to_string(i)).append(string("].shininess"));
                     string mattLinear     =   string("materials[").append(to_string(i)).append(string("].linear")); 
                     string matQuadratic   =   string("materials[").append(to_string(i)).append(string("].quadratic"));  
                     string matConstant    =   string("materials[").append(to_string(i)).append(string("].constant"));
                     glUniform3f(glGetUniformLocation(ProgramID,  matAmbient.c_str()),  ambientVec[i].x,  ambientVec[i].y,  ambientVec[i].z);//sphere ambient material property...
                     glUniform3f(glGetUniformLocation(ProgramID,  matDiffuse.c_str()),  DiffuseVec[i].x,  DiffuseVec[i].y,  DiffuseVec[i].z);//sphere diffuse material property...
                     glUniform3f(glGetUniformLocation(ProgramID,  matSpecular.c_str()),  SpecuLarVec[i].x,  SpecuLarVec[i].y,  SpecuLarVec[i].z);//sphere specular material property...
                    glUniform1f(glGetUniformLocation(ProgramID,  matShininess.c_str()),objectShininess[i]);//material linear
                    glUniform1f(glGetUniformLocation(ProgramID,  mattLinear.c_str()),matLinear[i]);//material linear
                    glUniform1f(glGetUniformLocation(ProgramID,  matQuadratic.c_str()),matQuadratic[i]);//material quadratic
                    glUniform1f(glGetUniformLocation(ProgramID,  matConstant.c_str()),matConstant[i]);//material constant
                    glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(modelBox)); 
                    renderCube(ProgramID); 

           }
           
          
          
          
                //3. Build floor...
          
          
                for(int i = 0; i < cubePosition.size(); i++){ 
                        int floorType = 3;
                        mat4 modelFLoor = mat4(1.0);
                        vec3 floorScale = vec3(25.0);
                        vec3 floorPosition = vec3(0.0);
                        vec3 floorColor = vec3(1.0, 0.0, 1.0);
                        modelFLoor = translate(modelFLoor, vec3(0.0));
                        //glUniformMatrix4fv(glGetUniformLocation(FloorProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                        //glUniformMatrix4fv(glGetUniformLocation(FloorProgramID, "model"), 1, GL_FALSE, glm::value_ptr(modelFLoor)); 
                        //glUniformMatrix4fv(glGetUniformLocation(FloorProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                        //OBJECT PROPERTIES...
                        //====================================================================
                        string objPos  = string("objects[").append(to_string(i)).append(string("].position")); 
                        string objSize = string("objects[").append(to_string(i)).append(string("].objSize")); 
                        string objType = string("objects[").append(to_string(i)).append(string("].objType")); 
                        string objColors = string("objects[").append(to_string(i)).append(string("].objcolor"));
                        glUniform1i(glGetUniformLocation(ProgramID, objType.c_str()), floorType);//box object type
                        glUniform3f(glGetUniformLocation(ProgramID,  objSize.c_str()), floorScale.x,  floorScale.y, floorScale.z);//box Scale...
                        glUniform3f(glGetUniformLocation(ProgramID,  objPos.c_str()), floorPosition.x,  floorPosition.y,   floorPosition.z);//sphere object position...
                        glUniform3f(glGetUniformLocation(ProgramID,  objColors.c_str()),  floorColor.x,  floorColor.y,  floorColor.z);//sphere object color...
                            
                        //OBJECT EMISSIVE PROPERTY
                        //_________________________________________________
                        //=================================================
                         string objMat = string("objects[").append(to_string(i)).append(string("].objMat"));
                         string objFuzz = string("objects[").append(to_string(i)).append(string("].objfuzz"));
                         string objrefractIdx = string("objects[").append(to_string(i)).append(string("].objRefractIdx"));
                         glUniform1i(glGetUniformLocation(ProgramID, objMat.c_str()),objMaterial[i]);//Material 
                         glUniform1i(glGetUniformLocation(ProgramID, objFuzz.c_str()),objfuzz[i]);//Fuzz
                         glUniform1f(glGetUniformLocation(ProgramID, objrefractIdx.c_str()),objRefractIdx[i]);//refractive Index

                            
                         //OBJECT MATERIAL PROPERTY
                         //_________________________________________________
                         //=================================================
                         string matAmbient     =   string("materials[").append(to_string(i)).append(string("].ambient"));
                         string matDiffuse     =   string("materials[").append(to_string(i)).append(string("].diffuse")); 
                         string matSpecular    =   string("materials[").append(to_string(i)).append(string("].specular"));    
                         string matShininess   =   string("materials[").append(to_string(i)).append(string("].shininess"));
                         string mattLinear     =   string("materials[").append(to_string(i)).append(string("].linear")); 
                         string matQuadratic   =   string("materials[").append(to_string(i)).append(string("].quadratic"));  
                         string matConstant    =   string("materials[").append(to_string(i)).append(string("].constant"));
                         glUniform3f(glGetUniformLocation(ProgramID,  matAmbient.c_str()),  ambientVec[i].x,  ambientVec[i].y,  ambientVec[i].z);//sphere ambient material property...
                         glUniform3f(glGetUniformLocation(ProgramID,  matDiffuse.c_str()),  DiffuseVec[i].x,  DiffuseVec[i].y,  DiffuseVec[i].z);//sphere diffuse material property...
                         glUniform3f(glGetUniformLocation(ProgramID,  matSpecular.c_str()),  SpecuLarVec[i].x,  SpecuLarVec[i].y,  SpecuLarVec[i].z);//sphere specular material property...
                         glUniform1f(glGetUniformLocation(ProgramID,  matShininess.c_str()),objectShininess[i]);//material linear
                         glUniform1f(glGetUniformLocation(ProgramID,  mattLinear.c_str()),matLinear[i]);//material linear
                         glUniform1f(glGetUniformLocation(ProgramID,  matQuadratic.c_str()),matQuadratic[i]);//material quadratic
                         glUniform1f(glGetUniformLocation(ProgramID,  matConstant.c_str()),matConstant[i]);//material constant
                         glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(modelFLoor));
                         glActiveTexture(GL_TEXTURE0);
                         glBindTexture(GL_TEXTURE_2D, tileTexture);   
                         PlaneObject(ProgramID);
                         }

           
           
                             

           
           
                     
         
           



}


//Second Scene...
void renderPathtracerScene(int ProgramID){
                int obType;//object type
                vec3 objectPos;//object position
                
                //OBJECT EMISSIVE PROPERTY...
                //float EMISSIVE = 0, METAL =1, DIELECTRIC =2, LAMBERTIAN=3;
                const int LAMBERTIAN = 0x00000001;
                const int METAL = 0x00000002;
                const int DIELECTRIC = 0x00000004;
                const int EMISSIVE = 0x00000008;
                
                
                
                vec3 cameraPosition = camera.Position;
                vec3 cameraDirection = camera.Front;
                
                glm::mat4 model = glm::mat4(1.0f);
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
                glm::mat4 view = camera.GetViewMatrix();



                //floor   
                glUseProgram(ProgramID);
                int  FloorObType = 1;
                objectPos = vec3(0.0);
                vec3 FloorScaleT = vec3(20.0f);
                model = scale(model, FloorScaleT);
                model = rotate(model, radians(90.0f), vec3(0.0, 0.0, 1.0));
                model = rotate(model, radians(90.0f), vec3(0.0, 1.0, 0.0));
                vec3 floorColor = vec3(1.0);
                
                
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"), FloorScaleT.x,FloorScaleT.y, FloorScaleT.z);
                glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"), FloorObType);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"),  objectPos.x, objectPos.y, objectPos.z);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  floorColor.x, floorColor.y, floorColor.z);
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(BaseVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                
                //top side
                int TopobType = 1;
                mat4 topmodel = glm::mat4(1.0f);
                vec3 topSide = vec3(20.0f);
                vec3 topSidePos = glm::vec3(0.0f, 1.0f, 0.0);
                topmodel = scale( topmodel, topSide);
                topmodel = glm::translate( topmodel, topSidePos );
                topmodel = rotate( topmodel, radians(90.0f), vec3(0.0, 0.0, 1.0));
                topmodel = rotate( topmodel, radians(90.0f), vec3(0.0, 1.0, 0.0));
                
                vec3  topLightColor = vec3(1.0,0.0,0.0);
                //glUseProgram(ProgramID);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"), topSide.x,topSide.y, topSide.z);
                glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"), TopobType);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"),  topSidePos.x, topSidePos.y,topSidePos.z);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),   topLightColor.x, topLightColor.y,  topLightColor.z);
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(topmodel));
                glBindVertexArray(BaseVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                
                                
                
                 //left side
                int leftSideObjType = 1;
                mat4 leftmodel = glm::mat4(1.0f);
                vec3 leftSideScale = vec3(20.0f);
                vec3 leftSidePosition = glm::vec3(-0.5f, 0.5f, 0.0);
                leftmodel = scale( leftmodel, leftSideScale);
                leftmodel = glm::translate( leftmodel,  leftSidePosition);
                leftmodel = rotate( leftmodel, radians(-90.0f), vec3(1.0, 0.0, 0.0));
                leftmodel = rotate( leftmodel, radians(90.0f), vec3(0.0, -0.5, 0.0));
                
                vec3  leftSideColor = vec3(0.0,1.0,0.0);
                //glUseProgram(ProgramID);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),  leftSideScale.x, leftSideScale.y,  leftSideScale.z);
                glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),  leftSideObjType);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"),  leftSidePosition.x, leftSidePosition.y,leftSidePosition.z);
                //glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),   topLightColor.x, topLightColor.y,  topLightColor.z);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  leftSideColor.x,leftSideColor.y, leftSideColor.z);
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(leftmodel));
                glBindVertexArray(BaseVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                
                //right side
                int rightSideObjType = 1;
                vec3 rightScale = vec3(20.0f);
                mat4 rightmodel = glm::mat4(1.0f);
                vec3 rightSidePosition =  glm::vec3(0.5f, 0.5f, 0.0);
                rightmodel = scale( rightmodel, rightScale);
                rightmodel = glm::translate( rightmodel, rightSidePosition);
                rightmodel = rotate( rightmodel, radians(-90.0f), vec3(1.0, 0.0, 0.0));
                rightmodel = rotate( rightmodel, radians(90.0f), vec3(0.0, -0.5, 0.0));
                
                vec3 rightSideColor = vec3(0.0,0.0,1.0);
                //glUseProgram(ProgramID);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"),  rightSidePosition.x, rightSidePosition.y,rightSidePosition.z);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),  rightScale.x, rightScale.y,  rightScale.z);
                glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),  rightSideObjType);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  rightSideColor.x,rightSideColor.y, rightSideColor.z);
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(rightmodel));
                glBindVertexArray(BaseVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);

                
                
                //back side
                mat4 backmodel = glm::mat4(1.0f);
                int blackSideType =1;
                vec3 backScale = vec3(20.0f);
                vec3 backSidePosition = glm::vec3(0.0f, 0.5f, -0.5f);
                backmodel = scale(backmodel, backScale);
                backmodel = glm::translate(backmodel, backSidePosition);
                backmodel = rotate(backmodel, radians(-90.0f), vec3(1.0, 0.0, 0.0));
                backmodel = rotate(backmodel, radians(90.0f), vec3(0.0, -1.5, 0.0));
                backmodel = rotate(backmodel, radians(-90.0f), vec3(1.0, 0.0, 0.0));
                vec3 backSideColor = vec3(1.0,1.0,0.0);
                //glUseProgram(ProgramID);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"),  backSidePosition.x, backSidePosition.y,backSidePosition.z);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),   backScale.x,  backScale.y,   backScale.z);
                glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),  blackSideType);
                glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  backSideColor.x,backSideColor.y, backSideColor.z);
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr( backmodel));
                glBindVertexArray(BaseVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                
                
                //spheres
                
                 //render sphere 0
                 int sphereType0 =2;
                 vec3 sphereColor0 = vec3(1.0, 0.0, 1.0);
                 mat4 spheremodel = glm::mat4(1.0f);
                 vec3 spherePosition0 = glm::vec3(2.0f, 1.5f, 1.0);
                 vec3 sphereScale0 = vec3(1.0);
                 spheremodel = scale(spheremodel, sphereScale0 );
                 spheremodel = glm::translate( spheremodel, spherePosition0);
                 spheremodel = glm::scale( spheremodel, sphereScale0);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"), spherePosition0.x,spherePosition0.y, spherePosition0.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),  sphereScale0.x, sphereScale0.y,  sphereScale0.z);
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),  sphereType0);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  sphereColor0.x,sphereColor0.y, sphereColor0.z);
                 glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr( spheremodel));
                 renderSphere(ProgramID);
                 
                 
                  //render sphere 1
                 int sphereType1 = 2;
                 float refIdx1 = 1.0;
                 spheremodel = glm::mat4(1.0f);
                 vec3 sphereScale1 = glm::vec3(1.0f);
                 vec3 sphereColor1 = vec3(0.0, 1.0, 0.0);
                 vec3 spherePosition1 = glm::vec3(1.0f, 1.5f, -1.5);
                 spheremodel = glm::translate(spheremodel, spherePosition1);
                 spheremodel = glm::scale( spheremodel,sphereScale1);
                 glUniform1f(glGetUniformLocation(ProgramID,  "objects.objRefractIdx"), refIdx1);
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objMat"), EMISSIVE);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"), spherePosition1.x,spherePosition1.y, spherePosition1.z);
                  glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),  sphereType1);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),  sphereScale1.x, sphereScale1.y, sphereScale1.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  sphereColor1.x,sphereColor1.y, sphereColor1.z);
                 glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(spheremodel));
                 renderSphere(ProgramID);


                  //render sphere 2
                 spheremodel = glm::mat4(1.0f);
                 int sphereType2 = 2; 
                 float refIdx = 1.0;
                 vec3 sphereScale2 =  glm::vec3(1.0f); 
                 vec3 spherePosition2 =  glm::vec3(1.0f, 1.5f, -3.0);
                 vec3 sphereColor2 = vec3(1.0, 0.5, 0.0);
                 spheremodel = glm::translate( spheremodel, spherePosition2);
                 spheremodel = glm::scale( spheremodel, sphereScale2);
                 
                 glUniform1f(glGetUniformLocation(ProgramID,  "objects.objRefractIdx"), refIdx);
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objMat"), EMISSIVE);
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),  sphereType2);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"), spherePosition2.x,spherePosition2.y, spherePosition2.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),  sphereScale2.x, sphereScale2.y, sphereScale2.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  sphereColor2.x,sphereColor2.y, sphereColor2.z);
                 glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr( spheremodel));
                 renderSphere(ProgramID);


                 //cube 1
                 int cubeType1 = 3;
                 float cubeRefIdx1 = 0.95;
                 int LambertCube1 = LAMBERTIAN;
                 vec3 cubeScale1 =  vec3(1.0);
                 vec3 cubeColor1 = vec3(1.0, 2.0, 0.0);
                 mat4 cubemodel = glm::mat4(1.0f);
                 vec3 cubePosition1 = glm::vec3(-1.0f, 1.2f, 2.0); 
                 cubemodel = glm::translate(cubemodel, cubePosition1);
                 //model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
                 cubemodel = glm::scale(cubemodel,  cubeScale1);
                 glUniform1f(glGetUniformLocation(ProgramID,  "objects.objRefractIdx"),  cubeRefIdx1);
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objMat"), LambertCube1);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),   cubeScale1.x, cubeScale1.y,  cubeScale1.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"), cubePosition1.x,cubePosition1.y, cubePosition1.z);
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),  cubeType1);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  cubeColor1.x,cubeColor1.y, cubeColor1.z);
                 glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cubemodel));
                 renderCube(ProgramID);
                 
                 //cube 2
                 int  cubeType2 = 3;
                 vec3 cubeColor2 = vec3(0.0, 1.0, 1.0);
                 vec3 cubeScale2 = glm::vec3(1.0);
                 vec3 cubePosition2 = glm::vec3(-1.2f, 1.2f, -1.0);
                 cubemodel = glm::mat4(1.0f);
                 cubemodel = glm::translate(cubemodel, cubePosition2);
                 //model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
                 cubemodel = glm::scale(cubemodel, glm::vec3(1.0));
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),  cubeType2);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),    cubeScale2.x,  cubeScale2.y, cubeScale2.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"), cubePosition2.x,cubePosition2.y, cubePosition2.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  cubeColor2.x,cubeColor2.y, cubeColor2.z);
                 glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cubemodel));
                 renderCube(ProgramID);


                     //glUniformMatrix4fv(glGetUniformLocation( programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
                    
                    
                
                
                
                //pyramid
                //spheres
                
                 //render pyramid 1
                 int pyramidType1 = 4;
                 vec3 pyramidColor = vec3(1.0, 0.25, 0.0);
                 float pyramidRefIdx = 0.0;
                 mat4 pyramidmodel = glm::mat4(1.0f);
                 vec3 pyramidScale1 = vec3(1.5f);
                 vec3 pyramidPosition1 = glm::vec3(-4.0f, 1.0f, 1.0);
                 pyramidmodel = scale(pyramidmodel, pyramidScale1);
                 pyramidmodel = glm::translate( pyramidmodel, pyramidPosition1);
                 //pyramidmodel = glm::scale( pyramidmodel, glm::vec3(1.0f));
                 glUniform1f(glGetUniformLocation(ProgramID,  "objects.objRefractIdx"),  pyramidRefIdx);
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objMat"), DIELECTRIC);
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),  pyramidType1);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),   pyramidScale1.x,  pyramidScale1.y, pyramidScale1.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"), pyramidPosition1.x,pyramidPosition1.y, pyramidPosition1.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  pyramidColor.x,pyramidColor.y, pyramidColor.z);
                 glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidmodel));
                 glBindVertexArray(PYRVAO);
                 glDrawArrays(GL_TRIANGLES,0, 18);
                 //renderSphere(ProgramID);
                 renderPyramind(ProgramID);
                
                
                 //render pyramid2
                 int  pyramidType2 = 4;
                 vec3 pyramidScale2 =  vec3(1.5f);
                 vec3 pyramidColor2 = vec3(0.25, 0.0, 1.0);
                 vec3 pyramidPosition2 = glm::vec3(1.0f, -0.0f, 2.0);
                 //mat4 pyramidmodel = glm::mat4(1.0f);
                 pyramidmodel = scale(pyramidmodel, pyramidScale2);
                 pyramidmodel = glm::translate( pyramidmodel, pyramidPosition2);
                 //pyramidmodel = glm::scale( pyramidmodel, glm::vec3(1.0f));2
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),   pyramidType2);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),   pyramidScale2.x, pyramidScale2.y, pyramidScale2.z);
                  glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"), pyramidPosition2.x, pyramidPosition2.y, pyramidPosition2.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  pyramidColor2.x,pyramidColor2.y, pyramidColor2.z);
                 glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidmodel));
                 glBindVertexArray(PYRVAO);
                 glDrawArrays(GL_TRIANGLES,0, 18);
                 renderPyramind(ProgramID);
                 
                 
                 //cone 1
                 int coneType1 = 5;
                 float coneRefIDx1 = 0.5;
                 vec3 coneScale1 = vec3(1.5f);
                 vec3 coneColor1 = vec3(1.0, 0.0, 0.25);
                 mat4 Conemodel = mat4(1.0);
                 float ConHeight = 2.0;
                 vec3  conePosition1 =  glm::vec3(1.5f, 0.0f, 2.0);
                 Conemodel = scale(Conemodel, coneScale1);
                 Conemodel = glm::translate( Conemodel, conePosition1);
                 //Conemodel = glm::scale(Conemodel, glm::vec3(1.0f));
                  glUniform1f(glGetUniformLocation(ProgramID,  "objects.objRefractIdx"),  coneRefIDx1);
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objMat"), METAL);
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),    coneType1);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"), conePosition1.x, conePosition1.y, conePosition1.z);
                 glUniform1f(glGetUniformLocation(ProgramID,  "objects.objectHeight"),ConHeight);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),   coneScale1.x, coneScale1.y, coneScale1.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  coneColor1.x,coneColor1.y,coneColor1.z);
                 glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(Conemodel));
                 glBindVertexArray(ConeVAO);
                 renderCone(ConHeight, ProgramID);
                 
                 //cone 2
                 int coneType2 = 5;
                 vec3 coneColor2 = vec3(1.0,0.25, 0.0);
                 Conemodel = mat4(1.0);
                 float ConHeight2 = 1.5;
                 vec3 coneScale2 = vec3(1.5f);
                 vec3 conePosition2 =  glm::vec3(-1.0f, 0.0f, 2.5);
                 Conemodel = scale(Conemodel, coneScale2);
                 Conemodel = glm::translate( Conemodel, conePosition2);
                 //Conemodel = glm::scale(Conemodel, glm::vec3(1.0f));
                 glUniform1i(glGetUniformLocation(ProgramID,  "objects.objType"),    coneType2);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.position"), conePosition2.x, conePosition2.y, conePosition2.z);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objSize"),    coneScale2.x,  coneScale2.y,  coneScale2.z);
                 glUniform1f(glGetUniformLocation(ProgramID,  "objects.objectHeight"),ConHeight2);
                 glUniform3f(glGetUniformLocation(ProgramID,  "objects.objcolor"),  coneColor2.x,coneColor2.y,coneColor2.z);
                 glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(Conemodel));
                 glBindVertexArray(ConeVAO);
                 renderCone(ConHeight, ProgramID);

}





