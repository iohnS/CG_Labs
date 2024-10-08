#version 410

uniform vec3 ambient_colour;
uniform vec3 diffuse_colour; 
uniform vec3 specular_colour;
uniform float shininess_value;

uniform vec3 light_position;
uniform samplerCube phong_cubemap;

in vec3 fN; 
in vec3 fL; 
in vec3 fV; 

out vec4 fColor;

void main()
{
    vec3 N = normalize(fN);
    vec3 L = normalize(fL);
    vec3 V = normalize(fV);
    vec3 R = normalize(reflect(-L,N));
    vec3 res_diffuse = diffuse_colour*max(dot(N,L),0.0);
    vec3 res_specular = specular_colour*pow(max(dot(R,V),0.0), shininess_value);
    fColor.xyz = (ambient_colour + res_diffuse + res_specular) * texture(phong_cubemap, fN).xyz;
    fColor.w = 1.0;
}
