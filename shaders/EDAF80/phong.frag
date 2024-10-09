#version 410

uniform vec3 ambient_colour;
uniform vec3 diffuse_colour; 
uniform vec3 specular_colour;
uniform float shininess_value;
uniform bool use_normal_mapping;
uniform vec3 light_position;

uniform sampler2D normal_map;
uniform sampler2D specular_map;
uniform sampler2D diffuse_map;

in VS_OUT
{
    vec3 fN;
    vec3 fV;
    vec3 fL;
    vec2 tex_coord;
    mat3 TBN;
} fs_in;

out vec4 fColor;

void main()
{
    vec3 N = normalize(fs_in.fN);
    if(use_normal_mapping)
    {
        vec3 normal = texture(normal_map, fs_in.tex_coord).xyz * 2.0 - 1.0;
        N = normalize(fs_in.TBN * normal);
    }
    vec3 L = normalize(fs_in.fL);
    vec3 V = normalize(fs_in.fV);
    vec3 R = normalize(reflect(-L,N));

    // Diffuse
    vec3 diffuse = diffuse_colour * texture(diffuse_map, fs_in.tex_coord).xyz * max(dot(N,L),0.0);

    // Specular
    vec3 specular = specular_colour * texture(specular_map, fs_in.tex_coord).xyz * pow(max(dot(R,V),0.0), shininess_value);

    fColor.xyz = (ambient_colour + diffuse + specular);
    fColor.w = 1.0;
}
