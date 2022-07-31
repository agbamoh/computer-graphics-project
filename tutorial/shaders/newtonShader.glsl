#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler2D sampler1;
uniform vec4 lightDirection;
uniform float time;
uniform float x;
uniform float y;

uniform int iterNum;
uniform vec4 coeffsReal;
uniform vec4 coeffsImag;
uniform float fxStart;
uniform float fxEnd;
uniform float fyStart;
uniform float fyEnd;
uniform float scrnXWidth;
uniform float scrnXHeight;
uniform vec4 root1;
uniform vec4 color1;
uniform vec4 root2;
uniform vec4 color2;
uniform vec4 root3;
uniform vec4 color3;
uniform float threashold;


out vec4 Color;

vec4 cMul(vec4 num1,vec4 num2){
    vec4 res = vec4(0,0,0,0);
    res.x = num1.x * num2.x - num1.y * num2.y;
    res.y = num1.x * num2.y + num1.y * num2.x;
    return res;
}

vec4 tzamud(vec4 num){
    return vec4(num.x,-num.y, 0, 0);
}

vec4 cSubD(vec4 num1,vec4 num2){
    return vec4(cMul(num1,tzamud(num2)))/cMul(num2,tzamud(num2)).x;
}

vec4 cPow(vec4 num,float pow){
    vec4 res = vec4(1,0,0,0);
    for(int i=0;i<pow;i++){
        res = cMul(res,num);
    }
    return res;
}

vec4 f(vec4 z) {
    vec4 a = vec4(coeffsReal[0], coeffsImag[0],0 ,0);
    vec4 b = vec4(coeffsReal[1], coeffsImag[1],0 ,0);
    vec4 c = vec4(coeffsReal[2], coeffsImag[2],0 ,0);
    vec4 d = vec4(coeffsReal[3], coeffsImag[3],0 ,0);
    return cMul(a,cPow(z,3)) + cMul(b,cPow(z,2)) + cMul(c,z)+d;
}

vec4 df(vec4 z){
    vec4 a = vec4(coeffsReal[0], coeffsImag[0],0 ,0);
    vec4 b = vec4(coeffsReal[1], coeffsImag[1],0 ,0);
    vec4 c = vec4(coeffsReal[2], coeffsImag[2],0 ,0);
    return 3*cMul(a,cPow(z,2)) + 2*cMul(b,z)+c;
}

float currIter = iterNum -1;

vec4 findIterPos() {
    vec4 z = vec4(texCoord0.s,texCoord0.t, 0, 0);
    //Rescale pixel according to current x,y coordinates
    z = vec4(fxEnd-z.x*(fxEnd-fxStart),fyEnd-z.y*(fyEnd-fyStart), 0, 0);
    for(int i=0;i < iterNum;i++) {
        vec4 fz = f(z);
        vec4 dfz = df(z);
        z = z - cSubD(fz,dfz);
        if(distance(z,root1)<threashold||distance(z,root2)<threashold||distance(z,root3)<threashold) {
            currIter = i;
            break;
        }
    }
    return z;
}

void main()
{
    vec4 finalZ = findIterPos();
    float dist1 = distance(finalZ,root1);
    float dist2 = distance(finalZ,root2);
    float dist3 = distance(finalZ,root3);
    float sub = 1-float(currIter/iterNum)*0.5;
    if(dist1<=dist2 && dist1<=dist3){
        //1 is minimal
        // Color = vec4(color1.r*0.5,color1.g*0.1,color1.b*0.1,1);
        Color = vec4(color1*sub);
    }else if(dist2<=dist1 && dist2<=dist3){
        //2 is minimal
        // Color = vec4(color2.r*0.1,color2.g*0.1,color2.b*0.1,1);
        Color = vec4(color2*sub);
    }else if(dist3<=dist1 && dist3<=dist2){
        //3 is minimal
        // Color = vec4(color3.r*0.1,color3.g*0.1,color3.b*0.1,1);
        Color = vec4(color3*sub);
    }
    
    // Color = vec4(clamp(Color.r,0,255),clamp(Color.g,0,255),clamp(Color.b,0,255),1);
}
