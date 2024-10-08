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
uniform mat4 vertex_model_to_world;  // World
uniform mat4 normal_model_to_world;  // WorldIT
uniform mat4 vertex_world_to_clip;   // ModelViewProj

in VS_OUT
{
    vec3 fN;
    vec3 fV;
    vec3 fL;
    vec2 fTexCoord;
    vec3 t;
    vec3 b;
    vec3 r_n;
} fs_in;

out vec4 fColor;

void main()
{

    vec3 n = normalize(fs_in.r_n);
    vec3 t = normalize(fs_in.t);
    vec3 b = normalize(fs_in.b);
    vec3 fN;

    if(use_normal_mapping){
        mat3 tbn = mat3(t, b, n);
        vec3 transformed_normal = mat3(normal_model_to_world) * tbn * n;
        fN = transformed_normal;
    } else {
        fN = vec3(normal_model_to_world * vec4(fs_in.fN, 0.0));
    }

    vec3 N = normalize(fN);
    vec3 L = normalize(fs_in.fL);
    vec3 V = normalize(fs_in.fV);
    vec3 R = normalize(reflect(-L,N));

    vec3 diffuse_tex = texture(diffuse_map, fs_in.fTexCoord).rgb;
    vec3 specular_tex = texture(specular_map, fs_in.fTexCoord).rgb;

    vec3 res_diffuse = diffuse_colour * max(dot(N,L),0.0) ;
    vec3 res_specular = specular_colour * pow(max(dot(R,V),0.0), shininess_value);
    fColor.xyz = (ambient_colour + res_diffuse * diffuse_tex + res_specular * specular_tex);
    fColor.w = 1.0;
}
