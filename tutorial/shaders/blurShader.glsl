#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;
in float shapeIndx;
uniform vec2 transparency;
in float r;//Radius of blur
in float sigma;
in vec2 texResolution;

uniform vec4 lightColor;
uniform sampler2D sampler1;
uniform vec4 lightDirection;

out vec4 Color;

vec2 toOtherCoords(vec2 texCoords){
    return vec2(texCoords.x*texResolution.x-0.5*texResolution.x,texCoords.y*texResolution.y-0.5*texResolution.y);
}

vec2 toTexCoords(vec2 otherCoords){
    return vec2((otherCoords.x+0.5*texResolution.x)/texResolution.x,(otherCoords.y+0.5*texResolution.y)/texResolution.y);
}

void main()
{
    float pi=3.14159265359;
    float gConstant=1/(2*pi*sigma*sigma);
    vec4 col=vec4(0,0,0,1);
    vec2 startPos = toOtherCoords(texCoord0);
    float x = startPos.x-r;
    float y = startPos.y-r;
    float weight;
    while(x<=startPos.x+r && y<=startPos.y+r){
        float relX = x-startPos.x;
        float relY = y-startPos.y;
        float expMone=relX*relX+relY*relY;
        float expMehane=2*sigma*sigma;
        weight=gConstant*exp(-expMone/expMehane);
        vec2 texC = toTexCoords(vec2(x,y));
        col+=texture2D(sampler1,clamp(texC,0,1))*weight;
        if(x>=startPos.x+r){
            y++;
            x = startPos.x-r;
        }else{
            x++;
        }        
    }
    Color=col * vec4(1,1,1, transparency.x);
}

