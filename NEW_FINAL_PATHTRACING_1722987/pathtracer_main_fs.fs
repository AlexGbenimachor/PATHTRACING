#version 430 core

#define M_PI 3.1415926535
#define objIndex 10
#define NumBounce 10
#define MaxSample 100
#define EPSILON 0.0001
#define NR_POINT_LIGHTS 6
#define MAXFLOAT 99999.99
#define ROTATION true



out vec4 FragColor;



//material property...
const int LAMBERTIAN = 0x00000001;
const int METAL = 0x00000002;
const int DIELECTRIC = 0x00000004;
const int EMISSIVE = 0x00000008;

struct Ray{

   vec3 origin;
   vec3 direction;

};

struct Triangle{

       vec3 V0, V1, V2;


};

struct Object{
       vec3  position;
       vec3  objSize;
       int   objType;
       vec3  objcolor;
       int   objMat;
       float objfuzz;
       float objRefractIdx;
       float objectHeight;
};


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
    float constant;
    float linear;
    float quadratic;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

//AnyHit
struct AnyHit{

       float t;
       bool  hit;
       int   objIdx;
       vec3  Normal;
       vec3  worldCoord;
       vec3  objColor;
       int  objMat;
       float objfuzz;
       int   objMatprop;
       float objRefractIdx;
       };




struct BoundingBox{

      vec3 min;
      vec3 max;


};

//define the BVH Node
struct BVH{
   vec3 min;
   vec3 max;
   float left;
   float right;
   float parent;
   float objIdx;

};

BVH bvh[10* 2 - 1];


//input variables
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

//generation parameters
uniform float globalTime;
uniform float aspectRatio;
uniform vec2 Iresolution;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform float fov;
uniform vec3 color;

//material uniform or color things 

uniform Light light;
uniform Object objects;
uniform Material materials;



vec3 GenerateRayDirection(vec2 Pixel, int NumSample);
vec3 rayTracing(Ray r);
vec3 Pathtracer(Ray r);
void main(){

    //call ray generation
    Ray ray;
    int Nsample = 10;
    vec3 rayDirection =  GenerateRayDirection(TexCoord,  Nsample);
    vec3 hit = normalize(rayDirection - cameraPos);
    ray.origin = cameraPos;
    ray.direction = rayDirection;
  
   
   vec3 color1 =  Pathtracer(ray);
   //color1 = sqrt(color1)/sqrt(Nsample);
   FragColor = vec4( (color1 * vec3(2.0)), 1.0);


}


//Random generator
float random(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453)+globalTime;//*0.001;
}

//Random min- max
float random_min_max(float min, float max){
      return  min + random(TexCoord) * (max - min);
      
}

//generating rays using monte carlo...
vec3 GenerateRayDirection(vec2 Pixel, int NumSample){
     float px =0.0, py =0.0;
     float scale = tan(fov / 2 * M_PI / 180);
     float Nsqrt = 1/sqrt(NumSample);
     float lowerbound = -1.0, upperbound =1.0;
     float randx =  lowerbound + random(Pixel) * (upperbound-lowerbound);//creating random X
     float randy =  lowerbound + random(Pixel) * (upperbound-lowerbound);//creating random Y
     //float h = tan(theta/2);
     float viewport_height = 2.0 * scale;
     float viewport_width = Iresolution.x / Iresolution.y * viewport_height;
     float focal_length = 1.0;
     
     vec3 horizontal = vec3( viewport_width, 0,  0);
     vec3 vertical = vec3(0, -viewport_height, 0);
     
     vec3 origin = cameraPos.zxy * vec3(-1, 1, 1);
     vec3 lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);
    
     vec2 uv = (Pixel.xy) / Iresolution.xy;
     
     for( int i = 0; i<Iresolution.x; i++){
        for(int j = 0; j< Iresolution.y; j++){
            vec3 pixel_center =  lower_left_corner + (i * uv.x) + (j*uv.y);//the pixel centre...
            for(int ii  = 0; ii< NumSample; ii++){
                for(int jj = 0; ii<NumSample; jj++){
                        px = -0.5 + sqrt(NumSample) * (ii + randx);
                        py = -0.5 + sqrt(NumSample) * (jj + randy);
                
                
                
                vec3 direction =    pixel_center + vec3(px , py, -1.0);    
                
                return direction;
              }
            
            }
        }
    }
     
     
     
     

}




//SPHERE INTERSECTION...
AnyHit SphereIntersection(vec3 position,float radius,  Ray r){
       AnyHit Hc;
       Hc.hit =  false;
       //float radius =  objects.objSize.x; // radius...
       vec3 oc = r.origin - position;//object position
       float a = dot(r.direction, r.direction);
       float b = dot(2*r.direction, oc);
       float c = dot(oc, oc) - (radius*radius);
       //Solving d = (b^2) - 4 * a * c;
       //(discrimant)
       float d = (b*b) - 4*a*c;
       
       if(d<0.0){
         Hc.t = -1;
         return Hc;
       
       }
       //picking the smallest t...
       float t1 = (-b + sqrt(d))/(2*a);
       float t2 = (-b - sqrt(d))/(2*a);
       
       //find the tNear and tFar...
       float tNear = min(t1, t2);
       float tFar  = max(t1, t2);
       
       Hc.t = tNear;
       
       if(tFar < 0.0 && tNear < 0.0){
         Hc.t = -1.0;
         return Hc;
        
        
         }
         
          if(tNear < 0.0 && tFar > 0.0){
            Hc.t = 1.0;
            Hc.hit = true;  
            return Hc;
        
        
         }
        
        
  
        
        //float intersect_distance = tNear;
        //vec3 plane_intersect_distances = tMinDist;
        
             
               
           //0.0001f
            if(tFar> 0.0f && tNear>0.0){
              Hc.t = 1.0;
              Hc.hit = true;        
              return Hc;
                
           }    
        
        Hc.worldCoord = r.origin + Hc.t * r.direction;
        Hc.Normal = normalize(Hc.worldCoord - oc);
        
        if(Hc.hit){
        
        Hc.Normal *= 1.0;
        
        
        }
        return Hc;

}; 



//BOX INTERSECTION...

AnyHit BoxIntersection(vec3 position, vec3 boxSize,  Ray r){

   AnyHit Hc;
   
   vec3 boxMin = position - boxSize / 2.0;
   vec3 boxMax = position + boxSize / 2.0;

   vec3 tMin = (boxMin - r.origin)/r.direction;
   vec3 tMax = (boxMax - r.origin)/r.direction;

   vec3 tMinDist = min(tMin, tMax);
   vec3 tMaxDist = max(tMin, tMax);

   //tFar and tNear for the boxes...
   float tNear = max(max(tMinDist.x, tMinDist.y), tMinDist.z);
   float tFar = min(min(tMaxDist.x, tMaxDist.y), tMaxDist.z);

   Hc.t = tNear;
   Hc.hit = false;
  
   /*if(tNear >= tFar || tFar <=0.0){
       Hc.t = -1.0;
       return Hc;
   }*/
   if(tFar < 0.0){
      Hc.t = -1.0;
      return Hc;
        
        
   }
        
  
        
        //float intersect_distance = tNear;
        //vec3 plane_intersect_distances = tMinDist;
        
   if(tNear<0.0){
      Hc.t = tFar;
      Hc.hit = true;        
      return Hc;
        
   }
        
        // Calculate the world-position of the intersection:
   Hc.worldCoord = r.origin + Hc.t * r.direction;

   vec3 center = (boxMax + boxMin) * 0.5;
   boxSize = (boxMax - boxMin) * 0.5;
   vec3 pc = Hc.worldCoord - center;
   // step(edge,x) : x < edge ? 0 : 1
   Hc.Normal = vec3(0.0);
   Hc.Normal += vec3(sign(pc.x), 0.0, 0.0) * step(abs(abs(pc.x) - boxSize.x), EPSILON);
   Hc.Normal += vec3(0.0, sign(pc.y), 0.0) * step(abs(abs(pc.y) - boxSize.y), EPSILON);
   Hc.Normal += vec3(0.0, 0.0, sign(pc.z)) * step(abs(abs(pc.z) - boxSize.z), EPSILON);


   if(Hc.hit){

      Hc.Normal *= 1.0;


   }

       
       
   return Hc;

};


//PLANE INTERSECTION ...
AnyHit PlaneIntersection(vec3 position, vec3 Normal, Ray r){

       AnyHit Hc;
       float denom  = dot(Normal, r.direction);
       if(abs(denom)>0.0001f){
       
           vec3 oc = position  - r.origin;
           
           Hc.t = dot(oc, Normal)/denom;
           Hc.t = 1.0;
           Hc.worldCoord = r.origin + r.direction * Hc.t;
           Hc.hit = true;
           Hc.Normal =normalize(Hc.worldCoord - oc);
           Hc.objIdx = 3;
           return Hc;
       
       
       
       }else{
       
          Hc.t = -1.0;
          Hc.hit = false;
          return Hc;
       }


      


};



//Pyramid intersection 
AnyHit intersectPyramid(Ray r, vec3 position){
    AnyHit Hc;
    Triangle Tri;
    vec3 oc = position - r.origin;
    float dotProduct;
    vec3 worldCoord;
    Triangle pyramidCoord[]={
                                    // FRONT FACE
    {{0.5f, -0.5f, -0.5f},{0.5f, -0.5, -0.5}, {0.0, 0.5, 0.0}},   
    {{-0.5f, -0.5f, 0.5f},{0.5f, -0.5, 0.5},  {0.0, 0.5, 0.0}},   // TOP
    {{-0.5f, -0.5f, -0.5f},{-0.5f, -0.5f, 0.5f}, {0.0, 0.5, 0.0}},    // TOP
    // LEFT FACE
    {{0.5f, -0.5f, -0.5f},{0.5f, -0.5f, 0.5f},  {0.0f, 0.5f, 0.0f}},
    
     // BASE LEFT
    {{-0.5f, -0.5f, -0.5f},{0.5f, -0.5f, 0.5f},{-0.5f, -0.5f, 0.5f}},
                                    // BASE RIGHT
    {{0.5f, -0.5f, 0.5f},{-0.5f, -0.5f, -0.5f},{0.5f, -0.5f, -0.5f}}

        };
        
    int sizeP = pyramidCoord.length();
    for(int i = 0; i<sizeP; i++){
    
        dotProduct = dot(dot((pyramidCoord[i].V0.x * r.direction.x +pyramidCoord[i].V0.y * r.direction.y+pyramidCoord[i].V0.z * r.direction.z),  (pyramidCoord[i].V1.x * r.direction.x +pyramidCoord[i].V1.y * r.direction.y+pyramidCoord[i].V1.z * r.direction.z)),  (pyramidCoord[i].V2.x * r.direction.x +pyramidCoord[i].V2.y * r.direction.y+pyramidCoord[i].V2.z * r.direction.z));
          
       if(dotProduct == 0.0 ){
           continue;
        }
       float t = dot(dot((pyramidCoord[i].V0.x * oc.x +pyramidCoord[i].V0.y * oc.y+pyramidCoord[i].V0.z * oc.z),  (pyramidCoord[i].V1.x * oc.x +pyramidCoord[i].V1.y * oc.y+pyramidCoord[i].V1.z * oc.z)),  (pyramidCoord[i].V2.x * oc.x +pyramidCoord[i].V2.y * oc.y+pyramidCoord[i].V2.z * oc.z))/dotProduct;
       
       
       if(t<0.0){
       
          continue;
       }
      
      worldCoord = r.origin + t + r.direction;
    }  

    if(worldCoord.x>=-1.0 && worldCoord.x<=1.0 && worldCoord.z >= -1.0 && worldCoord.z<=1.0){
       Hc.t = 1.0;
       Hc.hit = true;
       return Hc;
    }else{
    
     Hc.t = -1.0;
     Hc.hit = false;
     return Hc;
    }
    return Hc; 
}

//cone intersection ...

AnyHit ConeIntersection(Ray r, vec3 Position){
      //define cone parameters ...
      AnyHit Hc;
      float ConeHeight = 2.0;
      float radius =  1.0;
      
      float halfHeight = ConeHeight* 0.5;
      float tanTheta = radius / halfHeight;
      
      //ray parameter
      vec3 oc = Position - r.origin;
      //cout<<"\toc.x:\t"<<oc.x<<"\toc.y:\t"<<oc.y<<"\toc.z:\t"<<oc.z<<endl;
      float k = tanTheta * tanTheta + 1.0;
      
      //cout<<"\tK parameter:\t"<<k<<endl;
      
      vec3 oc2 = cross(oc , oc);
      //cout<<"\toc2.x:\t"<<oc2.x<<"\toc2.y:\t"<<oc2.y<<"\toc2.z:\t"<<oc2.z<<endl;
      float c = oc2.x + oc2.y - tanTheta * tanTheta * oc2.z;
      //cout<<"\tc:\t"<<c<<endl;
      float discriminant = k * r.direction.z * r.direction.z - c;
      //cout<<"\tdiscriminant:\t"<<discriminant<<endl;
      
      if (discriminant < 0.0) {
         Hc.t = -1.0;
         return Hc;
      }
      float sqrtDiscriminant = sqrt(discriminant);
      //cout<<"\tsqrtDiscriminant:\t"<<sqrtDiscriminant<<endl;
      float t1 = (-k * r.direction.z + sqrtDiscriminant) / (1.0 + k);
      float t2 = (-k * r.direction.z - sqrtDiscriminant) / (1.0 + k);
      //cout<<"\tt1-distance:\t"<<t1<<"\tt2-distance:\t"<<t2<<endl;
      float t = max(t1, t2);
      //cout<<"\tt-distance:\t"<<t<<endl;
       if (t < 0.0) {
         Hc.t = -1.0;
        return Hc;
       }
       vec3 worldCoord = r.origin + t * r.direction;
       //cout<<"\tworldCoord.x:\t"<<worldCoord.x<<"\tworldCoord.y:\t"<<worldCoord.y<<"\tworldCoord.z\t"<<worldCoord.z<<endl;
       
       if (worldCoord.z < 0.0 || worldCoord.z > ConeHeight) {
        Hc.t   = -1.0;
        Hc.hit = false;
        return Hc;
       }else{
        Hc.t   = 1.0;
        Hc.hit = true;
      
        return Hc;
      }



}

AnyHit  computecloseObjects(Ray r){
        AnyHit Csph, Cbox, Cpln, Cpyr, Ccon, closestHit;
        closestHit.objIdx = -1;
        Csph = SphereIntersection(objects.position,objects.position.y,  r);
        Cbox = BoxIntersection(objects.position, objects.objSize,  r);
        Cpln = PlaneIntersection(objects.position, vec3(0.0,1.0,0.0), r);
        Cpyr = intersectPyramid(r, objects.position);
        Ccon =  ConeIntersection(r, objects.position);
        
        //intersect floor
        if(objects.objType==1){
          if(Cpln.t < 0.0 || Cpln.t>0.0){
            closestHit = Cpln;
            closestHit.objIdx = 1;
          
          
          }
        
        
        }
        
        
        //intersect Sphere
        if(objects.objType == 2){
          if(Csph.t < 0.0 || Csph.t>0.0){
            closestHit = Csph;
            closestHit.objIdx = 2;
          
           
          
          }
        
        
        }
        //intersect cube ...
         if(objects.objType == 3){
          if(Cbox.t < 0.0 || Cbox.t>0.0){
            closestHit = Cbox;
            closestHit.objIdx = 3;
          
           
          
          }
        
        
        }
        
         //intersect pyramid ...
         if(objects.objType == 4){
          if(Cpyr.t < 0.0 || Cbox.t>0.0){
            closestHit = Cpyr;
            closestHit.objIdx = 4;
          
           
          
          }
        
        
        }
        
         if(objects.objType == 5){
          if(Ccon.t < 0.0 ||Ccon.t>0.0){
            closestHit = Ccon;
            closestHit.objIdx = 5;
          
           
          
          }
        
        
        }






return closestHit;
}



vec3 rayTracing(Ray r){
     
         AnyHit Hc =  computecloseObjects(r);
         //plane/square intersection ...
         if(objects.objType == 1 &&  Hc.objIdx == 1){
         
             vec3 color = objects.objcolor;//vec3(1.0, 0.0, 0.0) ;//* phongLighting(r, Hc) ;
             return color;
         }
         //Sphere intersection ...
        if(objects.objType == 2 &&  Hc.objIdx == 2){
            vec3 color =objects.objcolor;// vec3(0.0, 1.0, 0.0);// *  phongLighting(r, Hc) ;
            return color;
     
        }
        //cube intersection...
        if(objects.objType == 3 &&  Hc.objIdx == 3){
            vec3 color =objects.objcolor;// vec3(0.0, 1.0, 0.0);// *  phongLighting(r, Hc) ;
            return color;
     
        }
        //pyramid intersection
        if(objects.objType == 4 &&  Hc.objIdx == 4){
            vec3 color =objects.objcolor;// vec3(0.0, 1.0, 0.0);// *  phongLighting(r, Hc) ;
            return color;
     
        }
        //cone intersection
        if(objects.objType == 5 &&  Hc.objIdx == 5){
            vec3 color = objects.objcolor;// vec3(0.0, 1.0, 0.0);// *  phongLighting(r, Hc) ;
            return color;
     
        }
     
     
     

};





//this computes the intersection of the BoundBox
bool intersectBox(Ray r,  BoundingBox box) {
    AnyHit Hc;
    vec3 invDir = 1.0 / r.direction;
    vec3 t1 = (box.min - r.origin) * invDir;
    vec3 t2 = (box.max - r.origin) * invDir;
    vec3 tmin = min(t1, t2);
    vec3 tmax = max(t1, t2);
    float tminMax = max(max(tmin.x, tmin.y), tmin.z);
    float tmaxMin = min(min(tmax.x, tmax.y), tmax.z);
    return tminMax <= tmaxMin;
}





AnyHit PopulateBVHnode(Ray r, vec3 min, vec3 max){
       int N = 10* 2 - 1;
       BVH bvh[10* 2 - 1];
       //Object objects;
       AnyHit NearHitObject, Csph, Cbox, Cpln,Cpyr, Ccon;
       
       
       BoundingBox rootBox = BoundingBox(vec3(-1.0), vec3(1.0));
       
       bool hit = intersectBox(r ,  rootBox);
       int i = 0;
       for(int i = 0; i<N; i++){
       
          if(!hit){
             if(bvh[i].parent == 0.0){
                bvh[i].parent = 0.0;
             }
             
            
          if(objects.objType == 2){
            //Sphere intersection
             Csph = SphereIntersection(objects.position, objects.objSize.x,  r);
             if((Csph.t > 0) ||(Csph.t<0)){// && ((Csph.t < Cbox.t) || (Cbox.t < 0))&& !(bvh[i].parent < 0.0)){
                 i=  2*i+1;
                 bvh[i].right = Csph.t;
                 bvh[i].objIdx = 2;
                 NearHitObject = Csph;
                 NearHitObject.objIdx = 2;
                 NearHitObject.hit = true;  
                 //NearHitObject.hit     
             }
          } 
             
          if(objects.objType == 3){
             //Box intersection...
             Cbox =  BoxIntersection(objects.position, objects.objSize,  r);
              if((Cbox.t < 0) || (Cbox.t>0)){//  && ((Cbox.t < Csph.t) || (Csph.t < 0))&& !(bvh[i].parent < 0.0)){
                 i=  2*i+1;
                 bvh[i].left = Cbox.t;
                 NearHitObject = Cbox;
                 bvh[i].objIdx = 3;
                 NearHitObject.objIdx = 3;
                 NearHitObject.hit = true;       
             }
          }
          
          if(objects.objType == 1){
             //floorr, wall intersection
             Cpln = PlaneIntersection(vec3(0.0), vec3(0.0,1.0,0.0), r);//plane intersection...
              if(((Cpln.t > 0) || (Cpln.t<0)) && !(bvh[i].parent < 0.0)){
                 i=  2*i+1;
                 bvh[i].right = Cpln.t;
                 NearHitObject = Cpln;
                 bvh[i].objIdx = 1;
                 NearHitObject.objIdx = 1;
                 NearHitObject.hit = true;       
             }
       
          }    
          
          if(objects.objType == 4){
             //pyramid intersection
             Cpyr = intersectPyramid(r, objects.position);
              if(((Cpyr.t > 0) || (Cpyr.t < 0)) && !(bvh[i].parent < 0.0)){
                 i=  2*i+1;
                 bvh[i].right =Cpyr.t;
                 NearHitObject = Cpyr;
                 bvh[i].objIdx = 4;
                 NearHitObject.objIdx = 4;
                 NearHitObject.hit = true;       
             }
       
          }    
          
           if(objects.objType == 5){
             //Cone intersection
              Ccon =  ConeIntersection(r, objects.position);
              if(((Ccon.t > 0)||((Ccon.t < 0))) && !(bvh[i].parent < 0.0)){
                 i=  2*i+1;
                 bvh[i].right = Ccon.t;
                 NearHitObject =  Ccon;
                 bvh[i].objIdx = 5;
                 NearHitObject.objIdx = 5;
                 NearHitObject.hit = true;       
             }
       
          }    

     
          }else{
             //sphere no interesection...
             if(objects.objType == 2){
                if((Csph.t < 0) && ((Csph.t > Cbox.t) || (Cbox.t > 0))&& !(bvh[i].parent < 0.0)){
                   i = 2 * i + 1;
                   bvh[i].left = Csph.t;
                   bvh[i].objIdx = 2;
                   NearHitObject = Csph;
                   NearHitObject.objIdx = 2;
                   NearHitObject.hit = false;
                }
             }
              //box no intersection ....
              if(objects.objType == 3){
                if((Cbox.t > 0) && ((Cbox.t > Csph.t) || (Csph.t > 0))&& !(bvh[i].parent > 0.0)){
                   i = 2 * i + 1;
                   bvh[i].left = Cbox.t;
                   NearHitObject = Cbox;
                   bvh[i].objIdx = 3;
                   NearHitObject.objIdx = 3;
                   NearHitObject.hit = false;
             }
            }
             if(objects.objType == 1){
               if((Cpln.t < 0) && !(bvh[i].parent < 0.0)){
                  i = 2 * i + 1;
                  bvh[i].left   = Cpln.t;
                  NearHitObject = Cpln;
                  bvh[i].objIdx = 1;
                  NearHitObject.objIdx = 1;
                  NearHitObject.hit = false;
               }
             }
             
             if(objects.objType == 4){
             //pyramid  no intersection
             Cpyr = intersectPyramid(r, objects.position);
              if((Cpyr.t < 0) && !(bvh[i].parent < 0.0)){
                 i=  2*i+1;
                 bvh[i].left =Cpyr.t;
                 NearHitObject = Cpyr;
                 bvh[i].objIdx = 4;
                 NearHitObject.objIdx = 4;
                 NearHitObject.hit = true;       
             }
       
          }    
          
           if(objects.objType == 5){
             //Cone  no intersection
              Ccon =  ConeIntersection(r, objects.position);
              if((Ccon.t < 0) && !(bvh[i].parent < 0.0)){
                 i=  2*i+1;
                 bvh[i].right = Ccon.t;
                 NearHitObject =  Ccon;
                 bvh[i].objIdx = 5;
                 NearHitObject.objIdx = 5;
                 NearHitObject.hit = true;       
             }
       
          }    
          
          
          
          
          
          }
       
       
       }



return NearHitObject;

}

//Traversal algorithm...
AnyHit traverseTree(Ray r){
       vec3 min = vec3(-1.0);
       vec3 max = vec3(1.0);
       AnyHit Hc =  PopulateBVHnode(r, min, max);
       return Hc;
       
       

}


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 Pathtracer(Ray r){

     int NBounce = 0;
      AnyHit Hc =  traverseTree(r);//traverse Nodes
      vec3 color ;
      for(int bounce = 0; bounce<NumBounce; bounce++){
           NBounce  =  NBounce  + 1;
           //floor lighting -> basic square
           if(Hc.objIdx == 1 && objects.objType == 1){//trace floor...
            vec3 hit = normalize(r.direction - r.origin);
            vec3 jitteredLight = lightPos + r.direction;
            vec3 L = normalize(jitteredLight - hit);
           // ambient
            float ambientStrength = 0.2;
            vec3 ambient = ambientStrength * lightColor;
            
            // diffuse 
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(L - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            
             // specular
            float specularStrength = 0.5;
            vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
            vec3 reflectDir = reflect(-lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * lightColor; 
                    

            vec3 result = (ambient+diffuse+specular);
            
            color =  result * objects.objcolor;
           
           
           
           
           
           
           }
           //Boxes lighting-> cube objects... 
           if(Hc.objIdx == 3 && objects.objType == 3){
           
              if (objects.objType == 3 && objects.objMat == LAMBERTIAN){
              
                 vec3 hit = normalize(r.direction - r.origin);
                 vec3 jitteredLight = lightPos + FragPos;
                 vec3 L = normalize(jitteredLight - hit);
                 vec3 N = normalize(Normal);
                 float ScalarI = 1.5;
                 vec3 I = vec3(ScalarI);
                 color =  vec3(dot(N, L)) * objects.objcolor * I;
              
              
              //  return color;
              
              }else {
              
                color = vec3(0.0, 1.0, 0.0) * objects.objcolor;
                //return color;
              
              }

           
           
           
           
           }
           
            //sphere lighting... 
           if(Hc.objIdx == 2 && objects.objType == 2){
           
              if (objects.objType == 2 && objects.objMat == EMISSIVE){
              
                    vec3 hit = normalize(r.direction - r.origin);
                    vec3 jitteredLight = lightPos + r.direction;
                    vec3 L = normalize(jitteredLight - hit);
                   // ambient
                    float ambientStrength = 0.2;
                    vec3 ambient = light.ambient * lightColor;
                    
                    // diffuse 
                    vec3 norm = normalize(Normal);
                    vec3 lightDir = normalize(L - FragPos);
                    float diff = max(dot(norm, lightDir), 0.0);
                    vec3 diffuse = light.diffuse * diff * lightColor;
                    
                     // specular
                    float specularStrength = 0.5;
                    vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
                    vec3 reflectDir = reflect(-lightDir, norm);  
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
                    vec3 specular = light.specular * spec * lightColor; 
                    
                    float distance =  length(lightPos - FragPos);
                    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
                    
                    ambient  *= attenuation;  
                    diffuse   *= attenuation;
                    specular *= attenuation; 
                    vec3 result =  (ambient + diffuse + specular);
                    color = (result * objects.objcolor);
                //return color;
              
              }else {
              
                color = vec3(0.0, 1.0, 0.0) * objects.objcolor;
                //return color;
              
              }

           
           
           
           
           }
           //cone object lighting 
           if(Hc.objIdx == 5 && objects.objType == 5){
              if (objects.objType == 5 && objects.objMat == METAL){

            vec3 matAmbientCone = vec3(0.19225, 0.19225, 0.19225);
            vec3 matDiffuseCone =  vec3(0.50754,0.50754,0.50754);
            vec3 matSpecularCone =  vec3(0.508273, 0.508273,0.508273);
            float matConeShininess = 0.4;
            
            vec3 hit = normalize(r.direction - r.origin);
            vec3 jitteredLight = lightPos + r.direction;
            vec3 L = normalize(jitteredLight - hit);
           // ambient
            float ambientStrength = 0.2;
            vec3 ambient = light.ambient * matAmbientCone * lightColor;
            
            // diffuse 
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(L - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse =  light.diffuse * matDiffuseCone * diff * lightColor;
            
             // specular
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
             float ratio = 1.00 / 1.52;
            vec3 I =  normalize(FragPos - cameraPos);
            vec3 reflectDir = reflect(I, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
            vec3 specular = light.specular * matSpecularCone * spec * lightColor; 
            
            float distance =  length(lightPos - FragPos);
            float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
            
            /*ambient  *= attenuation;  
            diffuse   *= attenuation;
            specular *= attenuation; 
                */    
           /*
            vec3 lightDir = normalize(lightPos - FragPos);
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
           
           */
           
           vec3 result =  (ambient + diffuse + specular);
           color = (result * vec3(1.0));// * objects.objcolor;
          }else{
          
            color =  objects.objcolor;
          
          }
         }
         
          if(Hc.objIdx == 4 && objects.objType == 4){
              if (objects.objType == 4 && objects.objMat == DIELECTRIC){
                  vec3 hit = normalize(r.direction - r.origin);
                  vec3 jitteredLight = lightPos + r.direction;
                  vec3 L = normalize(jitteredLight - hit); //light position...
                  
                  // ambient
                  float ambientStrength = 0.2;
                  vec3 ambient = light.ambient * lightColor;
                  
                   // diffuse 
                   // diffuse 
                  vec3 norm = normalize(Normal);
                  vec3 lightDir = normalize(L - FragPos);
                  float diff = max(dot(norm, lightDir), 0.0);
                  vec3 diffuse =  light.diffuse  * diff * lightColor;
                  
                  //specular
                    float specularStrength = 0.5;
                    vec3 viewDir = normalize(viewPos-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
                     float ratio = 1.00 / 1.52;
                    vec3 I =  normalize(FragPos - cameraPos);
                    vec3 reflectDir = reflect(I, norm);  
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
                    vec3 specular = light.specular * spec * lightColor; 
                    
                    float distance =  length(lightPos - FragPos);
                    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
                  
                  
                    vec3 result =  (ambient + diffuse + specular) * attenuation;
                  
                  float cosTheta = dot(lightDir, norm);  
                  
                  vec3 F0 = vec3(1.00, 0.71, 0.29);
                  vec3 SurfaceColor =  vec3(1.00, 0.86, 0.57);
                  F0  = mix(F0, SurfaceColor, 0.5);
                  vec3 freColor = fresnelSchlick(cosTheta, F0);
              
                  color = (freColor * result) * objects.objcolor;
              
              }
        
        } 
         
         
        if(NBounce == NumBounce){
              break;
          
          
          }
      
      
      
      
      }

      return color; ///=NBounce;




}


