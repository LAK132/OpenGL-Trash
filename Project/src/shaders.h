#include <string>

std::string vshaderstr = R"(
#version 330 core

in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
in vec2 vTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec4 fColor;
out vec3 fNormal;
out vec2 fTexCoord;
out vec3 fPosition;
out vec3 fEye;

const vec4 WUP = vec4(0.0, 0.0, 0.0, 1.0);

void main()
{
    mat4 projview = projection * view;
    mat4 invprojview = transpose(inverse(projview));
    vec4 vertpos = model * vPosition; // object -> world space

    fTexCoord = vTexCoord;
    // fColor = vPosition;
    fColor = vColor;
    fEye = vec3(WUP * invprojview); // screen -> camera -> world space
    fNormal = mat3(model) * vNormal; // object -> world space (no translation/scale)
    fPosition = vertpos.xyz;

    gl_Position = projview * vertpos; // world -> camera -> screen space
})";

std::string fshaderstr = R"(
#version 330 core

smooth in vec4 fColor;
smooth in vec3 fEye;
smooth in vec3 fNormal;
smooth in vec3 fPosition;
smooth in vec2 fTexCoord;

uniform sampler2D albedo;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float shininess;

#define MAX_LIGHTS 6
struct light {
    vec3 position;
    vec4 color;
};
uniform int lightCount;
uniform light lights[MAX_LIGHTS];

out vec4 pColor;

void main() 
{
    // pColor = texture(albedo, fTexCoord);
    vec3 normal = normalize(fNormal);
    vec3 viewDir = normalize(fEye - fPosition);
    vec4 baseColor = texture(albedo, fTexCoord); // * fColor;

    pColor = ambient * baseColor;

    for(int i = 0; i < lightCount && i < MAX_LIGHTS; i++)
    {
        vec3 lightDir = normalize(lights[i].position - fPosition);
        float dNL = max(dot(normal, lightDir), 0.0f);
        vec4 color = diffuse;
        color = diffuse * baseColor;
        vec4 lambert = color * lights[i].color * dNL;

        vec3 halfVec = normalize(lightDir + viewDir);
        float dNH = max(dot(normal, halfVec), 0.0f);
        vec4 phong = specular * lights[i].color * pow(dNH, shininess);
        pColor += lambert + phong;
    }
})";