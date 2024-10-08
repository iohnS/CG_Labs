uniform vec3 ambient;
uniform vec3 diffuse; 
uniform vec3 specular;
uniform samplerCube phong_cubemap;

uniform float shininess;

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
    vec3 res_diffuse = diffuse*max(dot(N,L),0.0);
    vec3 res_specular = specular*pow(max(dot(R,V),0.0), shininess);
    fColor.xyz = (ambient + res_diffuse + res_specular) * texture(cubemap, fN).xyz;
    fColor.w = 1.0;
}