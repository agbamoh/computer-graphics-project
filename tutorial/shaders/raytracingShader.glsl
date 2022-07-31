 #version 330 

uniform vec2 camRot;
uniform vec4 eye;
uniform vec4 ambient;
uniform vec4[20] objects;
uniform vec4[20] objColors;
uniform vec4[10] lightsDirection;
uniform vec4[10] lightsIntensity;
uniform vec4[10] lightsPosition;
uniform ivec4 sizes;

in vec3 position0;
in vec3 normal0;

float innerIntersection(int sourceIndx,vec3 sourcePoint,vec3 v)
{
    float t = 0;
    if(objects[sourceIndx].w > 0) //sphere
    {
        vec3 rVec =  normalize(objects[sourceIndx].xyz - sourcePoint);
        float cosAlfa = dot(rVec,v);
        float r = objects[sourceIndx].w;
        t = 2*r*cosAlfa;
    }
    return t;
}

float intersection(inout int sourceIndx,vec3 sourcePoint,vec3 v)
{
    float tmin = 1.0e10;
    int indx = -1;
    for(int i=0;i<sizes.x;i++) //every object
    {
        if(i==sourceIndx)
            continue;
        if(objects[i].w > 0) //sphere
        {
            vec3 p0o =  objects[i].xyz - sourcePoint;
            float r = objects[i].w;
            float b = dot(v,p0o);
            float delta = b*b - dot(p0o,p0o) + r*r;
             float t;
            if(delta >= 0)
            {
                if(b>=0)
                    t = b - sqrt(delta);
                else
                    t = b + sqrt(delta);
                if(t<tmin && t>0)
                {
                    tmin = t;
                    indx = i;
                } 
            }   
        }
        else  //plane
        {    
            vec3 n =  normalize(objects[i].xyz);
            vec3 p0o = -objects[i].w*n/length(objects[i].xyz) - sourcePoint;
            float t = dot(n,p0o)/dot(n,v); 
            if(t>0 && t<tmin)
            {
                tmin = t;
                indx = i;
            }
        }
    }
    sourceIndx = indx; 
    return tmin;
}


//body index in objects, point on surface of object, diffuseFactor for plane squares
vec3 colorCalc(int sourceIndx, vec3 sourcePoint,vec3 u,float diffuseFactor)
{
    vec3 color = ambient.rgb*objColors[sourceIndx].rgb;
    float specularCoeff = 0.7f;
    for(int i = 0;i<sizes.y;i++) //every light source
    {
        vec3 v;
        if(lightsDirection[i].w < 0.5 ) //directional
        {
            int indx = sourceIndx;
            v = normalize(lightsDirection[i].xyz);
           //  v = normalize(vec3(0.0,0.5,-1.0));
            float t = intersection(indx,sourcePoint,-v);

            
            if(indx < 0 || objects[indx].w<=0 || indx <= sizes.w) //no intersection
            {
               // vec3 u = normalize(sourcePoint - eye.xyz);
                if(objects[sourceIndx].w > 0) //sphere
                {
                    
                    vec3 n = -normalize( sourcePoint - objects[sourceIndx].xyz);
                    vec3 refl = normalize(reflect(v,n));
                    if(dot(v,n)>0.0 )
                        color+= max(specularCoeff * lightsIntensity[i].rgb * pow(dot(refl,u),objColors[sourceIndx].a),vec3(0.0,0.0,0.0));  //specular  
                    color+= max(diffuseFactor * objColors[sourceIndx].rgb * lightsIntensity[i].rgb * dot(v,n),vec3(0.0,0.0,0.0)) ;  //difuse
                    //        color = vec3(1.0,1.0,0.0);
                }
                else  //plane
                {
                    vec3 n = normalize(objects[sourceIndx].xyz);
                    vec3 refl = normalize(reflect(v,n));
                    
                    color = min(color + max(specularCoeff * lightsIntensity[i].rgb * pow(dot(refl,u),objColors[sourceIndx].a),vec3(0.0,0.0,0.0)),vec3(1.0,1.0,1.0)); //specular
                    color = min( color + max(diffuseFactor * objColors[sourceIndx].rgb * lightsIntensity[i].rgb * dot(v,n),vec3(0.0,0.0,0.0)),vec3(1.0,1.0,1.0)); //difuse
                 
                  //      color = vec3(1.0,1.0,0.0);
                }
            }
         //   else if(indx == 1)
          //          color = lightsIntensity[i].rgb;
            
        }
        else  //flashlight
        {
            int indx = -1;
            v = -normalize(lightsPosition[i].xyz - sourcePoint);
            if(dot(v,normalize(lightsDirection[i].xyz))<lightsPosition[i].w)
            {
                continue;
            }
            else
            {
                //vec3 u = normalize(sourcePoint - eye.xyz);
                float t = intersection(indx,lightsPosition[i].xyz,v);
                if(indx <= sizes.w || indx == sourceIndx) //no intersection
                {
                    if(objects[sourceIndx].w > 0) //sphere
                    {
                        vec3 n = -normalize( sourcePoint - objects[sourceIndx].xyz);
                        vec3 refl = normalize(reflect(v,n));
                        if(dot(v,n)>0.0)
                          color+=max(specularCoeff * lightsIntensity[i].rgb * pow(dot(refl,u),objColors[sourceIndx].a),vec3(0.0,0.0,0.0)); //specular
                        color+= max(diffuseFactor * objColors[sourceIndx].rgb * lightsIntensity[i].rgb * dot(v,n),vec3(0.0,0.0,0.0));
                      //          color = vec3(1.0,1.0,0.0);            
                    }
                    else  //plane
                    {

                        vec3 n = normalize(objects[sourceIndx].xyz);
                        vec3 refl = normalize(reflect(v,n)); //specular
                        color = min(color + max(specularCoeff * lightsIntensity[i].rgb * pow(dot(refl,u),objColors[sourceIndx].a),vec3(0.0,0.0,0.0)),vec3(1.0,1.0,1.0));
                        color = min(color + max(diffuseFactor * objColors[sourceIndx].rgb * lightsIntensity[i].rgb *dot(v,n),vec3(0.0,0.0,0.0)),vec3(1.0,1.0,1.0));
                       // color = vec3(1.0,1.0,0.0);
                    }
                }
                //else if(indx == 1)
                //    color = lightsIntensity[i].rgb;
            }
        }
    }
         //   color = vec3(1.0,1.0,0.0);
    return min(color,vec3(1.0,1.0,1.0));
}

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 brakeLight(vec3 v,vec3 n,float myu){
    return sqrt(1-(myu*myu)*(1-(dot(n,v)*dot(n,v))))*n + myu*(v-dot(n,v)*n);
}

vec4 matMult(vec4[9] first,mat3 second){
    vec4 ans = vec4(0,0,0,0);
    for(int i=0; i < 9;i++){
        ans += first[i] * second[(i/3)][i%3];
    }
    return ans;

}

vec4 calcRay(vec3 pos){
    int indx = -1;
    vec3 position = vec3(pos.x + eye.x, pos.y + eye.y, eye.z - 1.0f);

    // rotation
    position = position - eye.xyz;
    position =  (rotationMatrix(vec3(0,1,0), camRot.x) * rotationMatrix(vec3(1,0,0), camRot.y) * vec4(position, 1)).xyz;
    position = position + eye.xyz;
    
    vec3 v = normalize( position - eye.xyz);
    float t = intersection(indx,eye.xyz ,v);
    if(indx < 0){
       return vec4(37.0/255.0, 94.0/ 255.0, 148.0/ 255.0,1.0);
    }
    else
    {
        //mirror
       int counter = 5;
        vec3 p = eye.xyz + t*v;
        vec3 n;
        while(counter>0 && indx<sizes.z + sizes.w) {
            if(indx < 0){
                return vec4(37.0/255.0, 94.0/ 255.0, 148.0/ 255.0,1.0);
            }
            else if (indx < sizes.w) {// transperant
                n = normalize(objects[indx].xyz - p);
                //first in brake light
                v = normalize(brakeLight(v,n,1/1.5));
                t = max(0,innerIntersection(indx,p,v));
                p = p + t * v;
                //find new normal
                n = normalize(p - objects[indx].xyz);
                //second out brake light
                v = normalize(brakeLight(v,n,1.5/1));
                t = intersection(indx,p,v);
                p = p + t*v;
                counter--;
            }
            else{                   // reflective
                if(objects[indx].w <=0)
                    n = normalize(objects[indx].xyz);
                else 
                    n = normalize(p - objects[indx].xyz);
                v = normalize(reflect(v,n));
                t = intersection(indx,p,v);
                counter--;
                p = p + t*v;
            }
        }

        float x = p.x;//max(abs(p.x),abs(p.y))*sign(p.x+p.y);
        float y = p.y;//max(min(abs(p.y),abs(p.x)),abs(p.z))*sign(min(abs(p.y),abs(p.x))+p.z);
        
        //if(objects[indx].w <= 0 && (mod(int(abs(1.5*x)),2) == mod(int(abs(1.5*y)),2)))
        if(objects[indx].w <= 0 && (((mod(int(1.5*x),2) == mod(int(1.5*y),2)) && ((x>0 && y>0) || (x<0 && y<0))) || ((mod(int(1.5*x),2) != mod(int(1.5*y),2) && ((x<0 && y>0) || (x>0 && y<0))))))
            return vec4(colorCalc(indx,p,v,0.5),1);
        else 
            return vec4(colorCalc(indx,p,v,1.0),1);      
    }
}

void main()
{  
    vec4[9] colors;
    float stepConst = 0.001;
    for(int i = 0; i < 9; i++) {
        colors[i] = calcRay(position0 + vec3(stepConst* ((i%3)-1),stepConst*int((i/3)-1),0));
    }
    mat3 gaussian;
    gaussian[0] = vec3(1,4,1);
    gaussian[1] = vec3(4,16,4);
    gaussian[2] = vec3(1,4,1);
    gaussian = gaussian*0.02777777;
    gl_FragColor = matMult(colors,gaussian);
}
 


