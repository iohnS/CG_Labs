#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 vertex_model_to_world;  // World
uniform mat4 normal_model_to_world;  // WorldIT
uniform mat4 vertex_world_to_clip;   // ModelViewProj

uniform vec3 light_position; 
uniform vec3 camera_position;  
uniform bool use_normal_mapping;

out VS_OUT{
    vec3 fN;
    vec3 fV;
    vec3 fL;
    vec2 tex_coord;
    mat3 TBN;
} vs_out;


void main()
{
    if(use_normal_mapping){
        vec3 T = normalize(vec3(vertex_model_to_world * vec4(tangent, 0.0)));
        vec3 B = normalize(vec3(vertex_model_to_world * vec4(bitangent, 0.0)));
        vec3 N = normalize(vec3(normal_model_to_world * vec4(normal, 0.0)));
        vs_out.TBN = mat3(T, B, N);
    }

    vs_out.tex_coord = tex_coord;
    vs_out.fN = vec3(normal_model_to_world * vec4(normal, 0.0));

    vec3 worldPos = vec3(vertex_model_to_world * vec4(vertex, 1.0));
    vs_out.fV = camera_position - worldPos;
    vs_out.fL = light_position - worldPos;
	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}

