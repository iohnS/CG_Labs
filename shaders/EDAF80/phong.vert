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

uniform sampler2D normal_map;
uniform sampler2D specular_map;
uniform sampler2D diffuse_map;

out VS_OUT{
    vec3 fN;
    vec3 fV;
    vec3 fL;
    vec2 fTexCoord;
    vec3 t;
    vec3 b;
    vec3 r_n;
} vs_out;


void main()
{
    vs_out.fTexCoord = tex_coord;
    vs_out.t = tangent;
    vs_out.b = bitangent;
    //vs_out.b = cross(normal, tangent);
    //vs_out.fN = normal;
    //if (use_normal_mapping) {
    //    vs_out.fN = normalize(normal_model_to_world * vec4(normal, 0.0)).xyz;
    //} else {
    //    vs_out.fN = normal;
    //}
    //vs_out.fN = texture(normal_map, tex_coord).rgb * 2.0 - vec3(1.0, 1.0, 1.0);
    vs_out.fN = normal;
    vs_out.r_n = texture(normal_map, tex_coord).rgb * 2.0 - vec3(1.0, 1.0, 1.0);
    vec3 worldPos = vec3(vertex_model_to_world * vec4(vertex, 1.0));
    vs_out.fV = camera_position - worldPos;
    vs_out.fL = light_position - worldPos;
	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}

